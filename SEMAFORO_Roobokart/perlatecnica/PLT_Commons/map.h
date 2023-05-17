/**
 ******************************************************************************
 * @file    map.h
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Header file for map methods.
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

#ifndef __PLT_MAP_H__
#define __PLT_MAP_H__

#include "mbed.h"

/** Remaps a number from one range to another. 
 *  @param in_data Value to remap. 
 *  @param in_min Lowest input value. 
 *  @param in_max Highest input value. 
 *  @param out_min Lowest output value. 
 *  @param out_max Highest output value. 
 *  @return Remapped value.
 */
template <typename T>
static T map(const T & in_data, const T & in_min, const T & in_max, const T & out_min, const T & out_max)
{
    return (in_data - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/** Remaps a number from one range to another. 
 *  @param in_data Value to remap. 
 *  @param in_min Lowest input value. 
 *  @param in_max Highest input value. 
 *  @param out_min Lowest output value. 
 *  @param out_max Highest output value. 
 *  @param out_data Remapped value.
 */
template <typename T>
static void map(const T & in_data, const T & in_min, const T & in_max, const T & out_min, const T & out_max, float & out_data)
{
    out_data = map(in_data, in_min, in_max, out_min, out_max);
}

/** Remaps a number from one range to another and truncates values in ranges. 
 *  @param in_data Value to remap. 
 *  @param in_min Lowest input value. 
 *  @param in_max Highest input value. 
 *  @param out_min Lowest output value. 
 *  @param out_max Highest output value. 
 *  @return Remapped value.
 */
template <typename T>
static T mapAndLimit(const T & in_data, const T & in_min, const T & in_max, const T & out_min, const T & out_max)
{
    const T out_data = map(in_data, in_min, in_max, out_min, out_max);
    if (out_data > out_max) return out_max;
    else if (out_data < out_min) return out_min;
    return out_data;
}

template <typename T>
/** Remaps a number from one range to another and truncates values in ranges.
 *  @param in_data Value to remap. 
 *  @param in_min Lowest input value. 
 *  @param in_max Highest input value. 
 *  @param out_min Lowest output value. 
 *  @param out_max Highest output value. 
 *  @param out_data Remapped value.
 */
static void mapAndLimit(const T & in_data, const T & in_min, const T & in_max, const T & out_min, const T & out_max, T & out_data)
{
    return mapAndLimit(in_data, in_min, in_max, out_min, out_max, out_data);
}

#endif