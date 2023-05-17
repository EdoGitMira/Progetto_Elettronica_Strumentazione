/**
 ******************************************************************************
 * @file    AnalogIR.cpp
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Implementation file for AnalogIn class.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2023 perlatecnica.it, MIT License</center></h2>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 */

#include "AnalogIR.h"
#include "map.h"

AnalogIR::AnalogIR(const PinName & pin, const float & min, const float & max, const float & threshold)
: AnalogIn(pin), min(min), max(max), is_max(0)
{  
    setThreshold(threshold);
    is_max = AnalogIn::read() >= threshold_max;
}

void AnalogIR::setThreshold(float value)
{
    AnalogIn::lock();
    if (value < 0) value = 0;
    if (value > 1) value = 1;
    threshold_min = 0.5f - (value/2);
    threshold_max = 0.5f + (value/2);
    AnalogIn::unlock();
}

float AnalogIR::getThreshold()
{
    float result;
    AnalogIn::lock();
    result = threshold_max - threshold_min;
    AnalogIn::unlock();
    return result;
}

void AnalogIR::setMin(const float & value)
{
    AnalogIn::lock();
    min = value;
    AnalogIn::unlock();
}

void AnalogIR::setMax(const float & value)
{
    AnalogIn::lock();
    max = value;
    AnalogIn::unlock();
}

float AnalogIR::read()
{
    float result;
    AnalogIn::lock();
    result = mapAndLimit<float>(AnalogIn::read(), min, max, 0.0f, 1.0f);
    AnalogIn::unlock();
    return result;
}

float AnalogIR::readRaw()
{    
    return AnalogIn::read();
}

bool AnalogIR::readDigital()
{
    AnalogIn::lock();  
    if (is_max)
    {
        if (read() < threshold_min)
        {
            is_max = false;
            AnalogIn::unlock();
            return false;
        }
        AnalogIn::unlock();
        return true;
    } 
    else
    {
        if (read() > threshold_max)
        {
            is_max = true;
            AnalogIn::unlock();
            return true;
        }
        AnalogIn::unlock();
        return false;
    }
}