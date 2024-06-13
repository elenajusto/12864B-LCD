/*
 * LCD12864B.c
 *
 *  Created on: Jun 13, 2024
 *      Author: elena
 */
#include "LCD12864B.h"
#include "main.h"					// Pin definitions and HAL import for current MCU

// Global variables
uint8_t GDdisplayedBank;
uint8_t GDcurrentBank;
uint8_t GDgraphicsDisplayed;
uint8_t GDleftByte, GDrightByte;
uint8_t GDprevYaddr, GDprevXaddr;

// Function to read data port (DB0-DB7 connected to various GPIO ports)
uint8_t GDreadDataPort(void) {
    uint8_t data = 0;

    // Set GPIOs to input mode
    GPIOA->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3 | GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
    GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE14);
    GPIOC->MODER &= ~(GPIO_MODER_MODE7);

    GD_RW_HIGH;
    GD_E_HIGH;
    HAL_Delay(1); // Short delay

    if (HAL_GPIO_ReadPin(DB0_GPIO_Port, DB0_Pin)) data |= (1 << 0);
    if (HAL_GPIO_ReadPin(DB1_GPIO_Port, DB1_Pin)) data |= (1 << 1);
    if (HAL_GPIO_ReadPin(DB2_GPIO_Port, DB2_Pin)) data |= (1 << 2);
    if (HAL_GPIO_ReadPin(DB3_GPIO_Port, DB3_Pin)) data |= (1 << 3);
    if (HAL_GPIO_ReadPin(DB4_GPIO_Port, DB4_Pin)) data |= (1 << 4);
    if (HAL_GPIO_ReadPin(DB5_GPIO_Port, DB5_Pin)) data |= (1 << 5);
    if (HAL_GPIO_ReadPin(DB6_GPIO_Port, DB6_Pin)) data |= (1 << 6);
    if (HAL_GPIO_ReadPin(DB7_GPIO_Port, DB7_Pin)) data |= (1 << 7);

    GD_E_LOW;
    return data;
}

// Function to write data port (DB0-DB7 connected to various GPIO ports)
void GDwriteDataPort(uint8_t b) {
    GD_E_HIGH;

    // Set GPIOs to output mode
    GPIOA->MODER |= (GPIO_MODER_MODE10_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 | GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0);
    GPIOB->MODER |= (GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE14_0);
    GPIOC->MODER |= (GPIO_MODER_MODE7_0);

    HAL_GPIO_WritePin(DB0_GPIO_Port, DB0_Pin, (b & (1 << 0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DB1_GPIO_Port, DB1_Pin, (b & (1 << 1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DB2_GPIO_Port, DB2_Pin, (b & (1 << 2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DB3_GPIO_Port, DB3_Pin, (b & (1 << 3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DB4_GPIO_Port, DB4_Pin, (b & (1 << 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DB5_GPIO_Port, DB5_Pin, (b & (1 << 5)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DB6_GPIO_Port, DB6_Pin, (b & (1 << 6)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DB7_GPIO_Port, DB7_Pin, (b & (1 << 7)) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    GD_E_LOW;
}

// Function to wait while busy
void GDwaitWhileBusy(void) {
    GD_RS_LOW;
    while (GDreadDataPort() & 0x80) {
        HAL_Delay(1); // Short delay
    }
}

// Function to write RAM
void GDramWrite(uint8_t b) {
    GDwaitWhileBusy();
    GD_RS_HIGH;
    GD_RW_LOW;
    GDwriteDataPort(b);
}

// Function to write instruction
void GDinsWrite(uint8_t b) {
    GDwaitWhileBusy();
    GD_RW_LOW;
    GDwriteDataPort(b);
}

// Function to clear graphics display
void GDgClear(void) {
    GDinsWrite(GDgraphicsDisplayed ? 0x36 : 0x34);
    GDinsWrite(0b00000010);
    uint8_t rowStart = GDcurrentBank ? 32 : 0;
    uint8_t rowFinish = GDcurrentBank ? 64 : 32;
    for (uint8_t row = rowStart; row < rowFinish; row++) {
        GDinsWrite(0b10000000 | row);
        GDinsWrite(0b10000000 | 0);
        for (uint8_t i = 0; i < 16; i++) {
            GDramWrite(0x00);
            GDramWrite(0x00);
        }
    }
    GDinsWrite(GDgraphicsDisplayed ? 0x32 : 0x30);
    GDprevXaddr = 0x10;
}

// Initialization function
void GDinit(void) {
    GD_RS_LOW;
    GD_RW_HIGH;
    GD_E_LOW;
    HAL_Delay(40);
    GDinsWrite(0x30);
    GDinsWrite(0x34);
    GDinsWrite(0x34);
    GDgraphicsDisplayed = 0;
    GDinsWrite(0x30);
    GDinsWrite(0x84);
    GDinsWrite(0x01);
    GDinsWrite(0x06);
    GDcurrentBank = 0;
    GDgClear();
    GDcurrentBank = 1;
    GDgClear();
    GDdisplayedBank = 0;
    GDprevXaddr = 0x10;
}

// Continue adapting the rest of the functions similarly

void GDflushGraphicsPixelBuffer(void) {
    if (GDprevXaddr & 0x20) {
        GDramWrite(GDleftByte);
        GDramWrite(GDrightByte);
        GDprevXaddr |= 0x10;
        GDprevXaddr &= ~0x20;
    }
}

void GDsetDDaddress(uint8_t addr) {
    GDflushGraphicsPixelBuffer();
    if (GDcurrentBank) {
        GDinsWrite(0xA0 | addr);
    } else {
        GDinsWrite(0x80 | addr);
    }
}

uint8_t GDreadRam(void) {
    GDwaitWhileBusy();
    GD_RS_HIGH;
    return GDreadDataPort();
}

void GDread16bits(void) {
    GDleftByte = GDreadRam();
    GDrightByte = GDreadRam();
}

void GDtWrite(uint8_t *text, uint8_t row, uint8_t col) {
    if (!*text) return;
    if (row > 3) row = 3;
    if (col > 15) col = 15;
    uint8_t addr = col >> 1;
    if (row & 0x01) addr += 16;
    if (row > 1) addr += 8;
    GDsetDDaddress(addr);
    uint8_t c1;
    if (col & 0x01) {
        GDreadRam();
        GDread16bits();
        c1 = GDleftByte;
        GDsetDDaddress(addr);
    } else {
        c1 = *text++;
    }
    while (addr < 32) {
        uint8_t c2 = *text;
        if (!c2) {
            GDsetDDaddress(addr);
            GDreadRam();
            GDread16bits();
            c2 = GDrightByte;
            GDsetDDaddress(addr);
        } else {
            text++;
        }
        GDramWrite(c1);
        GDramWrite(c2);
        c1 = *text++;
        if (!c1) break;
        addr++;
    }
}

void GDtCopyBank(void) {
    uint8_t currentBankCopy = GDcurrentBank;
    for (uint8_t addr = 0; addr < 32; addr++) {
        GDcurrentBank = GDdisplayedBank;
        GDsetDDaddress(addr);
        GDreadRam();
        GDread16bits();
        GDcurrentBank = currentBankCopy;
        GDsetDDaddress(addr);
        GDramWrite(GDleftByte);
        GDramWrite(GDrightByte);
    }
}

void GDtClear(void) {
    GDtWrite("  ", 0, 0);
    for (uint8_t i = 0; i < 62; i++) {
        GDramWrite(' ');
    }
}

void GDflipBanks(void) {
    GDflushGraphicsPixelBuffer();
    GDcurrentBank ^= 1;
    GDdisplayedBank ^= 1;
    GDinsWrite(GDgraphicsDisplayed ? 0x36 : 0x34);
    GDinsWrite(0x03);
    GDinsWrite(GDdisplayedBank ? 0x60 : 0x40);
    GDinsWrite(GDgraphicsDisplayed ? 0x32 : 0x30);
}

void GDdoubleBuffering(uint8_t on) {
    if (on) {
        GDcurrentBank = GDdisplayedBank ^ 1;
    } else {
        GDcurrentBank = GDdisplayedBank;
    }
}

void GDgDisplay(uint8_t graphicsOn) {
    GDgraphicsDisplayed = graphicsOn;
    GDinsWrite(GDgraphicsDisplayed ? 0x36 : 0x34);
    GDinsWrite(GDgraphicsDisplayed ? 0x36 : 0x34);
    GDinsWrite(GDgraphicsDisplayed ? 0x32 : 0x30);
}

void GDaddressPixel(uint8_t x, uint8_t y) {
    x >>= 4;
    if (y & 0x20) {
        x |= 0x08;
        y &= 0x1F;
    }
    y |= GDcurrentBank ? 0xA0 : 0x80;
    x |= 0x80;
    if ((y != GDprevYaddr) || (x != (GDprevXaddr & 0x9F))) {
        GDflushGraphicsPixelBuffer();
        GDinsWrite(GDgraphicsDisplayed ? 0x36 : 0x34);
        GDinsWrite(0x02);
        GDinsWrite(y);
        GDinsWrite(x);
        GDreadRam();
        GDread16bits();
        GDinsWrite(y);
        GDinsWrite(x);
        GDinsWrite(GDgraphicsDisplayed ? 0x32 : 0x30);
        GDprevYaddr = y;
        GDprevXaddr = x;
    }
}

void GDgWritePixel(uint8_t x, uint8_t y, uint8_t mode) {
    if (x & 0x80 || y & 0xC0) return;
    GDaddressPixel(x, y);
    uint8_t *rightOrLeft = x & 0x08 ? &GDrightByte : &GDleftByte;
    uint8_t pixelMask = 0x80 >> (x & 0x07);
    switch (mode) {
        case 0:
            *rightOrLeft &= ~pixelMask;
            break;
        case 1:
            *rightOrLeft |= pixelMask;
            break;
        default:
            *rightOrLeft ^= pixelMask;
            break;
    }
    if (((GDprevXaddr & 0x10) == 0x00) && (GDdisplayedBank != GDcurrentBank)) {
        GDprevXaddr |= 0x20;
    } else {
        GDflushGraphicsPixelBuffer();
    }
}

uint8_t GDgReadPixel(uint8_t x, uint8_t y) {
    if (x & 0x80 || y & 0xC0) return 0;
    GDaddressPixel(x, y);
    uint8_t *rightOrLeft = x & 0x08 ? &GDrightByte : &GDleftByte;
    uint8_t pixelMask = 0x80 >> (x & 0x07);
    return (*rightOrLeft & pixelMask) != 0x00;
}
