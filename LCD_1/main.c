//#############################################################################
//
// FILE:   main.c
//

//#############################################################################

#define     LCM_PIN_MASK  ((LCM_PIN_RS | LCM_PIN_EN | LCM_PIN_D7 | LCM_PIN_D6 | LCM_PIN_D5 | LCM_PIN_D4))
#define     FALSE                 0
#define     TRUE                  1
#define     LCD_3V_PRESENT        (bool) true

// Included Files


#include "driverlib.h"
#include "device.h"
#include "LCD.h"


// Main
void main(void){
#ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif
    // Device Initialization
    //clock set to 100MHz (ext osc 20MHz)
        while(!SysCtl_setClock(SYSCTL_PLL_ENABLE | SYSCTL_SYSDIV(1) | SYSCTL_IMULT(5)|
                            SYSCTL_OSCSRC_XTAL)){
                SysCtl_resetMCD();
            };
    initLCDpins();
    LCD_Init();


   // GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    //GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);

    // Loop Forever



    LCD_Clear();
     while(1)
     {
         // Turn on LED
         //GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0);

         // Delay for a bit.
         //DEVICE_DELAY_US(500000);

         // Turn off LED
         //GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);

         // Delay for a bit.
//         DEVICE_DELAY_US(500000);


         LCD_Set_Cursor(1,1);
         LCD_Write_String("LCD Library for");
         LCD_Set_Cursor(2,1);
         LCD_Write_String("C2000 Ware");
         DEVICE_DELAY_US(20000);
         LCD_Clear();
         LCD_Set_Cursor(1,1);
         LCD_Write_String("brrr");
         DEVICE_DELAY_US(20000);


     }
}

//
// End of File
//
