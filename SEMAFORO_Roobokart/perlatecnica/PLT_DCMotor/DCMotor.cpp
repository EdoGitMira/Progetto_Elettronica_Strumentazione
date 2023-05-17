/**
 ******************************************************************************
 * @file    DCMotor.cpp
 * @author  Francesco Caiazzo
 * @version V1.0.0
 * @date    20-January-2023
 * @link    https://github.com/Perlatecnica
 * @brief   Implementation file for DCMotor class.
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
 
 #include "DCMotor.h"

DCMotor::DCMotor(DCMotorBoard* board, uint8_t id, float brake_speed, bool invert)
: boardPTR(board), boardID(id), kp(brake_speed), isInverted(invert)
{
    
}

int DCMotor::setSpeed(const float & value)
{
    return boardPTR->setSpeed(isInverted ? -value : value, boardID);
}

int DCMotor::getSpeed(float & value)
{
    float out;
    int error = boardPTR->getSpeed(out, boardID);
    value = isInverted ? -out : out;
    return error;
}

int DCMotor::stop()
{
    return boardPTR->setSpeed(0, boardID);
}

int DCMotor::brake()
{    
    return boardPTR->setSpeed(isInverted ? -kp : kp, boardID);
}