/*
 * lcd_app.h
 *
 *  Created on: Nov 4, 2024
 *      Author: Phat_Dang
 */

#ifndef APP_LCD_H_
#define APP_LCD_H_


#include <stdint.h>

void memlcd_app_init(void);

void display(int);

void memlcd_display(uint8_t line, const char *buffer);


#endif /* APP_LCD_H_ */
