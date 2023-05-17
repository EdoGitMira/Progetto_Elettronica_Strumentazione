#ifndef __PLT_TRAFFIC_LIGHTS_READER_H__
#define __PLT_TRAFFIC_LIGHTS_READER_H__

#include <mbed.h>
#include "ColorSensor.h"


class TrafficLightsReader
{
    ColorSensor * color;

    uint64_t clear;
    uint64_t minlight;
    float red;
    float green;
    float blue;
    float threshold;

    public:
    enum class Result { RED, GREEN, YELLOW, BLACK };
    
    TrafficLightsReader() = delete;
    TrafficLightsReader(ColorSensor * color, const float & threshold, const uint32_t & minlight);

    int init()
    {
        if (color == nullptr) return 1;
        return color->init();
    }

    Result read();

    inline const uint64_t & getClear() { return clear; }
    inline const uint64_t & getMinLight() { return clear; }
    inline void setMinLight(const uint64_t & value) { clear = value; }

    inline const float & getRedPercent() const { return red; }
    inline const float & getGreenPercent() const { return green; }
    inline const float & getBluePercent() const { return blue; }    
    inline const float & getThreshold() const { return threshold; }
    inline void setThreshold(const float & value) { threshold = value; }

    private:
    int readColors();
    float safeDiv(uint32_t val1, uint64_t val2);    
};

#endif