/**
 ******************************************************************************
 * @file    STSpin240.cpp
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Implementation file for STSpin240 class.
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
 
 #include "STSpin240.h"

#define MAX_SPEED       100.0f
#define MAX_PERCENT     100.0f 
#define MIN_PERCENT     0.0f
#define DEF_LIMIT       100.0f  
#define DEF_FREQUENCY   100
#define DEMO_FREQUENCY  20000  
#define MAX_FREQUENCY   100000  
#define MIN_FREQUENCY   2

#define ADAPT_RANGE(ad_value, ad_min, ad_max) \
if (ad_value>ad_max) ad_value = ad_max; \
else if (ad_value<ad_min) ad_value = ad_min;

STSpin240::STSpin240(PinName flag_and_enable_pin, PinName standby_reset_pin, PinName dirA_pin, PinName dirB_pin, PinName pwmA_pin, PinName pwmB_pin, PinName pwmRef_pin) :
DCMotorBoard(2), flag_and_enable(flag_and_enable_pin),
standby_reset(standby_reset_pin),
dirA(dirA_pin), dirB(dirB_pin),
pwmA(pwmA_pin), pwmB(pwmB_pin), pwmRef(pwmRef_pin)
{
    enable();    
    setFrequency(DEF_FREQUENCY);
    setCurrentLimit(DEF_LIMIT);
    setSpeed(0);  
    flag_and_enable.mode(PullUp);
}

void STSpin240::setFrequency(uint32_t hz)
{
    mutex.lock();

    ADAPT_RANGE(hz, MIN_FREQUENCY, MAX_FREQUENCY);
    float value = 1.0f / hz;    
    pwmA.period(value);
    pwmB.period(value);
    pwmRef.period(value);
    
    mutex.unlock();
}

bool STSpin240::adaptSpeed(float & value)
{
    if (value<0.0f)
    {
        if (value < -MAX_SPEED) value = MAX_SPEED; 
        else value = -value;
        return false;
    } 
    else if (value > MAX_SPEED) value = MAX_SPEED;
    return true;
}

void STSpin240::setCurrentLimit(float percentage)
{
    ADAPT_RANGE(percentage, MIN_PERCENT, MAX_PERCENT);
    pwmRef.write(percentage/100);
}

int STSpin240::setSpeed(const float & value)
{    
    mutex.lock();
    
    float speed = value;
    speedA = value;
    speedB = value;
    
    dirA = adaptSpeed(speed);
    dirB = dirA;

    speed /= MAX_SPEED;

    pwmA.write(speed);
    pwmB.write(speed);
    
    mutex.unlock();
    return 0;
}

int STSpin240::setSpeed(const float & value, const uint8_t & index)
{
    mutex.lock();
    float speed = value;
    
    switch (index)
    {
        case 0:
        speedA = value;
        dirA = adaptSpeed(speed);
        pwmA.write(speed/MAX_SPEED);
        break;
        
        case 1:
        speedB = value;
        dirB = adaptSpeed(speed);
        pwmB.write(speed/MAX_SPEED);
        break;

        default:
        return 1;
    }

    mutex.unlock();
    return 0;
}

int STSpin240::getSpeed(float & value, const uint8_t & index)
{
    mutex.lock();
    
    switch (index)
    {
        case 0:
        value = speedA;
        break;
        
        case 1:
        value = speedB;
        break;

        default:
        return 1;
    }

    mutex.unlock();
    return 0;
}