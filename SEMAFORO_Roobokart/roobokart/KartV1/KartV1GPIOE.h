#ifndef __KART_V1_GPIOE_H__
#define __KART_V1_GPIOE_H__

#include "mbed.h"
#include "SafeI2C.h"
#include "PLTGPIO.h"
#include <cstdint>

/**  STMPE1600 registr map **/
#define STMPE1600_CHIP_ID_0_7  (uint8_t)0x00
#define STMPE1600_CHIP_ID_8_15 (uint8_t)0x01
#define STMPE1600_VERSION_ID   (uint8_t)0x02
#define STMPE1600_SYS_CTRL     (uint8_t)0x03
#define STMPE1600_IEGPIOR_0_7  (uint8_t)0x08
#define STMPE1600_IEGPIOR_8_15 (uint8_t)0x09
#define STMPE1600_ISGPIOR_0_7  (uint8_t)0x0A
#define STMPE1600_ISGPIOR_8_15 (uint8_t)0x0B
#define STMPE1600_GPMR_0_7	   (uint8_t)0x10
#define STMPE1600_GPMR_8_15	   (uint8_t)0x11
#define STMPE1600_GPSR_0_7	   (uint8_t)0x12
#define STMPE1600_GPSR_8_15	   (uint8_t)0x13
#define STMPE1600_GPDR_0_7	   (uint8_t)0x14
#define STMPE1600_GPDR_8_15	   (uint8_t)0x15
#define STMPE1600_GPIR_0_7	   (uint8_t)0x16
#define STMPE1600_GPIR_8_15	   (uint8_t)0x17

/**  STMPE1600 values **/
#define STMPE1600_SOFT_RESET   (uint8_t)0x80

/**  MAX7311 registr map **/
#define MAX7311_IP1ADDR	(uint8_t) 0x00	// input port 1					xxxx xxxx
#define MAX7311_IP2ADDR	(uint8_t) 0x01	// input port 2					xxxx xxxx
#define MAX7311_OP1ADDR	(uint8_t) 0x02	// output port 1				1111 1111
#define MAX7311_OP2ADDR	(uint8_t) 0x03	// output port 2				1111 1111
#define MAX7311_P1PADDR	(uint8_t) 0x04	// port 1 polarity inversion	0000 0000
#define MAX7311_P2PADDR	(uint8_t) 0x05	// port 2 polarity inversion	0000 0000
#define MAX7311_P1CADRR	(uint8_t) 0x06	// port 1 configuration			1111 1111
#define MAX7311_P2CADRR	(uint8_t) 0x07	// port 2 configuration			1111 1111
#define MAX7311_TORADRR	(uint8_t) 0x08	// timeout register				0000 0000


class KartV1GPIOE : SafeI2C
{   
    bool is_STMPE1600;
    bool is_MAX7311;
    Mutex mutex;

    int write16buf(uint8_t reg, uint16_t mask, bool value)
    {
        uint8_t data[2];
        int error;

        mutex.lock();

        error = SafeI2C::readMulti8(reg, data, 2);
        if (error) return error;

        if (value)
        {
            data[0] |= mask;
            data[1] |= mask >> 8;
        }
        else
        {
            data[0] &= ~mask;
            data[1] &= ~(mask >> 8);
        }

        error = SafeI2C::writeMulti8(reg, data, 2);

        mutex.unlock();

        return error;
    }

    int write16MaskValues(uint8_t reg, uint16_t mask, uint16_t  values)
    {
        uint8_t data[2];
        int error;

        mutex.lock();

        error = SafeI2C::readMulti8(reg, data, 2);
        if (error) return error;

        data[0] &= ~mask;
        data[1] &= ~(mask >> 8);

        data[0] |= values;
        data[1] |= values >> 8;        

        error = SafeI2C::writeMulti8(reg, data, 2);

        mutex.unlock();

        return error;
    }
    
    public:

    typedef enum : uint16_t 
    {
        GPIO_NULL = 0x0000,
           GPIO_0 = 0x0001,
           GPIO_1 = 0x0002,
           GPIO_2 = 0x0004,
           GPIO_3 = 0x0008,
           GPIO_4 = 0x0010,
           GPIO_5 = 0x0020,
           GPIO_6 = 0x0040,
           GPIO_7 = 0x0080,
           GPIO_8 = 0x0100,
           GPIO_9 = 0x0200,
          GPIO_10 = 0x0400,
          GPIO_11 = 0x0800,
          GPIO_12 = 0x1000,
          GPIO_13 = 0x2000,
          GPIO_14 = 0x4000,
          GPIO_15 = 0x8000,
         GPIO_ALL = 0xFFFF
    } GPIOName;

    typedef enum : bool
    {
        INPUT  = 0,
        OUTPUT = 1
    } GPIOMode;

    KartV1GPIOE(PinName sda, PinName scl, uint8_t address, const int & frequency = 1000000)
    : SafeI2C(sda, scl, address, frequency)
    {
        uint8_t data[2];
        is_STMPE1600 = SafeI2C::readMulti8(STMPE1600_CHIP_ID_0_7, data, 2) == 0;
        if (!is_STMPE1600)                
        {
            SafeI2C::addressI2C = address - 68;
            is_MAX7311 = SafeI2C::readMulti8(0x00, data, 2) == 0;
        }
    }

    bool isSTMPE1600() {
        return is_STMPE1600;
    }

    bool isMAX7311() {
        return is_MAX7311;
    }

    /*
    int reset() {
        int error;
        uint8_t data[2];
        if (is_STMPE1600)
        {      
            data[0] = STMPE1600_SOFT_RESET;
            error = dev_i2c.writeReg(dev_addr, STMPE1600_SYS_CTRL, data, 1);            
            return error;
        }
        else if (is_MAX7311)
        {
            data[0] = 0x00;
            data[1] = 0x00;

            error = dev_i2c.writeReg(dev_addr, MAX7311_P1PADDR, data, 2);
            if (error) return error;

            data[0] = 0x00;

            error = dev_i2c.writeReg(dev_addr, MAX7311_TORADRR, data, 1);
            if (error) return error;
            
            data[0] = 0xFF;
            data[1] = 0xFF;

            error = dev_i2c.writeReg(dev_addr, MAX7311_OP1ADDR, data, 2);
            if (error) return error;

            error = dev_i2c.writeReg(dev_addr, MAX7311_P1CADRR, data, 2);
            return error;
        }

        write(0);
        return 1;
    }*/

    int setMode(GPIOName pin, GPIOMode mode) {
        if (is_STMPE1600) 
        {            
            return write16buf(STMPE1600_GPDR_0_7, pin, mode);
        }
        else return write16buf(MAX7311_P1CADRR, pin, !mode);
    }

    int write(GPIOName pin, bool value) {        
        if (is_STMPE1600) return write16buf(STMPE1600_GPSR_0_7, pin, value);
        else return write16buf(MAX7311_OP1ADDR, pin, value);
    }

    int writeMask(uint16_t mask, uint16_t values) {        
        if (is_STMPE1600) return write16MaskValues(STMPE1600_GPSR_0_7, mask, values);
        else return write16MaskValues(MAX7311_OP1ADDR, mask, values);
    }

    private:
    /*
    int write(uint16_t value)
    {   
        uint8_t data[2];
        data[0] = ~value;
        data[1] = ~(value >> 8);
        if (is_STMPE1600) return SafeI2C::writeMulti8(STMPE1600_GPSR_0_7, data, 2);        
        else return SafeI2C::writeMulti8(MAX7311_OP1ADDR, data, 2);      
    } 
    */  
};

class DigitalOutKartV1GPIOE : public PLTDigitalOutBase//, public PLTDigitalInBase
{
    const KartV1GPIOE::GPIOName mask;
    KartV1GPIOE * sr;

    public:
    
    DigitalOutKartV1GPIOE() = delete;
    DigitalOutKartV1GPIOE(KartV1GPIOE * shiftRegisterKartV1GPIOE,
                          const KartV1GPIOE::GPIOName &gpio,
                          const bool & value = false);

    int write(const bool & value) override;
   // int read(bool & value) override;
};

#endif