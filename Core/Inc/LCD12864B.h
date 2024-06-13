/*
 * LCD12864B.h
 *
 *  Created on: Jun 13, 2024
 *      Author: elena
 */

#ifndef INC_LCD12864B_H_
#define INC_LCD12864B_H_

/*
 * INCLUDES
 */
#include "main.h"		// Pin definitions and HAL import for current MCU

/*
 * LOW LEVEL FUNCTIONS
 * Define macros to set and clear GPIO pins using HAL functions
 */
#define GD_RS_HIGH  HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_SET)
#define GD_RS_LOW   HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, GPIO_PIN_RESET)
#define GD_RW_HIGH  HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, GPIO_PIN_SET)
#define GD_RW_LOW   HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, GPIO_PIN_RESET)
#define GD_E_HIGH   HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_SET)
#define GD_E_LOW    HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, GPIO_PIN_RESET)

/*
 * EXTERNAL FUNCTIONs
 */
void GDinit(void);
void GDflipBanks(void);
void GDdoubleBuffering(uint8_t on);
void GDtClear(void);
void GDtWrite(uint8_t *text, uint8_t row, uint8_t col);
void GDtCopyBank(void);
void GDgDisplay(uint8_t graphicsOn);
void GDgClear(void);
void GDgWritePixel(uint8_t x, uint8_t y, uint8_t mode);
uint8_t GDgReadPixel(uint8_t x, uint8_t y);

#endif /* INC_LCD12864B_H_ */
