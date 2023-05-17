#ifndef __ROOBOKART_CONSOLE_APP_TOOLS__
#define __ROOBOKART_CONSOLE_APP_TOOLS__
#include "mbed.h"
#include "roobokart_def.h"
#include "DCMotor.h"
#include "VL53L1X.h"

#define ROOBOKART_APP_DOUBLE_BAR "\r\n===============================\r\n\r\n"

namespace console_app
{
    template <class T>
    void appSetMin(T & var, const T & value)
    { 
        if (var > value) var = value;
    }
    
    template <class T>
    void appSetMax(T & var, const T & value) 
    { 
        if (var < value) var = value;
    }

    static void appTitle(const char * title)
    {
        printf("\r\n=== %s ======\r\n\r\n", title);
        printf(" software-rev: 1.0.0\r\n");
        
        #if defined(ROOBOKART_V1)
        printf("roobokart-rev: V1.x\r\n");
        #elif defined(ROOBOKART_V3)
        printf("roobokart-rev: V3.0\r\n");
        #else
        printf("roobokart-rev: NULL\r\n");
        #endif

        printf("  mbed-os-rev: %d.%d.%d\r\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
        printf(ROOBOKART_APP_DOUBLE_BAR);
    }

    static bool appWaitUserButton(const char * text = "Press USER button\r\n")
    {
        bool hold = false;
        const int time = 1000;
        Timer timer;
        DigitalIn button(BUTTON1);
        printf("%s", text);
        while (!button) ThisThread::yield();
        while (button) ThisThread::yield();
        timer.start();
        while (!button && !hold) 
        {
            hold = timer.read_ms() > time;
            ThisThread::yield();
        }
        printf(ROOBOKART_APP_DOUBLE_BAR);
        return hold;
    }

    static void appMotorRun(DCMotor & motor, const char * name, const int8_t & speed)
    {  
        printf("Press USER button to set %s motor speed at %d%%\r\n", name, speed);
        appWaitUserButton("");      
        printf("Motor %s is running at %d%%.\r\n", name, speed);
        if (speed>0) printf("It should go foreward");
        else if (speed<0) printf("It should go backward");
        else printf("It should stop");
        printf("\r\n\r\n");
        motor.setSpeed(speed);
    }

    static void appMotorStop(DCMotor & motor, const char * name)
    {  
        printf("Press USER botton to stop %s motor\r\n", name);
        appWaitUserButton("");
        printf("Motor %s is running at %d%%.\r\n\r\n", name, 0);
        printf("It should stop\r\n\r\n");
        motor.setSpeed(0);
    }

    static void appTOFTest(VL53L1X & tof, const char * name, const uint8_t & address)
    {
        int error = tof.init(address);
        uint32_t distance;
        printf("Tof %s sensor: ", name);
        if (error) 
        {
            printf("ERROR\r\n");
            printf("Code: (%d)\r\n", error);
        }
        else 
        {
            printf("Found\r\n");
            printf("Default Address: 0x%x\r\n", 0x52);
            printf("New Address: 0x%x\r\n", address);
            error = tof.read(distance);
            printf("Distance: ");
            if (error) 
            { 
                printf("FAIL\r\n");
                printf("Code: (%d)\r\n", error);
            }
            else printf("%dmm\r\n", distance);
        }
        printf("\r\n");
    }

    template <class T>    
    static bool appIsLessThen(const char * name, const T & value, const T & ref, const char * if_less = "", const char * if_not_less = "")
    {
        if (name) printf("%s: %0.2f", name, (float)value);
        if (value < ref)
        {
            printf("%s", if_less);
            return true;
        }
        printf("%s", if_not_less);
        return false;
    }
}

#endif