/**
 ******************************************************************************
 * @file    PID.h
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Header file for class PID header of pid controller library.
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

#ifndef __PLT_PID_H__
#define __PLT_PID_H__

/** Class implementing the PID controller 
 *  @brief PID Controller
 */
template <typename T>
class PID
{
    const T _Kp;
    const T _Ki;
    const T _Kd;
    T _old_error;
    T _integral;

    PID() = delete;
    PID(const PID &) = delete;
    PID(PID &&) = delete;    

    PID & operator=(const PID &) = delete;
    PID & operator=(PID &&) = delete;

    public:
    /** Create and initialize the PID controller.
     *  @param Kp Proportional gain
     *  @param Ki Integral gain
     *  @param Kd Derivative gain
     */
    PID(const T & Kp, const T & Ki, const T & Kd);

    /** Processes the raw value.
     *  @param dt Loop interval time.
     *  @param setpoint Lalue to chase.
     *  @param currentvalue Sensor raw value.
     *  @return Returns manipulated value.
     */
    T evaluate(const T & dt, const T & setpoint, const T & currentvalue);

    /** Processes the raw value.
     *  @param dt Loop interval time.
     *  @param setpoint Value to chase.
     *  @param currentvalue Sensor raw value.
     *  @param min Minimum value of the output range.
     *  @param max Maximum value of the output range.
     *  @return Returns manipulated and truncated value.
     */
    T evaluate(const T & dt, const T & setpoint, const T & currentvalue,
               const T & min, const T & max);
               
    /** Processes the raw value.
     *  @param dt Loop interval time.
     *  @param setpoint Value to chase.
     *  @param currentvalue Sensor raw value.
     *  @param min Minimum value of the output range.
     *  @param max Maximum value of the output range.
     *  @param outrange Return a boolean value, true if the return value has been truncated.
     *  @return Returns manipulated and truncated value.
     */
    T evaluate(const T & dt, const T & setpoint, const T & currentvalue,
               const T & min, const T & max, bool & outrange);
    
    /** Reset PID controller*/
    void reset();
};


template <typename T>
PID<T>::PID(const T & Kp, const T & Ki, const T & Kd)
: _Kp(Kp), _Kd(Kd), _Ki(Ki)
{ 
    reset();
}

template <typename T>
void PID<T>::reset()
{
	_old_error = 0;
	_integral = 0;
}

template <typename T>
T PID<T>::evaluate(const T & dt, const T & setpoint, const T & currentvalue)
{
	// error
    T error = currentvalue - setpoint;

    // PID controller Proportional term
    T P = _Kp * error;

    // PID controller Discrete Integral term
    _integral += error * dt;
    T I = _Ki * _integral;

    // PID controller Derivative term
    T derivative = (error - _old_error) / dt;
    T D = _Kd * derivative;

    // Controller Output
    T C = P + I + D;

    // Store the current error value as the previous one for next calling
    _old_error = error;

    return C;
}

template <typename T>
T PID<T>::evaluate(const T & dt, const T & setpoint, const T & currentvalue,
                   const T & min, const T & max)
{
    T C = evaluate(dt, setpoint, currentvalue);

    // Saturation to interval values
    if ( C > max ) C = max;
    else if ( C < min ) C = min;

    return C;
}

template <typename T>
T PID<T>::evaluate(const T & dt, const T & setpoint, const T & currentvalue, 
                   const T & min, const T & max, bool & outrange)
{
    T C = evaluate(dt, setpoint, currentvalue);

    // Saturation to interval values
    if ( C > max )
    {        
        outrange = true;
        return max;
    }
    else if ( C < min )
    {        
        outrange = true;
        return min;
    }
    
    outrange = false;
    return C;
}

#endif