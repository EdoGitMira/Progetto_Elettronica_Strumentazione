/**
 ******************************************************************************
 * @file    AnalogIR.h
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Header file for class AnalogIR header of the IR sensor library.
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

#ifndef __PLT_ANALOG_IR_H__
#define __PLT_ANALOG_IR_H__

#include "mbed.h"

/** Class to use of an analog IR sensor 
 *  @extends AnalogIn
 */
class AnalogIR : private AnalogIn
{
    float min, max, is_max, threshold_min, threshold_max;

    AnalogIR() = delete;
    AnalogIR(const AnalogIR &) = delete;
    AnalogIR(AnalogIR &&) = delete;
    AnalogIR & operator=(const AnalogIR &) = delete;
    AnalogIR & operator=(AnalogIR &&) = delete;

    public:
    /** Connect IR sensor to a specific pin, with the selected parameters
     *  @param pin PinName of a PwmOut buzzer pin.
     *  @param min Floating point value that remaps the minimum value of the signal to the set value.
     *  @param max Floating point value that remaps the maximum value of the signal to the set value.
     *  @param threshold Floating point value between 0.0f and 1.0f, sets the digital trigger tolerance.
     */
    AnalogIR(const PinName & pin, const float & min = 0.0f, const float & max = 1.0f, const float & threshold = 0.1f);

    /** Sets digital trigger tolerance.
     *  @param value Floating point value between 0.0f and 1.0f.
     */
    void setThreshold(float value);

    /** Gets digital trigger tolerance.
     *  @return Floating point value between 0.0f and 1.0f.
     */
    float getThreshold();

    /** Remaps the minimum value of the signal to the set value.
     *  @param value Floating point value between 0.0f and 1.0f.
     */
    void setMin(const float & value);

    /** Remaps the maximum value of the signal to the set value.
     *  @param value Floating point value between 0.0f and 1.0f.
     */
    void setMax(const float & value);

    /** Return trigger result
     *  @return Returns a boolean value, 
     *          true if analog value is greater than high threshold, 
     *          false if the analog value is less than the low threshold
     */
    bool readDigital();

    /** Read an analog value remapped between the min and max set.
     *  @return Floating point value between min and max.
     */
    float read();

    /** Read a raw analog value from sensor
     *  @return Floating point value between 0.0f and 1.0f.
     */
    float readRaw();
};

#endif