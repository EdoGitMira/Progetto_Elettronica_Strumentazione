#include "74HC595.h"

_74HC595::_74HC595(PinName mosi, PinName miso, PinName sck, PinName cs, const uint8_t & value, int frequency)
: SafeSPI(mosi, miso, sck, 8, true, true, frequency), cs(cs), data(value)
{
    SafeSPI::writeSingle(value);
}

void _74HC595::setAll(const uint8_t & value)
{    
    mutex.lock();
    data = value;
    SafeSPI::writeSingle(data);
    mutex.unlock();     
}

void _74HC595::setSingle(const bool & value, const GPIO & gpio)
{
    mutex.lock();        
    if (value) data |= (uint8_t) gpio;
    else data &= ~(uint8_t)gpio;
    SafeSPI::writeSingle(data);
    mutex.unlock();   
}

void _74HC595::getSingle(bool &value, const GPIO & gpio)
{
    mutex.lock();        
    value = (bool)(data | (uint8_t)gpio); 
    mutex.unlock();
}

/**************************************************************************/

DigitalOut74HC595::DigitalOut74HC595(_74HC595 * shiftRegister74HC595, const _74HC595::GPIO &gpio)
: sr(shiftRegister74HC595), mask(gpio)
{
    
}

int DigitalOut74HC595::write(const bool & value)
{      
    if (sr) 
    {
        sr->setSingle(value, mask);
        return 0;
    }
    return 1;
}

int DigitalOut74HC595::read(bool & value)
{
    if (sr)
    {
        sr->getSingle(value, mask);
        return 0;
    }
    return 1;
}