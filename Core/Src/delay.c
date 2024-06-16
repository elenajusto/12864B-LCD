/*
 * delay.c
 *
 *  Created on: Jun 16, 2024
 *      Author: elena
 */

/* TIM 1 to create a delay in microseconds
 * TIM2 Running at 72 MHz
 * Prescalar is set to 72-1 to reduce it to 1 MHz
 * ARR is set to MAX i.e. 0xffff
 * Rest is unchanged
 */

#include "delay.h"
#include "stm32g0xx.h"

extern TIM_HandleTypeDef htim1;


void delay_init ()
{
	HAL_TIM_Base_Start(&htim1);  					// Change according to setup
}

void delay_us (uint16_t delay)
{
	__HAL_TIM_SET_COUNTER(&htim1, 0);  				// Reset the counter
	while ((__HAL_TIM_GET_COUNTER(&htim1))<delay);  // Wait for the delay to complete
}

void delay_ms(uint16_t delay)
{
	HAL_Delay (delay);
}
