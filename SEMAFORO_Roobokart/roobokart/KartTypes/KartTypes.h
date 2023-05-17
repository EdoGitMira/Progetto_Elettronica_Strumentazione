#ifndef __KART_MODES_H__
#define __KART_MODES_H__

#include "mbed.h"


enum class KartMode
{
    Startup,
    Navigation,
    Recovery,
    RoadSign,
    Obstacles,
    TrafficLight,
    Crossing,
    Error
};

static const char * toString(const KartMode & mode)
{       
    switch (mode)
    {
        case KartMode::Startup: return "Startup";
        case KartMode::Navigation: return "Navigation";
        case KartMode::Recovery: return "Recovery";
        case KartMode::RoadSign: return "RoadSign";
        case KartMode::Obstacles: return "Obstacles";
        case KartMode::TrafficLight: return "TrafficLight";
        case KartMode::Crossing: return "Crossing";
        case KartMode::Error: return "Error";
        default: return "UKNOW_MODE";            
    }
}  

#endif