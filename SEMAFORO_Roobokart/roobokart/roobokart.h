#ifndef __ROOBOKART_H__
#define __ROOBOKART_H__

#include "mbed.h"
#include "roobokart_def.h"
#include "KartTypes.h"
#include "KartObstacles.h"
#include "RoadLinesReader.h"
#include "DualMotor.h"
#include "TrafficLightsReader.h"
#include "Gyro.h"
#include "PID.h"
#include "map.h"
#include "VL53L0X.h"
#include "VL53L1X.h"
#include "VD6283.h"
#include "TCS3200.h"
#include "KartV1GPIOE.h"
#include "KartError.h"
#include "KartGUI.h"
#include "Buzzer.h"
#include "STSpin240.h"
#include "DCMotor.h"
#include <cstdint>

class Roobokart
{    
    KartMode currentmode;
    KartMode previousmode;

    public:
    #if defined(ROOBOKART_V1)

    KartV1GPIOE gpioe1, gpioe2;
    DigitalOutKartV1GPIOE en_tof_left, en_tof_right;
    VL53L0X tof_left, tof_right;
    TCS3200 color;
    AnalogIR irRight;
    
    #elif defined(ROOBOKART_V3)
    
    _74HC595 shift_register;
    DigitalOut74HC595 en_tof_left, en_tof_center, en_tof_right;
    VL53L1X tof_left, tof_center, tof_right;
    VD6283 color;   
    
    AnalogIR irRight_0;
    AnalogIR irRight_1;
    AnalogIR irRight_2;
    
    #else
    #endif     
    Buzzer buzzer;   
    AnalogIR irLeft;
    AnalogIR irCenter;

    STSpin240 motorBoard;
    DCMotor motorLeft;
    DCMotor motorRight;
    DualMotor movements;

    Gyro gyro;
    KartGUI gui;
    RoadLinesReader road;
    TrafficLightsReader trafficlight;
    KartObstacles obstacles;

    Roobokart();
    int init();    
    void setMode(const KartMode & nextmode);
    const KartMode & getMode() const;

    void modeObstacles()
    {
        gui.debugClear();
        gui.kartMode(KartMode::Obstacles);
        do
        {
            movements.brake();
            ThisThread::sleep_for(1);
        }
        while (obstacles.detected());

        return setMode(previousmode);
    }

    void modeError()
    {
        DigitalOut led(LED1);
        movements.stop();

        KartError error("Mode Error");
        error.setError(KartError::code_t::METHOD_NOT_IMPLEMENTED);
        gui.kartError(error);

        while (true)
        {
            led = !led;
            buzzer.tone(Buzzer::NOTE_A2, 500);
            ThisThread::sleep_for(500);
        }       
        return setMode(KartMode::Error);
    }

    void modeStartup()
    {        
        Timer timer;
        DigitalIn button(BUTTON1);
        gui.debugMessage("  [ Press User Button ]");
        
        while (button.read()) ThisThread::yield();
        
        timer.start();

        while (!button.read()) ThisThread::sleep_for(100);
        
        ThisThread::sleep_for(100);

        if (timer.read_ms()<1000)
        {
            // press 
        }
        else
        {
			 // hold press - Option 2
        }

        return setMode(KartMode::Navigation);
    }

    void modeNavigation()
    {
        PID<float> pid(NAV_PID_KP, NAV_PID_KI, NAV_PID_KD);
        Timer timer;
        bool outrange;
        float line, direction, speed, power;
        gui.kartMode(KartMode::Navigation);

        timer.start();
        ThisThread::sleep_for(100);
        while(road.isCenterBlack())
        {
            if (obstacles.detected()) return setMode(KartMode::Obstacles);            
            
            line = road.readLine();          
            direction = pid.evaluate(timer.read(), NAV_PID_SP, line);
            timer.reset();
            
            power = min(1.0f-abs(NAV_PID_SP-line), obstacles.safety());
            speed = mapAndLimit<float>(power, 0.0f, 1.0f, 30.0f, NAV_MAX_SPEED);
            
            movements.tank(direction, speed);           
                                    
            ThisThread::sleep_for(100);
        }
        return setMode(KartMode::RoadSign);
    }    

    void modeRoadSign()
    {
        bool lA, rA;
        float direction; 
        Timer timer;
        PID<float> pid(GYRO_PID_KP, GYRO_PID_KD, GYRO_PID_KI);

        movements.stop();
        gui.kartMode(KartMode::RoadSign);
        gui.debugMessage("Alignment");
        
        do
        {
            if (road.isLeftBlack()) 
            {
                motorLeft.setSpeed(RS_SEEK_SPEED);
                lA = true;
            }
            else
            {
                motorLeft.setSpeed(RS_ALIG_SPEED);
                lA = false;
            }
            if (road.isRightBlack()) 
            {
                motorRight.setSpeed(RS_SEEK_SPEED);
                rA = true;
            }
            else
            {
                motorRight.setSpeed(RS_ALIG_SPEED);
                rA = false;
            }
        }
        while(lA || rA);
        
        do
        {
            if (road.isLeftWhite()) 
            {
                motorLeft.setSpeed(RS_ALIG_SPEED);
                lA = true;
            }
            else
            {
                motorLeft.setSpeed(BRAKING_SPEED);
                lA = false;
            }
            if (road.isRightWhite()) 
            {
                motorRight.setSpeed(RS_ALIG_SPEED);
                rA = true;
            }
            else
            {
                motorRight.setSpeed(BRAKING_SPEED);
                rA = false;
            }
        }
        while(lA || rA);

        gui.debugMessage("Reset gyro");
        ThisThread::sleep_for(100);
        gyro.waitForYawStab(GYRO_STAB_TIMEOUT, GYRO_STAB_THRESHOLD);

        gui.debugMessage("Reading");
        gyro.setYawOffset();
        timer.start();
        road.roadSignStart();
        while (road.roadSignProcess())
        {
            direction = pid.evaluate(timer.read(), 0.0f, gyro.getYaw());
            timer.reset();    
            movements.car(direction, RS_READ_SPEED);
        }
        movements.stop();
        
        char c[2] = { 0, 0 };
        c[0] = 48 + road.getRoadSign();
        gui.screenSplash(c);

        return setMode(KartMode::TrafficLight);
    }

    void modeTrafficLight()
    {    
        float direction;
        PID<float> pid(GYRO_PID_KP, GYRO_PID_KD, GYRO_PID_KI);   
        Timer tPID, tSafe; 
        gui.kartMode(KartMode::TrafficLight);
        tPID.start();
        tSafe.start();

        while (true)
        {
            gui.debugReset();

            switch (trafficlight.read())
            {
                case TrafficLightsReader::Result::GREEN:                           
                return setMode(KartMode::Crossing);

                case TrafficLightsReader::Result::BLACK:               
                gui.debugMessage("SCAN");                

                direction = pid.evaluate(tPID.read(), -5.0f, gyro.getYaw());
                tPID.reset();    
                movements.car(direction, TL_SEEK_SPEED);

                if (tSafe.read_ms() >= TRAFFIC_LIGHT_TIMEOUT) return setMode(KartMode::Navigation);
                break;
                
                default:
                movements.brake();
                gui.debugMessage("STOP");
                tSafe.reset();
                break;
            }
        }        
    }

    void modeCrossing()
    {
        return setMode(KartMode::Navigation);
    }
};

#endif