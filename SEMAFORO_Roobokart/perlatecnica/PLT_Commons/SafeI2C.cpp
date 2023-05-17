#include "SafeI2C.h"

SafeI2C::SafeI2C(PinName sda, PinName scl, const uint8_t & address, const int & frequency)
: I2C(sda, scl), addressI2C(address)
{ 
    I2C::frequency(frequency);
}

/* I2C DEVICE MULTI 8 */

int SafeI2C::writeMulti8(const uint8_t & index, const uint8_t * data, const int & length)
{
    int res;
    
    I2C::lock();
    
    res = I2C::write(addressI2C, (char*)&index, 1, true);
    if (!res) res = I2C::write(addressI2C, (char*)data, length);
    
    I2C::unlock();

    return res;
}

int SafeI2C::readMulti8(const uint8_t & index, uint8_t * data, const int & length)
{
    int res;

    I2C::lock(); 

    res = I2C::write(addressI2C, (char*)&index, 1, true);
    if (!res) res = I2C::read(addressI2C, (char*)data, length);

    I2C::unlock();

    return res;
}

/* I2C DEVICE MULTI 16 */

int SafeI2C::writeMulti16(const uint16_t &  index, const uint8_t * data, const int & length)
{
    int res;
    char temp[2];

    temp[0] = (index >> 8) & 0xFF;
    temp[1] = index & 0xFF;
    
    I2C::lock();

    res = I2C::write(addressI2C, temp, 2, true);
    if (!res) res = I2C::write(addressI2C, (char*)data, length);

    I2C::unlock();

    return res;
}

int SafeI2C::readMulti16(const uint16_t &  index, uint8_t * data, const int & length)
{
    int res;
    char temp[2];

    temp[0] = (index >> 8) & 0xFF;
    temp[1] = index & 0xFF;

    I2C::lock();

    res = I2C::write(addressI2C, temp, 2, true);
    if (!res) res = I2C::read(addressI2C, (char*)data, length);

    I2C::unlock();

    return res;
}

/* I2C DEVICE 8 */

int SafeI2C::writeWord8(const uint8_t & index, const uint16_t & data)
{
    uint8_t buffer[2];
    buffer[0] = (data >> 8) & 0xFF;
    buffer[1] = data & 0xFF;
    return writeMulti8(index, buffer, 2);
} 

int SafeI2C::writeDoubleWord8(const uint8_t & index, const uint32_t & data)
{
    uint8_t buffer[4];
    buffer[0] = (data >> 24) & 0xFF;
    buffer[1] = (data >> 16) & 0xFF;
    buffer[2] = (data >> 8) & 0xFF;
    buffer[3] = data & 0xFF;
    return writeMulti8(index, buffer, 4);
}



int SafeI2C::readWord8(const uint8_t & index, uint16_t & data)
{
    uint8_t buffer[2];
    int result = readMulti8(index, buffer, 2);
    if (result) return result;
    data = (uint16_t)(buffer[0] << 8) | buffer[1];        
    return result;
}

int SafeI2C::readDoubleWord8(const uint8_t & index, uint32_t & data)
{
    uint8_t buffer[4];
    int result = readMulti8(index, buffer, 4);	
    if(result) return result;    
    data = (uint32_t)(buffer[0] << 24) | (uint32_t)(buffer[1] << 16) | (uint32_t)(buffer[2] << 8) | buffer[3];
    return result;
}

/* I2C DEVICE 16 */

int SafeI2C::writeWord16(const uint16_t & index, const uint16_t & data)
{
    uint8_t buffer[2];
    buffer[0] = (data >> 8) & 0xFF;
    buffer[1] = data & 0xFF;
    return writeMulti16(index, buffer, 2);
} 

int SafeI2C::writeDoubleWord16(const uint16_t & index, const uint32_t & data)
{
    uint8_t buffer[4];
    buffer[0] = (data >> 24) & 0xFF;
    buffer[1] = (data >> 16) & 0xFF;
    buffer[2] = (data >> 8) & 0xFF;
    buffer[3] = data & 0xFF;
    return writeMulti8(index, buffer, 4);
}

int SafeI2C::readWord16(const uint16_t & index, uint16_t & data)
{
    uint8_t buffer[2];
    int result = readMulti16(index, buffer, 2);
    if (result) return result;
    data = (uint16_t)(buffer[0] << 8) | buffer[1];        
    return result;
}  

int SafeI2C::readDoubleWord16(const uint16_t & index, uint32_t & data)
{
    uint8_t buffer[4];    
    int result = readMulti16(index, buffer, 4);	
    if(result) return result;    
    data = (uint32_t)(buffer[0] << 24) | (uint32_t)(buffer[1] << 16) | (uint32_t)(buffer[2] << 8) | buffer[3];
    return result;
}