#include "TCS3200.h"
#include "LockGuard.h"

#define TCS3200_DELAY   10000
#define TCS3200_TIMEOUT 1000

TCS3200::TCS3200(const PinName & s0,const PinName & s1,const PinName & s2,const PinName & s3,const PinName & out)
: mS0(s0), mS1(s1), mS2(s2), mS3(s3), signal(out) 
{
    writeMode(Mode::POWERDOWN);
    mode = Mode::SCALE_20;
    signal.rise(callback(this,&TCS3200::highTrigger)); 
}

int TCS3200::init()
{
    return 0;
}

bool TCS3200::readRed(uint32_t & out) 
{ 
    uint32_t temp;
    bool result;

    result = readRawRed(temp);
    if (result) return result;
    
    out = adaptValue(temp);
    return result;
}

bool TCS3200::readGreen(uint32_t & out)
{ 
    uint32_t temp;
    bool result;

    result = readRawGreen(temp);
    if (result) return result;
    
    out = adaptValue(temp);
    return result;
}

bool TCS3200::readBlue(uint32_t & out)
{ 
    uint32_t temp;
    bool result;

    result = readRawBlue(temp);
    if (result) return result;
    
    out = adaptValue(temp);
    return result;
}

bool TCS3200::readClear(uint32_t & out)
{ 
    uint32_t temp;
    bool result;

    result = readRawClear(temp);
    if (result) return result;
    
    out = adaptValue(temp);
    return result;
}

void TCS3200::SetMode(const Mode & mode)
{
    mutex.lock();
    this->mode = mode;
    mutex.unlock();  
}

const TCS3200::Mode & TCS3200::GetMode() 
{ 
    LockGuard guard(mutex);
    return mode;
}

void TCS3200::setExposureTime(const uint32_t & microsec) 
{    
    mutex.lock();
    exposuretime = microsec;
    mutex.unlock(); 
}

const uint32_t & TCS3200::getExposureTime() 
{
    LockGuard guard(mutex);
    return exposuretime;
}

void TCS3200::highTrigger()
{ 
    count++;
}

void TCS3200::startRead()
{    
    timeout.attach_us(callback(this, &TCS3200::stopRead), exposuretime);
    semaphore.release();
}

void TCS3200::stopRead()
{
    semaphore.release();
}

void TCS3200::writeMode(const Mode & mode)
{     
    switch (mode)
    {
        case Mode::SCALE_2:
        mS0 = 0;                     
        mS1 = 1;
        break;

        case Mode::SCALE_20:
        mS0 = 1;                     
        mS1 = 0;
        break;

        case Mode::SCALE_100:
        mS0 = 1;                     
        mS1 = 1;
        break;

        default:
        mS0 = 0;                     
        mS1 = 0;
        break;
    }    
}

uint32_t TCS3200::adaptValue(const uint32_t & value) 
{ 
    return (value * 1000000 * (uint32_t) mode) / exposuretime ;
}

bool TCS3200::readRawColor(uint32_t & out, const bool & s2, const bool & s3)
{
    LockGuard guard(mutex);
    bool result; 
    
    writeMode(mode);
    
    mS2=s2;                    
    mS3=s3;
    count = 0;

    timeout.attach_us(callback(this, &TCS3200::startRead), TCS3200_DELAY);
    if (semaphore.try_acquire_for(TCS3200_DELAY + TCS3200_TIMEOUT))
    {
        result = !semaphore.try_acquire_for(exposuretime + TCS3200_TIMEOUT);        
    }
    else
    {
        result = true;
    }     
    
    writeMode(Mode::POWERDOWN);

    out = count;

    return result;
}

int TCS3200::read(const Filter & filter, Result & out, bool raw)
{
    int error = 0;

    resetResult(out);

    if (filter & Filter::Red)
    {
        if (raw) error = readRawRed(out.red);
        else error = readRed(out.red);
    }
    if (error) return error;

    if (filter & Filter::Green)
    {
        if (raw) error = readRawGreen(out.green);
        else error = readGreen(out.green);
    }
    if (error) return error;

    if (filter & Filter::Blue)
    {
        if (raw) error = readRawBlue(out.blue);
        else error = readBlue(out.blue);
    }
    if (error) return error;

    if (filter & Filter::Clear)
    {
        if (raw) error = readRawClear(out.clear);
        else error = readClear(out.clear);
    }
    if (error) return error;

    if (filter & Filter::Visible)
    {
        if (raw) error = readRawClear(out.visible);
        else error = readClear(out.visible);
    }
    if (error) return error;

    if (filter & (Filter::UV | Filter::IR)) return 1;

    return error;
}