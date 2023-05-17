#include "TrafficLightsReader.h"

TrafficLightsReader::TrafficLightsReader(ColorSensor * color, const float & threshold, const uint32_t & minlight)
: color(color), threshold(threshold), minlight(minlight)
{ }


TrafficLightsReader::Result TrafficLightsReader::read()
{
    readColors(); 

    if (abs(red - green) > threshold)
    {
        if ((red > green) && (red > blue)) return Result::RED;        
        else if ((green > blue) && (green > red)) return Result::GREEN;            
    } 
    else 
    {
        if ((red - blue) > threshold) return Result::YELLOW;
    }
    return Result::BLACK;
}

int TrafficLightsReader::readColors()
{
    ColorSensor::Result result;
    int error;

    error = color->read(ColorSensor::Filter::RGB, result);
    if (error) return error;

    clear = result.red;
    clear += result.blue;
    clear += result.green;
    
    if (clear > minlight)
    {
        green = safeDiv(result.green, clear);
        blue = safeDiv(result.blue, clear);
        red = safeDiv(result.red, clear);
    }   
    else
    {
        green = 0;
        blue = 0;
        red = 0;
    }

    return error;
}

float TrafficLightsReader::safeDiv(uint32_t val1, uint64_t val2)
{
    if (val2 == 0) return 0.0f;
    return (float) val1 / val2;
}