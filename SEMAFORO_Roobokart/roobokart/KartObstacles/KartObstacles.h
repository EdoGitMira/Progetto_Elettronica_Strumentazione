#ifndef __PLT_KART_OBSTACLES_H__
#define __PLT_KART_OBSTACLES_H__

#include "mbed.h"
#include "RangeSensor.h"
#include "LockGuard.h"
#include "map.h"

class KartObstacles
{    
    RangeSensor * rs_left;
    RangeSensor * rs_center;
    RangeSensor * rs_right;    
    uint32_t measure_min;
    Mutex mutex;

    Thread thread;
    public:

    uint32_t brake_mm;
    uint32_t safety_mm;

    KartObstacles(RangeSensor * left, RangeSensor * center, RangeSensor * right)
    : rs_left(left), rs_center(center), rs_right(right),
      brake_mm(80), safety_mm(150), measure_min(0xFFFFFFFF)
    {

    }

    int init()
    {
        thread.start(callback(this,&KartObstacles::kart_obs_thread));
        return 0;
    }

    const uint32_t & getMin()
    {
        LockGuard guard(mutex);
        return measure_min;
    }

    float safety()
    {
        return mapAndLimit<float>(getMin(), brake_mm, safety_mm, 0, 1);
    }

    bool detected()
    {
        return getMin() <= brake_mm;
    }

    private:

    void read_value(RangeSensor * rs, uint32_t & out)
    {
        uint32_t temp;
        if ((rs!= nullptr) && (rs->read(temp) == 0) && (temp < out))
        {
            out = temp;
        }
    }

    void kart_obs_thread()
    {
        uint32_t temp;
        while (true)
        {
            temp = 0xFFFFFFFF;
            read_value(rs_left, temp);            
            read_value(rs_center, temp);            
            read_value(rs_right, temp);

            mutex.lock();
            measure_min = temp;
            mutex.unlock();
        }
    }
};


#endif