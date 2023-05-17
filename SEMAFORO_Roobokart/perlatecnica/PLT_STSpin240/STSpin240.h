/**
 ******************************************************************************
 * @file    STSpin240.h
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Header file for class STSpin240 header of the dc motor board library,
 *          extend DCMotorBoard class.
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

#ifndef __PLT_STSPIN240_H__
#define __PLT_STSPIN240_H__

#include "mbed.h"
#include "DCMotorBoard.h"

/** DC Motor board controller STSpin240
* @extends DCMotorBoard
*/
class STSpin240 : public DCMotorBoard
{    
    InterruptIn flag_and_enable;    /* Flag Interrupt */      
    Mutex mutex;                    /* Mutex for thread safety */
    float speedA;                   /* Motor A speddd */
    float speedB;                   /* Motor B speddd */

    DigitalOut standby_reset;       /* Standby/reset pin */    
    DigitalOut dirA;                /* Direction pin of bridge A */   
    DigitalOut dirB;                /* Direction pin of bridge B */    

    PwmOut pwmA;                    /* Pulse Width Modulation pin for bridge A input */
    PwmOut pwmB;                    /* Pulse Width Modulation pin for bridge B input */
    PwmOut pwmRef;                  /* Pulse Width Modulation pin for Ref signal */
    

    STSpin240() = delete;
    STSpin240(const STSpin240 &) = delete;
    STSpin240(STSpin240 &&) = delete;

    STSpin240 & operator=(const STSpin240 &) = delete;
    STSpin240 & operator=(STSpin240 &&) = delete;

    public:
    using DCMotorBoard::motorCount; /** Maximum number of motors */
    
    /** Connect STSpin240 board
     *  @param flag_and_enable_pin InterruptIn PinName for signaling overheating or shutdown
     *  @param standby_reset_pin DigitalOut PinName for enabling the motor board
     *  @param dirA_pin DiditalOut PinName for motor A direction control
     *  @param dirB_pin DiditalOut PinName for motor A direction control
     *  @param pwmA_pin PwmOut PinName for motor A speed control
     *  @param pwmB_pin PwmOut PinName for motor B speed control
     *  @param pwmRef_pin PwmOut PinName for current limit control of all motors
     */   
    STSpin240(PinName flag_and_enable_pin, PinName standby_reset_pin, PinName dirA_pin,
              PinName dirB_pin, PinName pwmA_pin, PinName pwmB_pin, PinName pwmRef_pin);
    

    /** Sets speed and direction for all motors
     *  @param value Floating point value betwin -100.0f and 100.0f
     *  @returns Integer representing output error, 0 for no error
     */
    int setSpeed(const float & value) override;

    /** Sets the speed and direction of a specific motor
     *  @param value Floating point value betwin -100.0f and 100.0f
     *  @param index Unsigned integer value that identifies selected engine
     *  @returns Integer representing output error, 0 for no error
     */
    int setSpeed(const float & value, const uint8_t & index) override;

    /** Gets the speed and direction of a specific motor
     *  @param value Floating point value betwin -100.0f and 100.0f
     *  @param index Unsigned integer value that identifies selected engine
     *  @returns Integer representing output error, 0 for no error
     */
    int getSpeed(float & value, const uint8_t & index) override;
    
    
    /** Set current limit for all motors
     *  @param percentage Floating point value betwin 0.0f and 100.0f
     */
    void setCurrentLimit(float percentage);

    /** Set current limit for all motors
     *  @param frequency Unsigned integer value for frequency in Hz
     */
    void setFrequency(uint32_t frequency);

    /** Disable all motors */
    inline void disable() { standby_reset = 0; }

    /** Enable all motors */
    inline void enable() { standby_reset = 1; }
    
    /** Check if the engine is actually enabled */
    inline bool isEnabled() { return standby_reset; }

    /** Set method called when board shuts down */
    inline void onFlag(Callback<void(void)> method) { flag_and_enable.fall(method); }

    /** Set method called when board turns on */
    inline void onUnflag(Callback<void(void)> method) { flag_and_enable.rise(method); }
    
    private:
    bool adaptSpeed(float & value);
};

#endif