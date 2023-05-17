#ifndef __PLT_SAFE_SPI_H__
#define __PLT_SAFE_SPI_H__

#include "mbed.h"

class SafeSPI : protected SPI
{
    public:
    /** Configure the data transmission format.
     *  @param bits Number of bits per SPI frame (4 - 16).
     *  @param mode Clock polarity and phase mode (0 - 3).
     */
    SafeSPI(PinName mosi, PinName miso, PinName sclk, int bits = 8, bool clock_polarity = false, bool clock_phase = false, int frequency = 42000000);

    int writeSingle(const uint16_t & data);
    int readSingle(uint16_t & data);
    int readAndWriteSingle(const uint16_t & datain, uint16_t & dataout);

    protected:
    virtual int otherSelect() { return 0; }
    virtual int otherUnelect() { return 0; }
};


#endif