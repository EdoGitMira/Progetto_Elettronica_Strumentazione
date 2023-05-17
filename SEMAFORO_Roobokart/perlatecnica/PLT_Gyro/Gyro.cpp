/**
 ******************************************************************************
 * @file    Gyro.cpp
 * @author  Francesco Caiazzo
 * @version V2.0.0
 * @date    13-May-2022
 * @brief   Implementation file for the Gyro class
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2022 perlatecnica.it, MIT License</center></h2>
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

#include "Gyro.h"
#include <cstdio>

Gyro::Gyro(PinName sda, PinName scl, uint8_t address) : acc_gyro(sda, scl, address)
{
    
}

int Gyro::init()
{  
    int error;
    float sensitivity;

    error = acc_gyro.init();
    if(error) return error;
    error = acc_gyro.enable_x();
    if(error) return error;
    error = acc_gyro.enable_g();
    if(error) return error;
    error = acc_gyro.set_x_odr(sampleRate);
    if(error) return error;
    error = acc_gyro.set_g_odr(sampleRate);
    if(error) return error;

    error = acc_gyro.get_g_sensitivity(&sensitivity);
    if(error) return error;
    sensitivity /= 1000;
    gyroscopeSensitivity.axis.x = sensitivity;
    gyroscopeSensitivity.axis.y = sensitivity;
    gyroscopeSensitivity.axis.z = sensitivity;
    
    error = acc_gyro.get_x_sensitivity(&sensitivity);
    if(error) return error;
    sensitivity /= 1000;
    accelerometerSensitivity.axis.x = sensitivity;
    accelerometerSensitivity.axis.y = sensitivity;
    accelerometerSensitivity.axis.z = sensitivity;

    const FusionAhrsSettings settings = 
    { 
        .gain = 0.5f,
        .accelerationRejection = 10.0f,
        .magneticRejection = 20.0f, 
        .rejectionTimeout = 5
    };

    FusionOffsetInitialise(&offset, sampleRate);
    FusionAhrsInitialise(&ahrs);
    FusionAhrsSetSettings(&ahrs, &settings); 
    
    attitude_thread.start(callback(this,&Gyro::_attitude_thread));

    while (ahrs.initialising) 
    {
        ThisThread::yield();
    }
    
    return error;
}

bool Gyro::waitForYawStab(const uint32_t timeout_ms, const float threshold)
{
    float old_val = getYaw();
    float cur_val = -old_val;
    Timer timeout;
    timeout.start();   

    while (ahrs.initialising) 
    {
        ThisThread::yield();
    }
    
    do
    {
        ThisThread::sleep_for(20);
        cur_val = getYaw();
        if (abs(cur_val - old_val) < threshold) 
        {
            return false;
        }
        else
        {            
            old_val = cur_val;
        }
    }
    while (timeout.read_ms() < timeout_ms);
    
    timeout.stop();
    return true;
}

float Gyro::range(float value)
{
    if (value<=-180) return value + 360;
    if (value>180) return value - 360;
    return value;
}

void Gyro::_attitude_thread()
{ 
    int16_t axes_acc[3];
    int16_t axes_gyr[3];
    FusionVector gyroscope;
    FusionVector accelerometer;
    clock_t timestamp;
    clock_t previousTimestamp = clock();
    float deltaTime;    

    while (true)
    {
        timestamp = clock();
        
        acc_gyro.get_x_axes_raw(axes_acc);
        acc_gyro.get_g_axes_raw(axes_gyr);        
                   
        gyroscope.axis.x = axes_gyr[0];
        gyroscope.axis.y = axes_gyr[1];
        gyroscope.axis.z = axes_gyr[2];
            
        accelerometer.axis.x = axes_acc[0];
        accelerometer.axis.y = axes_acc[1];
        accelerometer.axis.z = axes_acc[2];
            
        gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
        accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
        
        gyroscope = FusionOffsetUpdate(&offset, gyroscope);
                    
        deltaTime = (float) (timestamp - previousTimestamp) / (float) CLOCKS_PER_SEC;
        previousTimestamp = timestamp;
        
        FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime); 
        
        euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs)); 
    }
}