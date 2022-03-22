/*
 * lcd_lib.c
 *
 *  Created on: 26/02/2022
 *      Author: Noel Prado
 */




#include <pinout.h>
#include "LCD.h"
#include "driverlib.h"
#include "device.h"



#define LCD_BACK_LIGHT_CONF GPIO_1_GPIO1

/*** Defines ***/
#define     LCM_PIN_MASK  ((LCM_PIN_RS | LCM_PIN_EN | LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5 | LCM_PIN_D4))
#define     FALSE                 0
#define     TRUE                  1
#define     LCD_3V_PRESENT        (bool) true

void initLCDpins(void){
    //configuracion inicial de los pines
 GPIO_setPinConfig(LCD_PIN_D7_CONF);
 GPIO_setPinConfig(LCD_PIN_D6_CONF);
 GPIO_setPinConfig(LCD_PIN_D5_CONF);
 GPIO_setPinConfig(LCD_PIN_D4_CONF);
 GPIO_setPinConfig(LCD_PIN_EN_CONF);
 GPIO_setPinConfig(LCD_PIN_RS_CONF);
 GPIO_setPinConfig(LCD_PIN_PWR_CONF);
 GPIO_setPinConfig(LCD_PIN_BACK_LIGHT_CONF);


 //configurando entradas y salidas de los pines
 GPIO_setDirectionMode(LCD_PIN_D7, GPIO_DIR_MODE_OUT);
 GPIO_setDirectionMode(LCD_PIN_D6, GPIO_DIR_MODE_OUT);
 GPIO_setDirectionMode(LCD_PIN_D5, GPIO_DIR_MODE_OUT);
 GPIO_setDirectionMode(LCD_PIN_D4, GPIO_DIR_MODE_OUT);
 GPIO_setDirectionMode(LCD_PIN_EN, GPIO_DIR_MODE_OUT);
 GPIO_setDirectionMode(LCD_PIN_RS, GPIO_DIR_MODE_OUT);
 GPIO_setDirectionMode(LCD_PIN_PWR, GPIO_DIR_MODE_OUT);
 GPIO_setDirectionMode(LCD_PIN_BACK_LIGHT, GPIO_DIR_MODE_OUT);


}

void LCD_Port(char a){

    if(a & 1)
        GPIO_writePin(LCD_PIN_D4, 1);
    else
        GPIO_writePin(LCD_PIN_D4, 0);

    if(a & 2)
        GPIO_writePin(LCD_PIN_D5, 1);
    else
        GPIO_writePin(LCD_PIN_D5, 0);

    if(a & 4)
        GPIO_writePin(LCD_PIN_D6, 1);
    else
        GPIO_writePin(LCD_PIN_D6, 0);

    if(a & 8)
        GPIO_writePin(LCD_PIN_D7, 1);
    else
        GPIO_writePin(LCD_PIN_D7, 0);

}

void LCD_CMD (char a){

    GPIO_writePin(LCD_PIN_RS, 0);
    LCD_Port(a);
    GPIO_writePin(LCD_PIN_EN, 1);
    DEVICE_DELAY_US(4000);
    GPIO_writePin(LCD_PIN_EN, 0);

}

void LCD_Clear(void){
    LCD_CMD(0);
    LCD_CMD(1);



}


void LCD_Set_Cursor(char a, char b){

    char temp, z ,y;
    if(a == 1){
        temp = 0x80 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        LCD_CMD(z);
        LCD_CMD(y);


    }
    else if(a == 2){
        temp = 0xC0 + b - 1;
        z = temp >> 4;
        y = temp & 0x0F;
        LCD_CMD(z);
        LCD_CMD(y);
    }
}

void LCD_Init(){

    LCD_Port(0x00);
    DEVICE_DELAY_US(20000);
    LCD_CMD(0x03);
    DEVICE_DELAY_US(5000);
    LCD_CMD(0x03);
    DEVICE_DELAY_US(500);
    LCD_CMD(0x03);

    /////////////
    LCD_CMD(0x02);
    LCD_CMD(0x02);
    LCD_CMD(0x08);
    LCD_CMD(0x00);
    LCD_CMD(0x011);
    LCD_CMD(0x00);
    LCD_CMD(0x01);
    LCD_CMD(0x00);
    LCD_CMD(0x06);

}

void LCD_Write_Char(char a){

    char temp, y;
    temp = a&0x0F;
    y = a&0xF0;
    GPIO_writePin(LCD_PIN_RS, 1);
    LCD_Port(y>>4);
    GPIO_writePin(LCD_PIN_EN, 1);
    DEVICE_DELAY_US(40000);
    GPIO_writePin(LCD_PIN_EN, 0);
    LCD_Port(temp);
    DEVICE_DELAY_US(40000);
    GPIO_writePin(LCD_PIN_EN, 1);
    DEVICE_DELAY_US(40000);
    GPIO_writePin(LCD_PIN_EN, 0);

}

void LCD_Write_String(char *a){
    int i;
    for(i = 0; a[i]!='\0';i++)
        LCD_Write_Char(a[i]);
}

