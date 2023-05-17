/**
 ******************************************************************************
 * @file    DualMotor.cpp
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Implementation file for DualMotor class.
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

#include "DualMotor.h"

#define MAX_SPEED       100.0f

DualMotor::DualMotor(DCMotor * left_motor, DCMotor * right_motor)
: left(left_motor), right(right_motor)
{

}

void DualMotor::car(float direction, float speed)
{
    float lMotorSpeed, rMotorSpeed;
    bool invert = (direction < 0);

    if (invert) direction = -direction; 

    saturate(direction);
    saturate(speed);

    lMotorSpeed = speed;
    rMotorSpeed = (1.0f - (direction / (MAX_SPEED/2))) * speed;

    if (invert) swap(lMotorSpeed, rMotorSpeed);

    left->setSpeed(lMotorSpeed);
    right->setSpeed(rMotorSpeed);		
}

void DualMotor::tank(float direction, float speed)
{
	float lMotorSpeed, rMotorSpeed,	lMotorExcess, rMotorExcess;

	saturate(direction);
	saturate(speed);

	lMotorSpeed = speed + direction;
	rMotorSpeed = speed - direction;

	saturate(lMotorSpeed, lMotorExcess);
	saturate(rMotorSpeed, rMotorExcess);

	lMotorSpeed -= rMotorExcess;
	rMotorSpeed -= lMotorExcess;
    
	left->setSpeed(lMotorSpeed);
	right->setSpeed(rMotorSpeed);
}

void DualMotor::tankL(float direction, float speed)
{
	float rMotorSpeed, rMotorExcess;

	saturate(direction);
	saturate(speed);

	rMotorSpeed = speed - direction;

	saturate(rMotorSpeed, rMotorExcess);

	speed -= rMotorExcess;
    
	right->setSpeed(rMotorSpeed);
	left->setSpeed(speed);
}

void DualMotor::tankR(float direction, float speed)
{
	float lMotorSpeed,	lMotorExcess;

	saturate(direction);
	saturate(speed);

	lMotorSpeed = speed + direction;

	saturate(lMotorSpeed, lMotorExcess);
	
	speed -= lMotorExcess;
    
	right->setSpeed(speed);
	left->setSpeed(lMotorSpeed);
}

void DualMotor::saturate(float & value) 
{
	if (value > MAX_SPEED) value = MAX_SPEED;
	else if (value < -MAX_SPEED) value = -MAX_SPEED;	
}

void DualMotor::saturate(float & value, float & excess)
{
	if (value > MAX_SPEED)
    {
        excess = value - MAX_SPEED;
        value = MAX_SPEED;
    } else
    if (value < -MAX_SPEED)
    { 
        excess = value + MAX_SPEED;
        value = -MAX_SPEED;
    } 
    else
    { 
        excess = 0.0f;
    }
}

void DualMotor::forward(float speed)
{
    left->setSpeed(speed);
    right->setSpeed(speed);
}

void DualMotor::backward(float speed)
{
    left->setSpeed(-speed);
    right->setSpeed(-speed);
}

void DualMotor::turnLeft(float speed, DualMotor::Pivot pivot)
{
    switch (pivot)
    {
        case DualMotor::Pivot::Center:        
        left->setSpeed(-speed);
        right->setSpeed(speed);
        break;

        case DualMotor::Pivot::Left:
        left->stop();
        right->setSpeed(speed);
        break;
        
        case DualMotor::Pivot::Right:
        left->setSpeed(-speed);
        right->stop();
        break;
    }
}

void DualMotor::turnRight(float speed, DualMotor::Pivot pivot)
{
    switch (pivot)
    {
        case DualMotor::Pivot::Center:        
        left->setSpeed(speed);
        right->setSpeed(-speed);
        break;

        case DualMotor::Pivot::Left:
        left->stop();
        right->setSpeed(-speed);
        break;
        
        case DualMotor::Pivot::Right:
        left->setSpeed(speed);
        right->stop();
        break;
    }
}

void DualMotor::stop()
{
    left->stop();
    right->stop();
}

void DualMotor::brake()
{
    left->brake();
    right->brake();
}