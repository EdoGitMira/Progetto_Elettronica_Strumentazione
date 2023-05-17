/**
 ******************************************************************************
 * @file    DCMotorBoard.h
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2022
 * @link    https://github.com/Perlatecnica
 * @brief   Header file for class DCMotorBoard interface for dc motor board library.
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

#ifndef __PLT_DC_MOTOR_BOARD_H__
#define __PLT_DC_MOTOR_BOARD_H__

#include "mbed.h"

class DCMotorBoard
{
    DCMotorBoard() = delete;
    DCMotorBoard(const DCMotorBoard &) = delete;

    public:
    /** Maximum number of motors */
    const uint8_t motorCount;
 
    DCMotorBoard(const uint8_t & motor_count) : motorCount(motor_count) {}

    virtual int setSpeed(const float & value) = 0;
    virtual int setSpeed(const float & value, const uint8_t & index) = 0;
    virtual int getSpeed(float & value, const uint8_t & index) = 0;
};

#endif