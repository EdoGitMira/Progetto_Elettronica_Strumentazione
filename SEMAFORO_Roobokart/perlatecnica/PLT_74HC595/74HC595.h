#ifndef __PLT_SHIFT_REGISTER_74HC595_H__
#define __PLT_SHIFT_REGISTER_74HC595_H__

#include "mbed.h"
#include "PLTGPIO.h"
#include "SafeSPI.h"

class _74HC595 : protected SafeSPI
{
    uint8_t data;
    Mutex mutex;
    DigitalOut cs;

    public:  

    enum class GPIO : uint8_t {
        GPIO_0 = 0b00000001,
        GPIO_1 = 0b00000010,
        GPIO_2 = 0b00000100,
        GPIO_3 = 0b00001000,
        GPIO_4 = 0b00010000,
        GPIO_5 = 0b00100000,
        GPIO_6 = 0b01000000,
        GPIO_7 = 0b10000000
    };

    _74HC595() = delete;
    _74HC595(PinName mosi, PinName miso, PinName sck, PinName cs, const uint8_t & value = 0, int frequency = 42000000);

    void setAll(const uint8_t & value);
    void setSingle(const bool & value, const GPIO & gpio);
    void getSingle(bool & value, const GPIO & gpio);

    private:

    int otherSelect() override
    { 
        cs.write(0);
        return 0;
    }
    
    int otherUnelect() override
    {
        cs.write(1);
        return 0;
    }
};

class DigitalOut74HC595 : public PLTDigitalOutBase, public PLTDigitalInBase
{
    const _74HC595::GPIO mask;
    _74HC595 * sr;

    public:
    
    DigitalOut74HC595() = delete;
    DigitalOut74HC595(_74HC595 * shiftRegister74HC595, const _74HC595::GPIO &gpio);

    int write(const bool & value) override;
    int read(bool & value) override;
};

#endif