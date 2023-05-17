#ifndef __PLT_DIGITAL_OUT_BASE__
#define __PLT_DIGITAL_OUT_BASE__

#include "mbed.h"

class PLTDigitalOutBase
{
    public:
    virtual int write(const bool & value) = 0;
};

class PLTDigitalInBase
{
    public:
    virtual int read(bool & value) = 0;
};

class PLTAnalogOutBase
{
    public:
    virtual int write(const float & value) = 0;
};

class PLTAnalogInBase
{
    public:
    virtual int read(float & value) = 0;
};

#endif