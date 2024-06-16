/*
 * delay.h
 *
 *  Created on: Jun 16, 2024
 *      Author: elena
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stdint.h"

void delay_init();

void delay_us (uint16_t delay);

void delay_ms(uint16_t delay);


#endif /* INC_DELAY_H_ */
