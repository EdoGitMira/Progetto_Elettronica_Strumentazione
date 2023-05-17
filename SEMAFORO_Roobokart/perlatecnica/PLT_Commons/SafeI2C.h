#ifndef __PLT_DEVICE_I2C_H__
#define __PLT_DEVICE_I2C_H__

#include "mbed.h"

class SafeI2C : protected I2C
{ 
    protected:
    uint8_t addressI2C;

    public:
    SafeI2C() = delete;
    SafeI2C(PinName sda, PinName scl, const uint8_t & address, const int & frequency = 1000000);
    
    SafeI2C(const SafeI2C &) = delete;
    SafeI2C(SafeI2C &&) = delete;

    SafeI2C & operator=(const SafeI2C &) = delete;
    SafeI2C & operator=(SafeI2C &&) = delete;   
    
    int writeMulti8(const uint8_t & index, const uint8_t * data, const int & length);
    int readMulti8(const uint8_t & index, uint8_t * data, const int & length);
    int writeMulti16(const uint16_t & index, const uint8_t * data, const int & length);
    int readMulti16(const uint16_t & index, uint8_t * data, const int & length);
    
    inline int writeByte8(const uint8_t & index, const uint8_t & data);
    int writeWord8(const uint8_t & index, const uint16_t & data);
    int writeDoubleWord8(const uint8_t & index, const uint32_t & data);
    //int writeQuadWord8
    
    inline int readByte8(const uint8_t & index, uint8_t & data);
    int readWord8(const uint8_t & index, uint16_t & data);
    int readDoubleWord8(const uint8_t & index, uint32_t & data);
    //int readQuadWord8
    
    inline int writeByte16(const uint16_t & index, const uint8_t & data);   
    int writeWord16(const uint16_t & index, const uint16_t & data);   
    int writeDoubleWord16(const uint16_t & index, const uint32_t & data);    
    //int writeQuadWord16
    
    inline int readByte16(const uint16_t & index, uint8_t & pdata);    
    int readWord16(const uint16_t & index, uint16_t & pdata);    
    int readDoubleWord16(const uint16_t & index, uint32_t & pdata);
    //int readQuadWord16

    protected:

    
};

inline int SafeI2C::writeByte8(const uint8_t & index, const uint8_t & data)
{
    return writeMulti8(index, &data, 1);
}

inline int SafeI2C::readByte8(const uint8_t & index, uint8_t & data)
{
    return readMulti8(index, &data, 1);
}   

inline int SafeI2C::writeByte16(const uint16_t & index, const uint8_t & data)
{
    return writeMulti16(index, &data, 1);
}  

inline int SafeI2C::readByte16(const uint16_t & index, uint8_t & data)
{
    return readMulti16(index, &data, 1);
} 

#endif