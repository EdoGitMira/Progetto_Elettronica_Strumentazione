/**
 ******************************************************************************
 * @file    DualMotor.h
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Header file for class DualMotor header of two motors controller library.
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

#ifndef PLT_DC_MOTORS_H
#define PLT_DC_MOTORS_H

#include "DCMotor.h"

/** Class for controlling a vehicle with two electric motors for steering */
class DualMotor
{
    DCMotor * left;     /** Ponter to left motor */
    DCMotor * right;    /** Ponter to right motor */

    DualMotor() = delete;
    DualMotor(const DualMotor &) = delete;
    DualMotor(DualMotor &&) = delete;

    DualMotor & operator=(const DualMotor &) = delete;
    DualMotor & operator=(DualMotor &&) = delete;
    
    public:
    /** Enumeration of pivot location options */
    enum class Pivot
    {
        Center, /**< Pivot on center */
        Left,   /**< Pivot on left */
        Right   /**< Pivot on right */
    };

    /** Builder of the DualMotor class
     *  @param left_motor DCMotor Pointer to left motor
     *  @param right_motor DCMotor Pointer to right motor
     */
    DualMotor(DCMotor * left_motor, /**< pointer motor */ 
              DCMotor * right_motor);           

    /** Steering algorithm that simulates the behavior of a car
     *  @param direction Floating point value between -100.0f and 100.0f, manages vehicle steering
     *  @param speed Floating point value between -100.0f and 100.0f, manages vehicle speed
     *  @note Speed values between -50.0f and 50.0f faithfully simulate the behavior of a car, it will not spin on itself.
     *        Speed values between -100 and -50 or between 50 and 100 simulate behavior similar to the tank(float, float) method.
     *        In case of motor saturation, real speed could be lower than set speed.
     */
    void car(float direction, float speed);

    /** Steering algorithm that simulates the behavior of a tank with a pivot in the center
     *  @param direction Floating point value between -100.0f and 100.0f, manages vehicle steering
     *  @param speed Floating point value between -100.0f and 100.0f, manages vehicle speed
     *  @note In case of motor saturation, real speed could be lower than set speed.
     *        The vehicle will turn on itself even with speed set to zero.
     */
    void tank(float direction, float speed);

    /** Steering algorithm that simulates the behavior of a tank with rotation pivot on the left
     *  @param direction Floating point value between -100.0f and 100.0f, manages vehicle steering
     *  @param speed Floating point value between -100.0f and 100.0f, manages vehicle speed
     *  @note In case of motor saturation, real speed could be lower than set speed.
     *        The vehicle will turn on itself even with speed set to zero.
     */
    void tankL(float direction, float speed);
    
    /** Steering algorithm that simulates the behavior of a tank with rotation pivot on the right
     *  @param direction Floating point value between -100.0f and 100.0f, manages vehicle steering
     *  @param speed Floating point value between -100.0f and 100.0f, manages vehicle speed
     *  @note In case of motor saturation, real speed could be lower than set speed.
     *        The vehicle will turn on itself even with speed set to zero.
     */
    void tankR(float direction, float speed);

    /** Drives the vehicle forward.
     *  @param speed Floating point value between -100.0f and 100.0f, manages vehicle speed.
     *  @note The vehicle may not go straight.
    */
    void forward(float speed);

    /** Drives the vehicle backward. 
     *  @param speed Floating point value between -100.0f and 100.0f, manages vehicle speed.
     *  @note The vehicle may not go straight.
    */
    void backward(float speed);

    /** Turn left the vehicle on itself using set rotation pivot.
     *  @param speed Floating point value between -100.0f and 100.0f, manages vehicle speed.
     *  @param pivot Pivot enumerator to set rotation pivot.
    */
    void turnLeft(float speed, Pivot pivot = Pivot::Center);

    /** Turn right the vehicle on itself using set rotation pivot.
     *  @param speed Floating point value between -100.0f and 100.0f, manages vehicle speed.
     *  @param pivot Pivot enumerator to set rotation pivot.
    */
    void turnRight(float speed, Pivot pivot = Pivot::Center);

    /** Stop the vehicle */
    void stop();

    /** Stop the vehicle using brakes
     *  @note The effect of this method can change depending on the type of motor, brake and settings implemented.
     */
    void brake();

    private:
    void saturate(float & value);
    void saturate(float & value, float & excess);
};

#endif