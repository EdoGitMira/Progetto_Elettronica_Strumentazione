#ifndef __PLT_COLOR_SENSOR_H__
#define __PLT_COLOR_SENSOR_H__

#include "mbed.h"

class ColorSensor
{
    public:
    enum Filter : uint8_t
    {
        Null = 0x00,
        Clear = 0x01,
        Red = 0x02,
        Green = 0x04,
        Blue  = 0x08,
        IR = 0x10,
        UV = 0x20,
        Visible = 0x40,
        RGB = 0x0E
    };

    struct Result
    {
        uint32_t clear;
        uint32_t red;
        uint32_t green;
        uint32_t blue;
        uint32_t ir;
        uint32_t uv;
        uint32_t visible ;       
    };

    virtual int init() = 0;
    virtual int read(const Filter & filter, Result & out, bool raw = false) = 0;

    protected:
    void resetResult(Result & out)
    {
        out.clear = 0;
        out.red = 0;
        out.green = 0;
        out.blue = 0;
        out.ir = 0;
        out.uv = 0;
        out.visible = 0;
    }
};


#endif