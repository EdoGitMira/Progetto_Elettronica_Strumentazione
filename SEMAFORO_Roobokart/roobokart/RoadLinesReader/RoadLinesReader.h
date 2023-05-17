#ifndef __PLT_ROAD_LINES_READER_H__
#define __PLT_ROAD_LINES_READER_H__

#include "mbed.h"
#include "AnalogIR.h"
#include <cstdint>

class RoadLinesReader
{ 
    bool old_sign_value;
    bool internal;
    uint8_t value_mask;
    uint8_t sign_value;
    const uint8_t ir_right_count;

    public:
    AnalogIR * ir_left;
    AnalogIR * ir_center;
    AnalogIR ** ir_right; 

    RoadLinesReader() = delete;
    RoadLinesReader(AnalogIR * left, AnalogIR * center, AnalogIR * right);
    RoadLinesReader(AnalogIR * left, AnalogIR * center, AnalogIR * right_0, AnalogIR * right_1);
    RoadLinesReader(AnalogIR * left, AnalogIR * center, AnalogIR * right_0, AnalogIR * right_1, AnalogIR * right_2);
    RoadLinesReader(AnalogIR * left, AnalogIR * center, AnalogIR ** right, const uint8_t right_count);
    
    
    RoadLinesReader(const RoadLinesReader &) = delete;
    RoadLinesReader(RoadLinesReader &&) = delete;

    virtual ~RoadLinesReader();

    RoadLinesReader & operator=(const RoadLinesReader &) = delete;
    RoadLinesReader & operator=(RoadLinesReader &&) = delete;

    int init();
    
    float readLine();

    void roadSignStart(uint8_t bits = 2);
    bool roadSignProcess();

    void setIRThreshold(float value);

    const uint8_t & getRoadSign() const;

    bool isLeftBlack();
    bool isRightBlack();
    bool isCenterBlack();

    bool isLeftWhite();  
    bool isRightWhite();
    bool isCenterWhite();
    
};

#endif 