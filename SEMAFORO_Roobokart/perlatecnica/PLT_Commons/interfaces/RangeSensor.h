#ifndef __PLT_RANGE_SENSOR_H__
#define __PLT_RANGE_SENSOR_H__

#include "mbed.h"


class RangeSensor
{
    public:
    virtual int read(uint32_t & distance) = 0;
};


#endif