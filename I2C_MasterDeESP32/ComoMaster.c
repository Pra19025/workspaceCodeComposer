// Según el pinout debe usarse I2CB
#include "driverlib.h"
#include "device.h"


#include "i2cLib_FIFO_master_slave_interrupt.h"
#define I2CB_ADDRESS   0x40


//
// Globals
//
uint16_t status = 0;

struct I2CHandle I2CA;
struct I2CHandle I2CB;

//
// Function Prototypes
//

__interrupt void i2cBISR(void);
__interrupt void i2cBFIFOISR(void);

uint16_t AvailableI2C_slaves[MAX_I2C_IN_NETWORK];

uint16_t I2CB_TXdata[MAX_BUFFER_SIZE];
uint16_t I2CB_RXdata[MAX_BUFFER_SIZE];

uint32_t I2CB_ControlAddr;
uint16_t status;

void I2C_GPIO_init(void);
void I2Cinit(void);

//
// Main
//
void main(void)
{

    Device_init();
    Device_initGPIO();
    I2C_GPIO_init();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    I2Cinit();

    I2C_setOwnSlaveAddress(I2CB_BASE, I2CB_ADDRESS);


    uint16_t i;

    for(i=0;i<MAX_I2C_IN_NETWORK;i++)
    {
        AvailableI2C_slaves[i] = 0;
    }

    I2C_disableInterrupt(I2CB_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));

    uint16_t *pAvailableI2C_slaves = AvailableI2C_slaves;

    I2C_clearStatus(I2CB_BASE,I2C_STS_NO_ACK|I2C_STS_ARB_LOST|I2C_STS_REG_ACCESS_RDY|I2C_STS_STOP_CONDITION);

    ESTOP0;
    I2C_disableInterrupt(I2CB_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));

    status = I2CBusScan(I2CB_BASE, pAvailableI2C_slaves);
    I2C_clearStatus(I2CB_BASE,I2C_STS_NO_ACK|I2C_STS_ARB_LOST|I2C_STS_REG_ACCESS_RDY|I2C_STS_STOP_CONDITION);


    I2C_enableInterrupt(I2CB_BASE, (I2C_INT_ADDR_SLAVE|I2C_INT_STOP_CONDITION | I2C_INT_ARB_LOST | I2C_INT_NO_ACK));
    EINT;
    ERTM;


    for(i=0;i<MAX_BUFFER_SIZE;i++)
    {
        //vaciando los buffers
        I2CB_TXdata[i] = 0;
        I2CB_RXdata[i] = 0;
    }


    while(1)
    {

    }

}

void I2C_GPIO_init(void)
{
    // I2CB pins (SDAB / SCLB)
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SDAB, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SDAB, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SDAB, GPIO_QUAL_ASYNC);

    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCLB, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCLB, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCLB, GPIO_QUAL_ASYNC);

    GPIO_setPinConfig(DEVICE_GPIO_CFG_SDAB);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCLB);
}

void I2Cinit(void)
{

    //I2CB initialization
    I2C_disableModule(I2CB_BASE);
    I2C_initMaster(I2CB_BASE, DEVICE_SYSCLK_FREQ, 100000, I2C_DUTYCYCLE_50);
    I2C_setConfig(I2CB_BASE, I2C_MASTER_SEND_MODE);
    I2C_setSlaveAddress(I2CB_BASE, 80);
    I2C_disableLoopback(I2CB_BASE);
    I2C_setBitCount(I2CB_BASE, I2C_BITCOUNT_8);
    I2C_setDataCount(I2CB_BASE, 2);
    I2C_setAddressMode(I2CB_BASE, I2C_ADDR_MODE_7BITS);
    I2C_enableFIFO(I2CB_BASE);
    I2C_clearInterruptStatus(I2CB_BASE, I2C_INT_ADDR_SLAVE | I2C_INT_ARB_LOST | I2C_INT_NO_ACK | I2C_INT_STOP_CONDITION);
    I2C_setFIFOInterruptLevel(I2CB_BASE, I2C_FIFO_TXEMPTY, I2C_FIFO_RX16);
    I2C_enableInterrupt(I2CB_BASE, I2C_INT_ADDR_SLAVE | I2C_INT_ARB_LOST | I2C_INT_NO_ACK | I2C_INT_STOP_CONDITION);
    I2C_setEmulationMode(I2CB_BASE, I2C_EMULATION_FREE_RUN);
    I2C_enableModule(I2CB_BASE);

}


