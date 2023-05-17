/**
 ******************************************************************************
 * @file    DCMotor.h
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Header file for class DCMotor header of the dc motor library,
 *          use DCMotorBoard class.
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
 
#ifndef __DC_MOTOR_H__
#define __DC_MOTOR_H__

#include "mbed.h"
#include "DCMotorBoard.h"

/** DC Motor controller */
class DCMotor
{
    DCMotorBoard* boardPTR; /* DC motor board pointer */
    const uint8_t boardID;  /* motor id */
    float kp;               /* brake speed */
    bool isInverted;        /* invert direction */
    
    DCMotor() = delete;
    DCMotor(const DCMotor &) = delete;
    DCMotor(DCMotor &&) = delete;

    DCMotor & operator=(const DCMotor &) = delete;
    DCMotor & operator=(DCMotor &&) = delete;

    public:
    /** DC Motor controller
     *  @param board Pointer of DCMotorBoard extension to DC Motor board
     *  @param id Insigned integer for dc motor id
     *  @param brake_speed Floating point value between -100.0f and 100.0f used during braking
     *  @param invert Boolean value to reverse the direction of the motor
     */
    DCMotor(DCMotorBoard* board, uint8_t id, float brake_speed = 0, bool invert = false);

    /** Sets motor current speed
     *  @param value Floating point value between -100.0f and 100.0f
     *  @return Integer representing output error, 0 for no error
     */
    int setSpeed(const float & value);

    /** Gets motor current speed
     *  @param value Floating point value between -100.0f and 100.0f
     *  @return Integer representing output error, 0 for no error
     */
    int getSpeed(float & value);

    /** Sets motor speed to 0
     *  @return Integer representing output error, 0 for no error
     */
    int stop();

    /** Sets motor speed at the set braking speed
     *  @return Integer representing output error, 0 for no error
     */
    int brake();
};

#endif