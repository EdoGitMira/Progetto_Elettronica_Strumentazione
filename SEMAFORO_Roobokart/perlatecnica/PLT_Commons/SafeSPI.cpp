#include "SafeSPI.h"

SafeSPI::SafeSPI(PinName mosi, PinName miso, PinName sclk,
                 int bits, bool clock_polarity, bool clock_phase, int frequency)
: SPI(mosi, miso, sclk)
{ 
    int mode = 0;
    if (clock_polarity) mode |= 0b10;
    if (clock_phase)    mode |= 0b01;    
    SPI::format(bits, mode);
    SPI::frequency(frequency);
}

int SafeSPI::writeSingle(const uint16_t & data)
{
    int err;
    SPI::lock();   

    err = otherSelect();
    if (err) return err;    
    
    SPI::write(data);    
    err = otherUnelect();
    
    SPI::unlock();
    return err;
}

int SafeSPI::readSingle(uint16_t & data)
{
    int err;
    SPI::lock();
    
    err = otherSelect();
    if (err) return err;
    
    data = SPI::write(0);
    err = otherUnelect();
    
    SPI::unlock();
    return err;
}

int SafeSPI::readAndWriteSingle(const uint16_t & datain, uint16_t & dataout)
{
    int err;
    SPI::lock();
    
    err = otherSelect();
    if (err) return err;
    
    dataout = SPI::write(datain);
    err = otherUnelect();
    
    SPI::unlock();
    return err;
}