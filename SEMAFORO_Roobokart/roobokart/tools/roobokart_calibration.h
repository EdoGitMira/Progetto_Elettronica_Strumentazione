#ifndef __ROOBOKART_CALIBRATION_TOOLS__
#define __ROOBOKART_CALIBRATION_TOOLS__

#include "roobokart_console_app.h"


namespace calibration 
{
    static void ir()
    {              
        AnalogIn left(LEFT_FRONT_IR);
        AnalogIn center(CENTER_FRONT_IR);
        DigitalIn button(BUTTON1);
        
        float min_left = 1;
        float min_center = 1;
        
        float max_left = 0;
        float max_center = 0;

        console_app::appTitle("ROOBOKART IR CALIBRATION");

        #if defined(ROOBOKART_V1)
        
        AnalogIn right(RIGHT_FRONT_IR);
        
        float min_right = 1;        
        float max_right = 0;

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
        
        #else
        #error Invalid Roobokart Model Selected.
        #endif

        printf("Put all IR sensors on the black road.\r\n");
        printf("Press USER button when you are done.\r\n");
        
        while (button);
        printf(ROOBOKART_APP_DOUBLE_BAR);
        printf("Move the kart on back and forth on the black road\r\n");
        printf("without lifting it for at least 10 seconds.\r\n");
        printf("Press USER button when you are done.\r\n");
        ThisThread::sleep_for(500);               
        
        do
        {
            console_app::appSetMax(max_left, left.read());
            console_app::appSetMax(max_center, center.read());
            
            #if defined(ROOBOKART_V1)            
            console_app::appSetMax(max_right, right.read());
            
            #elif defined(ROOBOKART_V3)
            console_app::appSetMax(max_right_0, right_0.read());
            console_app::appSetMax(max_right_1, right_1.read());
            console_app::appSetMax(max_right_2, right_2.read());   

            #endif            
        }
        while (button);
        printf(ROOBOKART_APP_DOUBLE_BAR);

        #if defined(ROOBOKART_V1)
        printf("  left max: %f\r\n", max_left);
        printf("center max: %f\r\n", max_center);
        printf(" right max: %f\r\n", max_right);        
        #elif defined(ROOBOKART_V3)
        printf("   left max: %f\r\n", max_left);
        printf(" center max: %f\r\n", max_center);
        printf("right 0 max: %f\r\n", max_right_0);
        printf("right 1 max: %f\r\n", max_right_1);
        printf("right 2 max: %f\r\n", max_right_2);
        #endif

        printf("\r\n");
        printf("Put left and right IR sensors on the white line.\r\n");
        printf("Press USER button when you are done.\r\n");
        ThisThread::sleep_for(500);

        while (button);
        printf(ROOBOKART_APP_DOUBLE_BAR);
        printf("Move the car on left and right on the line\r\n");
        printf("without tilting it for at least 10 seconds.\r\n");
        printf("Press USER button when you are done.\r\n");
        ThisThread::sleep_for(500);               
        
        do
        {
            console_app::appSetMin(min_left, left.read());
            
            #if defined(ROOBOKART_V1)            
            console_app::appSetMin(min_right, right.read());
            
            #elif defined(ROOBOKART_V3)
            console_app::appSetMin(min_right_0, right_0.read());
            console_app::appSetMin(min_right_1, right_1.read());
            console_app::appSetMin(min_right_2, right_2.read());   

            #endif            
        }
        while (button);
        printf(ROOBOKART_APP_DOUBLE_BAR);

        #if defined(ROOBOKART_V1)
        printf("  left min: %f\r\n", min_left);
        printf(" right min: %f\r\n", min_right);        
        #elif defined(ROOBOKART_V3)
        printf("   left min: %f\r\n", min_left);
        printf("right 0 min: %f\r\n", min_right_0);
        printf("right 1 min: %f\r\n", min_right_1);
        printf("right 2 min: %f\r\n", min_right_2);
        #endif
        
        printf("\r\n");
        printf("Put center IR sensor on the blue road sign.\r\n");
        printf("Press USER button when you are done.\r\n");
        ThisThread::sleep_for(500);

        while (button);
        printf(ROOBOKART_APP_DOUBLE_BAR);
        printf("Move the car on left and right on the blue road sign\r\n");
        printf("without tilting it for at least 10 seconds.\r\n");
        printf("Press USER button when you are done.\r\n");
        ThisThread::sleep_for(500);               
        
        do
        {
            console_app::appSetMin(min_center, center.read());
        }
        while (button);
        printf(ROOBOKART_APP_DOUBLE_BAR);

        #if defined(ROOBOKART_V1)
        printf("center min: %f\r\n", min_center);       
        #elif defined(ROOBOKART_V3)
        printf(" center min: %f\r\n", min_center);
        #endif

        printf("\r\n");
        printf("Check following data and replace it in:\r\n");
        printf("""roobokart_def.h""\r\n\r\n");

        #if defined(ROOBOKART_V1)
        printf("#define   IR_L_MIN_VALUE %ff\r\n", min_left);
        printf("#define   IR_L_MAX_VALUE %ff\r\n", max_left);
        printf("#define   IR_C_MIN_VALUE %ff\r\n", min_center);       
        printf("#define   IR_C_MAX_VALUE %ff\r\n", max_center);
        printf("#define IR_R_0_MIN_VALUE %ff\r\n", min_right);        
        printf("#define IR_R_0_MAX_VALUE %ff\r\n", max_right);  
        printf("#define IR_R_1_MIN_VALUE %ff\r\n", min_right);        
        printf("#define IR_R_1_MAX_VALUE %ff\r\n", max_right);  
        printf("#define IR_R_2_MIN_VALUE %ff\r\n", min_right);        
        printf("#define IR_R_2_MAX_VALUE %ff\r\n", max_right);        
        #elif defined(ROOBOKART_V3)
        printf("#define   IR_L_MIN_VALUE %ff\r\n", min_left);
        printf("#define   IR_L_MAX_VALUE %ff\r\n", max_left);
        printf("#define   IR_C_MIN_VALUE %ff\r\n", min_center);
        printf("#define   IR_C_MAX_VALUE %ff\r\n", max_center);
        printf("#define IR_R_0_MIN_VALUE %ff\r\n", min_right_0);
        printf("#define IR_R_0_MAX_VALUE %ff\r\n", max_right_0);
        printf("#define IR_R_1_MIN_VALUE %ff\r\n", min_right_1);
        printf("#define IR_R_1_MAX_VALUE %ff\r\n", max_right_1);
        printf("#define IR_R_2_MIN_VALUE %ff\r\n", min_right_2);
        printf("#define IR_R_2_MAX_VALUE %ff\r\n", max_right_2);
        #endif

        printf("\r\nCalibration End\r\n");
        printf(ROOBOKART_APP_DOUBLE_BAR);
    }
}

#endif