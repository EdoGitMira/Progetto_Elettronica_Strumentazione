#include "RoadLinesReader.h"
#include <cstdint>

#define MIN_IR_THRESHOLD 0.05f
#define DEF_IR_THRESHOLD 0.10f
#define MAX_IR_THRESHOLD 0.80f

RoadLinesReader::RoadLinesReader(AnalogIR * left, AnalogIR * center, AnalogIR * right) 
: old_sign_value(0), sign_value(0), value_mask(0),
  ir_left(left), ir_center(center),
  ir_right_count(1), internal(true)
{ 
   ir_right = new AnalogIR*[ir_right_count];
   ir_right[0] = right;
}

RoadLinesReader::RoadLinesReader(AnalogIR * left, AnalogIR * center, AnalogIR * right_0, AnalogIR * right_1) 
: old_sign_value(0), sign_value(0), value_mask(0),
  ir_left(left), ir_center(center),
  ir_right_count(2), internal(true)
{ 
   ir_right = new AnalogIR*[ir_right_count];
   ir_right[0] = right_1;
   ir_right[1] = right_0;
} 

RoadLinesReader::RoadLinesReader(AnalogIR * left, AnalogIR * center, AnalogIR * right_0, AnalogIR * right_1, AnalogIR * right_2) 
: old_sign_value(0), sign_value(0), value_mask(0),
  ir_left(left), ir_center(center),
  ir_right_count(3), internal(true)
{ 
   ir_right = new AnalogIR*[ir_right_count];
   ir_right[0] = right_2;
   ir_right[1] = right_1;
   ir_right[2] = right_0;
}

RoadLinesReader::RoadLinesReader(AnalogIR * left, AnalogIR * center,  AnalogIR ** right, const uint8_t right_count) 
: old_sign_value(0), sign_value(0), value_mask(0),
  ir_left(left), ir_center(center), ir_right(right),
  ir_right_count(right_count), internal(false)
{
   
}

RoadLinesReader::~RoadLinesReader()
{
    if (internal) delete [] ir_right;
}

void RoadLinesReader::roadSignStart(uint8_t bits)
{    
    if (bits > 0)
    {
        if (bits > 8) bits = 8;
        value_mask = 1 << (bits-1);  
    }
    else value_mask = 0;  
    sign_value = 0;
    old_sign_value = isRightBlack();
}

bool RoadLinesReader::roadSignProcess()
{   
    if (value_mask)
    {
        bool cur_sign_value = isRightBlack();
        if (cur_sign_value != old_sign_value)         
        {  
            old_sign_value = cur_sign_value;
            if (cur_sign_value)
            {                   
                if (isLeftBlack()) sign_value |= value_mask;
                value_mask >>= 1; 
            }         
        }
        return true;  
    }
    return false;
}

const uint8_t & RoadLinesReader::getRoadSign() const
{
    return sign_value;
}

int RoadLinesReader::init()
{    
    if (ir_left == nullptr) return 1;
    if (ir_center == nullptr) return 2;
    if (ir_center == nullptr) return 3;
    
    uint8_t i = ir_right_count;    
    int error = 3;
    do
    {
        i--;
        error++;
        if (ir_right[i] == nullptr) return error;
    }    
    while (i>0);
    
    return 0;
}

float RoadLinesReader::readLine()
{
    float value = 0;
    uint8_t i = ir_right_count;
    
    do
    {
        i--;
        value += ir_right[i]->read();
    }    
    while (i>0);
    
    return (value * 2.0f / ir_right_count) - 1.0f;
}

void RoadLinesReader::setIRThreshold(float value)
{
    if (value<MIN_IR_THRESHOLD) value = MIN_IR_THRESHOLD;
    else if (value>MAX_IR_THRESHOLD) value = MAX_IR_THRESHOLD;

    ir_left->setThreshold(value);
    ir_center->setThreshold(value);

    uint8_t i = ir_right_count;
    
    do
    {
        i--;
        ir_right[i]->setThreshold(value);
    }    
    while (i>0);
}

bool RoadLinesReader::isLeftBlack()   
{ return ir_left->readDigital(); }

bool RoadLinesReader::isCenterBlack() 
{ return ir_center->readDigital(); }
    
bool RoadLinesReader::isLeftWhite()   
{ return !ir_left->readDigital(); }  

bool RoadLinesReader::isCenterWhite() 
{ return !ir_center->readDigital(); }

bool RoadLinesReader::isRightBlack()  
{ return ir_right[0]->readDigital(); }

bool RoadLinesReader::isRightWhite()  
{ return !ir_right[0]->readDigital(); }