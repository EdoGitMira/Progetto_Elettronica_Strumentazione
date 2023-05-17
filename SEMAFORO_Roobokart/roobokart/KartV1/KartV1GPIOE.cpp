#include "KartV1GPIOE.h"

DigitalOutKartV1GPIOE::DigitalOutKartV1GPIOE(KartV1GPIOE * shiftRegisterKartV1GPIOE,
                                             const KartV1GPIOE::GPIOName & gpio,
                                             const bool & value)
: sr(shiftRegisterKartV1GPIOE), mask(gpio)
{
    if (sr)
    {
        sr->setMode(mask, KartV1GPIOE::GPIOMode::OUTPUT);
        sr->write(mask, value);
    }
}

int DigitalOutKartV1GPIOE::write(const bool & value)
{      
    if (sr) 
    {
        sr->write(mask, value);
        return 0;
    }
    return 1;
}

/*int DigitalOutKartV1GPIOE::read(bool & value)
{
    if (sr)
    {
        //sr->getSingle(value, mask);
        return 0;
    }
    return 1;
}*/