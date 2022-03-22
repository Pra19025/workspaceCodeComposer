// Según el pinout debe usarse I2CB

#include "driverlib.h"

// Included Files
//
#include "driverlib.h"
#include "device.h"

// I2C message states for I2CMsg struct
#define MSG_STATUS_INACTIVE         0x0000 // Message not in use, do not send
#define MSG_STATUS_SEND_WITHSTOP    0x0010 // Send message with stop bit
#define MSG_STATUS_WRITE_BUSY       0x0011 // Message sent, wait for stop
#define MSG_STATUS_SEND_NOSTOP      0x0020 // Send message without stop bit
#define MSG_STATUS_SEND_NOSTOP_BUSY 0x0021 // Message sent, wait for ARDY
#define MSG_STATUS_RESTART          0x0022 // Ready to become master-receiver
#define MSG_STATUS_READ_BUSY        0x0023 // Wait for stop before reading data

//
// Defines
//
#define SLAVE_ADDRESS               0x04
#define NUM_BYTES                   8
#define MAX_BUFFER_SIZE             14      // Max is currently 14 because of
                                            // 2 address bytes and the 16-byte
                                            // FIFO

//
// Error messages for read and write functions
//
#define ERROR_BUS_BUSY              0x1000
#define ERROR_STOP_NOT_READY        0x5555
#define SUCCESS                     0x0000

//
// Typedefs
//
struct I2CMsg {
    uint16_t msgStatus;                  // Word stating what state msg is in.
                                         // See MSG_STATUS_* defines above.
    uint16_t slaveAddr;                  // Slave address tied to the message.
    uint16_t numBytes;                   // Number of valid bytes in message.
    uint16_t msgBuffer[MAX_BUFFER_SIZE]; // Array holding message data.
};

//
// Globals
//
struct I2CMsg i2cMsgOut = {MSG_STATUS_SEND_WITHSTOP,
                           SLAVE_ADDRESS,
                           NUM_BYTES,
                           0x01         //message bytes
                           };
struct I2CMsg i2cMsgIn  = {MSG_STATUS_SEND_NOSTOP,
                           SLAVE_ADDRESS,
                           NUM_BYTES,
                           };

struct I2CMsg *currentMsgPtr;                   // Used in interrupt

uint16_t passCount = 0;
uint16_t failCount = 0;

//
// Function Prototypes
//
void initI2C(void);
uint16_t readData(struct I2CMsg *msg);
uint16_t writeData(struct I2CMsg *msg);

void fail(void);
void pass(void);


//
// Main
//
void main(void)
{
    uint16_t i;


    Device_init();
    Device_initGPIO();

    GPIO_setPinConfig(DEVICE_GPIO_CFG_SDAB);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SDAB, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SDAB, GPIO_QUAL_ASYNC); //que no sea síncrono

    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCLB);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCLB, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCLB, GPIO_QUAL_ASYNC);


    // para el blink de prueba
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);


    Interrupt_initModule();
    Interrupt_initVectorTable();

    initI2C();

    //
    // Clear incoming message buffer
    //
    for (i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        i2cMsgIn.msgBuffer[i] = 0x0000;
    }


    while(1)
    {

            // blink
            // Turn on LED
            GPIO_writePin(DEVICE_GPIO_PIN_LED1, 0);

            // Delay for a bit.
            DEVICE_DELAY_US(100000);

            // Turn off LED
            GPIO_writePin(DEVICE_GPIO_PIN_LED1, 1);

            // Delay for a bit.
            DEVICE_DELAY_US(100000);



    }
}

//
// Function to configure I2C A in FIFO mode.
//
void initI2C()
{
    //
    // Must put I2C into reset before configuring it
    //
    I2C_disableModule(I2CB_BASE);

    //
    // I2C configuration. Use a 100kHz I2CCLK with a 33% duty cycle.
    //
    I2C_initMaster(I2CB_BASE, DEVICE_SYSCLK_FREQ, 100000, I2C_DUTYCYCLE_33);
    I2C_setBitCount(I2CB_BASE, I2C_BITCOUNT_8);
    I2C_setSlaveAddress(I2CB_BASE, SLAVE_ADDRESS);
    I2C_setEmulationMode(I2CB_BASE, I2C_EMULATION_FREE_RUN);

    //
    // Enable stop condition and register-access-ready interrupts
    //
    I2C_enableInterrupt(I2CB_BASE, I2C_INT_STOP_CONDITION |
                                   I2C_INT_REG_ACCESS_RDY);

    //
    // FIFO configuration
    //
    I2C_enableFIFO(I2CB_BASE);
    I2C_clearInterruptStatus(I2CB_BASE, I2C_INT_RXFF | I2C_INT_TXFF);

    //
    // Configuration complete. Enable the module.
    //
    I2C_enableModule(I2CB_BASE);
}

//
// Function to send the data that is to be written to the EEPROM
//
uint16_t writeData(struct I2CMsg *msg)
{
    uint16_t i;

    //
    // Wait until the STP bit is cleared from any previous master
    // communication. Clearing of this bit by the module is delayed until after
    // the SCD bit is set. If this bit is not checked prior to initiating a new
    // message, the I2C could get confused.
    //
    if(I2C_getStopConditionStatus(I2CB_BASE))
    {
        return(ERROR_STOP_NOT_READY);
    }

    //
    // Setup slave address
    //
    I2C_setSlaveAddress(I2CB_BASE, SLAVE_ADDRESS);

    //
    // Check if bus busy
    //
    if(I2C_isBusBusy(I2CB_BASE))
    {
        return(ERROR_BUS_BUSY);
    }

    //
    // Setup number of bytes to send msgBuffer and address
    //
    I2C_setDataCount(I2CB_BASE, (msg->numBytes + 2));


    for (i = 0; i < msg->numBytes; i++)
    {
        I2C_putData(I2CB_BASE, msg->msgBuffer[i]);
    }

    //
    // Send start as master transmitter
    //
    I2C_setConfig(I2CB_BASE, I2C_MASTER_SEND_MODE);
    I2C_sendStartCondition(I2CB_BASE);
    I2C_sendStopCondition(I2CB_BASE);

    return(SUCCESS);
}

//
// Function to prepare for the data that is to be read from the EEPROM
//
uint16_t readData(struct I2CMsg *msg)
{
    //
    // Wait until the STP bit is cleared from any previous master
    // communication. Clearing of this bit by the module is delayed until after
    // the SCD bit is set. If this bit is not checked prior to initiating a new
    // message, the I2C could get confused.
    //
    if(I2C_getStopConditionStatus(I2CB_BASE))
    {
        return(ERROR_STOP_NOT_READY);
    }

    //
    // Setup slave address
    //
    I2C_setSlaveAddress(I2CB_BASE, SLAVE_ADDRESS);

    //
    // If we are in the the address setup phase, send the address without a
    // stop condition.
    //
    if(msg->msgStatus == MSG_STATUS_SEND_NOSTOP)
    {
        //
        // Check if bus busy
        //
        if(I2C_isBusBusy(I2CB_BASE))
        {
            return(ERROR_BUS_BUSY);
        }

        //
        // Send data to setup EEPROM address
        //
        I2C_setDataCount(I2CB_BASE, 2);
        I2C_setConfig(I2CB_BASE, I2C_MASTER_SEND_MODE);
        I2C_sendStartCondition(I2CB_BASE);
    }
    else if(msg->msgStatus == MSG_STATUS_RESTART)
    {
        //
        // Address setup phase has completed. Now setup how many bytes expected
        // and send restart as master-receiver.
        //
        I2C_setDataCount(I2CB_BASE, (msg->numBytes));
        I2C_setConfig(I2CB_BASE, I2C_MASTER_RECEIVE_MODE);
        I2C_sendStartCondition(I2CB_BASE);
        I2C_sendStopCondition(I2CB_BASE);
    }

    return(SUCCESS);
}


//
// Function to be called if data written matches data read
//
void pass(void)
{
    asm("   ESTOP0");
    for(;;);
}

//
// Function to be called if data written does NOT match data read
//
void fail(void)
{
    asm("   ESTOP0");
    for(;;);
}




