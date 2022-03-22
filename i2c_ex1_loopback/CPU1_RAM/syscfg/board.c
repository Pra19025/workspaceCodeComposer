/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "board.h"

void Board_init()
{
	EALLOW;

	PinMux_init();
	I2C_init();

	EDIS;
}

void PinMux_init()
{
	//
	// I2CA -> myI2C0 Pinmux
	//
	GPIO_setPinConfig(GPIO_0_I2CA_SDA);
	GPIO_setPadConfig(0, GPIO_PIN_TYPE_PULLUP);
	GPIO_setQualificationMode(0, GPIO_QUAL_ASYNC);
	GPIO_setPinConfig(GPIO_1_I2CA_SCL);
	GPIO_setPadConfig(1, GPIO_PIN_TYPE_PULLUP);
	GPIO_setQualificationMode(1, GPIO_QUAL_ASYNC);

}

void I2C_init(){
	//myI2C0 initialization
	
	I2C_disableModule(myI2C0_BASE);
	I2C_initMaster(myI2C0_BASE, DEVICE_SYSCLK_FREQ, 400000, I2C_DUTYCYCLE_50);
	I2C_setConfig(myI2C0_BASE, I2C_MASTER_SEND_MODE);
	I2C_setSlaveAddress(myI2C0_BASE, 60);
	I2C_enableLoopback(myI2C0_BASE);
	I2C_setOwnSlaveAddress(myI2C0_BASE, 0);
	I2C_setBitCount(myI2C0_BASE, I2C_BITCOUNT_8);
	I2C_setDataCount(myI2C0_BASE, 2);
	I2C_setAddressMode(myI2C0_BASE, I2C_ADDR_MODE_7BITS);
	I2C_enableFIFO(myI2C0_BASE);
	I2C_clearInterruptStatus(myI2C0_BASE, I2C_INT_RXFF | I2C_INT_TXFF);
	I2C_setFIFOInterruptLevel(myI2C0_BASE, I2C_FIFO_TX2, I2C_FIFO_RX2);
	I2C_enableInterrupt(myI2C0_BASE, I2C_INT_RXFF | I2C_INT_TXFF);
	I2C_setEmulationMode(myI2C0_BASE, I2C_EMULATION_STOP_SCL_LOW);
	I2C_enableModule(myI2C0_BASE);

}
