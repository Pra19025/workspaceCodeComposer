/*
 * LCD.h
 *
 *  Created on: 26/02/2022
 *      Author: Noel Prado
 */

#ifndef LCD_H_
#define LCD_H_


/*** Standard includes ***/
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>


/*** Dependencies ***/
#include <driverlib.h>


/*** Defines ***/

void initLCDpins(void);
void LCD_CMD (char a);
void LCD_Init(void);
void LCD_Write_Char(char a);
void LCD_Write_String(char *a);
void LCD_Clear(void);
void LCD_Port(char a);
void LCD_Set_Cursor(char a, char b);



#endif /* LCD_H_ */
