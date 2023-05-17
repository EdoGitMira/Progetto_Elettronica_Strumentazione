#ifndef __PLT_TCS3200_H__
#define __PLT_TCS3200_H__

#include <mbed.h>
#include "ColorSensor.h"

class TCS3200 : public ColorSensor
{
    public:
    enum class Mode
    {
        POWERDOWN = 0,
        SCALE_2 = 50,
        SCALE_20 = 5,
        SCALE_100 = 1,
    };  

    private:
    Mode mode;
    DigitalOut mS0;
    DigitalOut mS1;
    DigitalOut mS2;
    DigitalOut mS3;
    InterruptIn signal;
    Mutex mutex;
    Semaphore semaphore;
    Timeout timeout; 
    uint32_t count;
    uint32_t exposuretime = 30000;

    public:  
    TCS3200() = delete;
    TCS3200(const PinName & s0,const PinName & s1,const PinName & s2,const PinName & s3,const PinName & out);
    
    TCS3200(const TCS3200 &) = delete;
    TCS3200(TCS3200 &&) = delete;

    TCS3200& operator=(const TCS3200 &) = delete;
    TCS3200& operator=(TCS3200 &&) = delete;

    inline bool readRawRed(uint32_t & out) { return readRawColor(out, 0, 0); }
    inline bool readRawGreen(uint32_t & out) { return readRawColor(out, 1, 1); }
    inline bool readRawBlue(uint32_t & out) { return readRawColor(out, 0, 1); }
    inline bool readRawClear(uint32_t & out) { return readRawColor(out, 1, 0); }

    bool readRed(uint32_t & out);
    bool readGreen(uint32_t & out);
    bool readBlue(uint32_t & out);
    bool readClear(uint32_t & out);

    void SetMode(const Mode & mode);
    const Mode & GetMode();

    void setExposureTime(const uint32_t & microsec);
    const uint32_t & getExposureTime();

    int init() override;
    int read(const Filter & filter, Result & out, bool raw = false) override;

    private:
    void highTrigger();
    void startRead();
    void stopRead();    
    void writeMode(const Mode & mode);
    uint32_t adaptValue(const uint32_t & value);
    bool readRawColor(uint32_t & out, const bool & s2, const bool & s3);    
};
 
#endif