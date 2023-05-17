
/* 
* This file is part of VL53L1 Platform 
* 
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved 
* 
* License terms: BSD 3-clause "New" or "Revised" License. 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this 
* list of conditions and the following disclaimer. 
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution. 
* 
* 3. Neither the name of the copyright holder nor the names of its contributors 
* may be used to endorse or promote products derived from this software 
* without specific prior written permission. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
* 
*/

#include "VL53L1X.h"

VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_WriteMulti(uint16_t index, uint8_t *pdata, uint32_t count) {
	if (SafeI2C::writeMulti16(index, pdata, count)) return Exception::ERROR_I2C_FAIL;
    return Exception::NO_ERROR;
}

VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_ReadMulti(uint16_t index, uint8_t *pdata, uint32_t count){
	if (SafeI2C::readMulti16(index, pdata, count)) return Exception::ERROR_I2C_FAIL;
    return Exception::NO_ERROR;
}

VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_WrByte(uint16_t index, uint8_t data) {
	if (SafeI2C::writeMulti16(index, &data, 1)) return Exception::ERROR_I2C_FAIL;
    return Exception::NO_ERROR;
}

VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_WrWord(uint16_t index, uint16_t data) {
	uint8_t buffer[2];
    buffer[0] = data >> 8;
    buffer[1] = data & 0x00FF;
    if (SafeI2C::writeMulti16(index, buffer, 2)) return Exception::ERROR_I2C_FAIL;
    return Exception::NO_ERROR;
}

VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_WrDWord(uint16_t index, uint32_t data) {
	uint8_t buffer[4];
    buffer[0] = (data >> 24) & 0xFF;
    buffer[1] = (data >> 16) & 0xFF;
    buffer[2] = (data >>  8) & 0xFF;
    buffer[3] = (data >>  0) & 0xFF;
    if (SafeI2C::writeMulti16(index, buffer, 4)) return Exception::ERROR_I2C_FAIL;
    return Exception::NO_ERROR;
}

VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_RdByte(uint16_t index, uint8_t *data) {
	if (SafeI2C::readMulti16(index, data, 1)) return Exception::ERROR_I2C_FAIL;
    return Exception::NO_ERROR;
}

VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_RdWord(uint16_t index, uint16_t *data) {
    uint8_t buffer[2];
	int8_t result = SafeI2C::readMulti16(index, buffer, 2);
    if (result) return Exception::ERROR_I2C_FAIL;    
    *data = (uint16_t)(buffer[0] << 8) | buffer[1];
    return Exception::NO_ERROR;
}

VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_RdDWord(uint16_t index, uint32_t *data) {
    uint8_t buffer[4];
	int8_t result = SafeI2C::readMulti16(index, buffer, 4);	
    if(result) return Exception::ERROR_I2C_FAIL;    
    *data = (uint16_t)(buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    return Exception::NO_ERROR;
}
/*
VL53L1X::VL53L1X_ERROR VL53L1X::VL53L1_WaitMs(int32_t wait_ms){
	ThisThread::sleep_for(wait_ms);
    return Exception::NO_ERROR;
}*/
