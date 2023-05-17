#ifndef __ROOBOKART_TEST_TOOLS__
#define __ROOBOKART_TEST_TOOLS__

#include "roobokart_console_app.h"
#include "STSpin240.h"
#include "74HC595.h"

namespace test
{
    static void ir()
    {
        const float good_delay = 0.9f;
        const float sufficient_delay = 0.5f;

        bool warning = false;
        bool error = false;

        AnalogIn left(LEFT_FRONT_IR);
        AnalogIn center(CENTER_FRONT_IR);
        
        float min_left = 1;
        float min_center = 1;
        
        float max_left = 0;
        float max_center = 0;

        float cur_left = 0;
        float cur_center = 0;

        console_app::appTitle("ROOBOKART IR TEST");

        #if defined(ROOBOKART_V1)
        
        AnalogIn right(RIGHT_FRONT_IR);
        
        float min_right = 1;        
        float max_right = 0;  
        float cur_right = 0;

        #elif defined(ROOBOKART_V3)
        
        AnalogIn right_0(RIGHT_FRONT_0_IR);
        AnalogIn right_1(RIGHT_FRONT_1_IR);
        AnalogIn right_2(RIGHT_FRONT_2_IR);

        float min_right_0 = 1;
        float min_right_1 = 1;
        float min_right_2 = 1;
        
        float max_right_0 = 0;
        float max_right_1 = 0;
        float max_right_2 = 0;

        float cur_right_0 = 0;
        float cur_right_1 = 0;
        float cur_right_2 = 0;
        
        #else
        #error Invalid Roobokart Model Selected.
        #endif
        
        console_app::appTitle("ROOBOKART IR TEST");

        printf("This test requires some samples acquisition.\r\n");
        printf("Place the board with the IR sensors facing up.\r\n");
        printf("Show a non-transparent white surface to all sensors at close range and take a sample.\r\n");
        printf("Show a non-transparent opaque black surface to all sensors and take a sample.\r\n");
        printf("Cou can also take samples to one sensor at a time.\r\n");
        printf("Vacuum or distant surface are equivalent to black color.\r\n\r\n");

        console_app::appWaitUserButton("Press USER button to start.\r\n");

        do
        {
            cur_center = center.read();
            cur_left = left.read();

            console_app::appSetMin(min_left, cur_left);
            console_app::appSetMin(min_center, cur_center);
            
            console_app::appSetMax(max_left, cur_left);
            console_app::appSetMax(max_center, cur_center);
            
            printf("            < value >  < min >   < max >\r\n");
            printf("   left ir: %f [%f, %f]\r\n", cur_left, min_left, max_left);
            printf(" center ir: %f [%f, %f]\r\n", cur_center, min_center, max_center); 
            
            #if defined(ROOBOKART_V1)
            
            cur_right = right.read();
            console_app::appSetMin(min_right, cur_right);
            console_app::appSetMax(max_right, cur_right);
            printf("  right ir: %f [%f, %f]\r\n", cur_right, min_right, max_right);

            #elif defined(ROOBOKART_V3)

            cur_right_0 = right_0.read();
            cur_right_1 = right_1.read();
            cur_right_2 = right_2.read();

            console_app::appSetMin(min_right_0, cur_right_0);
            console_app::appSetMin(min_right_1, cur_right_1);
            console_app::appSetMin(min_right_2, cur_right_2);

            console_app::appSetMax(max_right_0, cur_right_0);
            console_app::appSetMax(max_right_1, cur_right_1);
            console_app::appSetMax(max_right_2, cur_right_2);
            
            printf("right 0 ir: %f [%f, %f]\r\n", cur_right_0, min_right_0, max_right_0);
            printf("right 1 ir: %f [%f, %f]\r\n", cur_right_1, min_right_1, max_right_1);
            printf("right 2 ir: %f [%f, %f]\r\n", cur_right_2, min_right_2, max_right_2);
            
            #endif

            printf("\r\n");
            printf("Press USER button to take samples\r\n");
            printf("Hold press USER button to get result\r\n");

        }
        while (!console_app::appWaitUserButton(""));

        cur_left = max_left - min_left;
        cur_center = max_center - min_center;
        
        if (console_app::appIsLessThen("   left ir max difference", cur_left, good_delay, "", " is OK\r\n"))
        {
            warning = true;
            error |= console_app::appIsLessThen(nullptr, cur_left, sufficient_delay, " FAIL\r\n", " is LESS then aspected\r\n");
        }

        if (console_app::appIsLessThen(" center ir max difference", cur_center, good_delay, "", " is OK\r\n"))
        {
            warning = true;
            error |= console_app::appIsLessThen(nullptr, cur_center, sufficient_delay, " FAIL\r\n", " is LESS then aspected\r\n");
        }

        #if defined(ROOBOKART_V1)
        cur_right = max_right - min_right;

        if (console_app::appIsLessThen("  right ir max difference", cur_right, good_delay, "", " is OK\r\n"))
        {
            warning = true;
            error |= console_app::appIsLessThen(nullptr, cur_right, sufficient_delay, " FAIL\r\n", " is LESS then aspected\r\n");
        }

        #elif defined(ROOBOKART_V3)
        cur_right_0 = max_right_0 - min_right_0;
        cur_right_1 = max_right_1 - min_right_1;
        cur_right_2 = max_right_2 - min_right_2;

        if (console_app::appIsLessThen("right_0 ir max difference", cur_right_0, good_delay, "", " is OK\r\n"))
        {
            warning = true;
            error |= console_app::appIsLessThen(nullptr, cur_right_0, sufficient_delay, " FAIL\r\n", " is LESS then aspected\r\n");
        }

        if (console_app::appIsLessThen("right_1 ir max difference", cur_right_1, good_delay, "", " is OK\r\n"))
        {
            warning = true;
            error |= console_app::appIsLessThen(nullptr, cur_right_1, sufficient_delay, " FAIL\r\n", " is LESS then aspected\r\n");
        }

        if (console_app::appIsLessThen("right_2 ir max difference", cur_right_2, good_delay, "", " is OK\r\n"))
        {
            warning = true;
            error |= console_app::appIsLessThen(nullptr, cur_right_2, sufficient_delay, " FAIL\r\n", " is LESS then aspected\r\n");
        }
        
        #endif

        printf("\r\n");
        if (error)
        {
            printf("Test Result: FAIL\r\n");
            printf("\r\nERROR:\r\n");
            printf("Detected sensor below sufficient operating parameters.\r\n");
            printf("Check sensor board flat cable and reboot.\r\n");
        }
        else
        {
            printf("Result: Successful\r\n");
            if (warning)
            {
                printf("\r\nWARNING:\r\n");
                printf("Detected sensor slightly below optimal operating parameters.\r\n");
                printf("Check sensor board flat cable and reboot.\r\n");
            }
        }

        printf("\r\n IR TEST END\r\n");        
        printf(ROOBOKART_APP_DOUBLE_BAR);
    }

    static void motors()
    {        
        STSpin240 motorBoard(F_E_PIN, STANDBY_RST_PIN, DIR_A_PIN, DIR_B_PIN, PWM_A_PIN, PWM_B_PIN, PWM_REF_PIN);
        DCMotor motorLeft(&motorBoard, 0, BRAKING_SPEED);
        DCMotor motorRight(&motorBoard, 1, BRAKING_SPEED);

        console_app::appTitle("ROOBOKART MOTOR TEST");

        console_app::appMotorRun(motorLeft, "LEFT", 50);
        console_app::appMotorRun(motorLeft, "LEFT", 100);
        console_app::appMotorStop(motorLeft, "LEFT");

        console_app::appMotorRun(motorLeft, "LEFT", -50);
        console_app::appMotorRun(motorLeft, "LEFT", -100);
        console_app::appMotorStop(motorLeft, "LEFT");

        console_app::appMotorRun(motorRight, "RIGHT", 50);
        console_app::appMotorRun(motorRight, "RIGHT", 100);
        console_app::appMotorStop(motorRight, "RIGHT");

        console_app::appMotorRun(motorRight, "RIGHT", -50);
        console_app::appMotorRun(motorRight, "RIGHT", -100);
        console_app::appMotorStop(motorRight, "RIGHT");

        printf("Test End\r\n");
        printf(ROOBOKART_APP_DOUBLE_BAR);            
    }

    static void tof()
    {
        #ifdef ROOBOKART_V3
        _74HC595 shift_register(SPI3_MOSI, SPI3_MISO, SPI3_SCK, SHIFT_REG_LE);
        DigitalOut74HC595 en_tof_left(&shift_register, _74HC595::GPIO::GPIO_4);
        DigitalOut74HC595 en_tof_center(&shift_register, _74HC595::GPIO::GPIO_5);
        DigitalOut74HC595 en_tof_right(&shift_register, _74HC595::GPIO::GPIO_6);
        VL53L1X tof_left(I2C3_SDA, I2C3_SCL, &en_tof_left);
        VL53L1X tof_center(I2C3_SDA, I2C3_SCL, &en_tof_center);
        VL53L1X tof_right(I2C3_SDA, I2C3_SCL, &en_tof_right);
        
        // attento agli indirizzi
        console_app::appTitle("ROOBOKART TOF TEST");
        console_app::appTOFTest(tof_left, "LEFT", 0x54);
        console_app::appTOFTest(tof_center, "CENTER", 0x56);
        console_app::appTOFTest(tof_right, "RIGHT", 0x58);

        printf("TEST END\r\n");        
        printf(ROOBOKART_APP_DOUBLE_BAR);
        #endif
    }
}

#endif