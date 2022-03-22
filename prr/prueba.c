
// Included Files
//
#include "driverlib.h"
#include "device.h"


#include "i2cLib_FIFO_master_slave_interrupt.h"

//
// Defines
//
#define I2CA_ADDRESS   0x30
#define I2CB_ADDRESS   0x40

//!  --------------------------------
//!    Signal   |  I2CA   |  I2CB
//!  --------------------------------
//!     SCL     | DEVICE_GPIO_PIN_SCLA   |  DEVICE_GPIO_PIN_SCLB
//!     SDA     | DEVICE_GPIO_PIN_SDAA   |  DEVICE_GPIO_PIN_SDAB
//!  --------------------------------


//
// Globals
//
uint16_t status = 0;

struct I2CHandle I2CA;
struct I2CHandle I2CB;

//
// Function Prototypes
//
__interrupt void i2cAISR(void);
__interrupt void i2cAFIFOISR(void);

__interrupt void i2cBISR(void);
__interrupt void i2cBFIFOISR(void);

uint16_t AvailableI2C_slaves[MAX_I2C_IN_NETWORK];

uint16_t I2CA_TXdata[MAX_BUFFER_SIZE];
uint16_t I2CB_TXdata[MAX_BUFFER_SIZE];

uint16_t I2CA_RXdata[MAX_BUFFER_SIZE];
uint16_t I2CB_RXdata[MAX_BUFFER_SIZE];

uint32_t I2CA_ControlAddr;
uint32_t I2CB_ControlAddr;
uint16_t status;

void I2C_GPIO_init(void);
void I2Cinit(void);

//
// Main
//
void main(void)
{

    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();

    //
    // Board initialization
    //

    I2C_GPIO_init();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    I2Cinit();


    I2C_setOwnSlaveAddress(I2CA_BASE, I2CA_ADDRESS);
    I2C_setOwnSlaveAddress(I2CB_BASE, I2CB_ADDRESS);



    //I2Cs connected to I2CA will be found in AvailableI2C_slaves buffer
    //after you run I2CBusScan function.
    //When you run I2C BusScan you need to disable I2C interrupts and clear
    //the flag set during I2CBusScan
    uint16_t i;

    for(i=0;i<MAX_I2C_IN_NETWORK;i++)
    {
        AvailableI2C_slaves[i] = 0;
    }

    I2C_disableInterrupt(I2CA_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));
    I2C_disableInterrupt(I2CB_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));

    uint16_t *pAvailableI2C_slaves = AvailableI2C_slaves;
    status = I2CBusScan(I2CA_BASE, pAvailableI2C_slaves);

    I2C_clearStatus(I2CA_BASE,I2C_STS_NO_ACK|I2C_STS_ARB_LOST|I2C_STS_REG_ACCESS_RDY|I2C_STS_STOP_CONDITION);
    I2C_clearStatus(I2CB_BASE,I2C_STS_NO_ACK|I2C_STS_ARB_LOST|I2C_STS_REG_ACCESS_RDY|I2C_STS_STOP_CONDITION);

    ESTOP0;
    I2C_disableInterrupt(I2CA_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));
    I2C_disableInterrupt(I2CB_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));

    status = I2CBusScan(I2CB_BASE, pAvailableI2C_slaves);

    I2C_clearStatus(I2CA_BASE,I2C_STS_NO_ACK|I2C_STS_ARB_LOST|I2C_STS_REG_ACCESS_RDY|I2C_STS_STOP_CONDITION);
    I2C_clearStatus(I2CB_BASE,I2C_STS_NO_ACK|I2C_STS_ARB_LOST|I2C_STS_REG_ACCESS_RDY|I2C_STS_STOP_CONDITION);


    I2C_enableInterrupt(I2CA_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));
    I2C_enableInterrupt(I2CB_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));


    //
    // Set I2C use, initializing it for FIFO mode
    //

    Interrupt_register(INT_I2CA, &i2cAISR);
    Interrupt_enable(INT_I2CA);

    Interrupt_register(INT_I2CB, &i2cBISR);
    Interrupt_enable(INT_I2CB);

    Interrupt_register(INT_I2CA_FIFO, &i2cAFIFOISR);
    Interrupt_enable(INT_I2CA_FIFO);

    Interrupt_register(INT_I2CB_FIFO, &i2cBFIFOISR);
    Interrupt_enable(INT_I2CB_FIFO);

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;


    for(i=0;i<MAX_BUFFER_SIZE;i++)
    {
        I2CA_TXdata[i] = i+1;
        I2CA_RXdata[i] = 0;
        I2CB_TXdata[i] = 0;
        I2CB_RXdata[i] = 0;
    }

    I2CA.currentHandlePtr = &I2CA;
    I2CA.base             = I2CA_BASE;
    I2CA.SlaveAddr        = I2CB_ADDRESS;
    I2CA.pControlAddr     = &I2CA_ControlAddr;
    I2CA.NumOfAddrBytes   = 4;
    I2CA.pTX_MsgBuffer    = I2CA_TXdata;
    I2CA.pRX_MsgBuffer    = I2CA_RXdata;
    I2CA.NumOfDataBytes   = 64;

    I2CB.currentHandlePtr = &I2CB;
    I2CB.base             = I2CB_BASE;
    I2CB.SlaveAddr        = I2CB_ADDRESS;
    I2CB.NumOfAddrBytes   = 4;
    I2CB.pControlAddr     = (uint32_t *)0;
    I2CB.pTX_MsgBuffer    = (uint16_t *)0;
    I2CB.pRX_MsgBuffer    = (uint16_t *)0;




    // Example3: I2CB as Master Transmitter and I2CA working Slave Receiver //

            // I2CB = Master Transmitter
            // I2CA = Slave Receiver
            // I2CB generates
            // 1) START condition
            // 2) I2CA address (Slave address) + Write mode
            // 3) Transmit start address of I2CA_RXdata
            // 4) Transmit contents of I2CB_TXdata array
            // 5) I2CA received data is stored in I2CA_RXdata array
            // 6) Contents of I2CB_TXdata and I2CA_RXdata should match

        for(i=0;i<MAX_BUFFER_SIZE;i++)
        {
           I2CA_TXdata[i] = 0;
           I2CA_RXdata[i] = 0;
           I2CB_TXdata[i] = i+1;
           I2CB_RXdata[i] = 0;
        }

        I2CB.currentHandlePtr = &I2CB;
        I2CB.base             = I2CB_BASE;
        I2CB.SlaveAddr        = I2CA_ADDRESS;
        I2CB.pControlAddr     = &I2CB_ControlAddr;
        I2CB.NumOfAddrBytes   = 4;
        I2CB.pTX_MsgBuffer    = I2CB_TXdata;
        I2CB.pRX_MsgBuffer    = I2CB_RXdata;
        I2CB.NumOfDataBytes   = 64;


        I2CA.currentHandlePtr = &I2CA;
        I2CA.base             = I2CA_BASE;
        I2CA.SlaveAddr        = I2CB_ADDRESS;
        I2CA.NumOfAddrBytes   = 4;
        I2CA.pControlAddr     = (uint32_t *)0;
        I2CA.pTX_MsgBuffer    = (uint16_t *)0;
        I2CA.pRX_MsgBuffer    = (uint16_t *)0;

        //Slave pControlAddr should be 0 proper operation.
        //Slave pControlAddr is transmitted by I2CB master
        I2CA.pControlAddr   = (uint32_t *)0;
        I2CB_ControlAddr    = (uint32_t)I2CA_RXdata;
        status = I2C_MasterTransmitter(&I2CB);

        // Wait for I2CB to be complete transmission of data
        while(I2C_getStatus(I2CB.base) & I2C_STS_BUS_BUSY);

        for(i=0;i<I2CB.NumOfDataBytes;i++)
        {
            if((I2CB_TXdata[i] != I2CA_RXdata[i]) || (status != 0))
            {
                //Fail condition. Code shouldn't reach here
                //Check status (global variable) for I2C errors
                ESTOP0;
            }
        }

        //If code reached below ESTOP0, I2CB as master transmitter and
        //I2CA as slave receiver worked correctly
        //Observe the contents of I2CB_TXdata and I2CA_RXdata in memory browser
        // Example3: I2CB as Master Transmitter and I2CA working Slave Receiver - PASSED//
        ESTOP0;
}
