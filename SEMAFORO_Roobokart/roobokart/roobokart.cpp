#include "roobokart.h"
#include <cstdint>

#if defined(ROOBOKART_V1)

Roobokart::Roobokart() :
motorBoard(F_E_PIN, STANDBY_RST_PIN, DIR_A_PIN, DIR_B_PIN, PWM_A_PIN, PWM_B_PIN, PWM_REF_PIN),
motorLeft(&motorBoard, 0, BRAKING_SPEED),
motorRight(&motorBoard, 1, BRAKING_SPEED),
movements(&motorLeft,&motorRight),
buzzer(BUZZER_PIN),
irLeft(LEFT_FRONT_IR, IR_L_MIN_VALUE, IR_L_MAX_VALUE),
irCenter(CENTER_FRONT_IR, IR_C_MIN_VALUE, IR_C_MAX_VALUE),
irRight(RIGHT_FRONT_IR, IR_R_0_MIN_VALUE, IR_R_0_MAX_VALUE),
color(TCS3200_S0,TCS3200_S1,TCS3200_S2,TCS3200_S3,TCS3200_OUT),
gpioe1(I2C1_SDA, I2C1_SCL, 0x42*2),
gpioe2(I2C1_SDA, I2C1_SCL, 0x43*2),
gyro(I2C1_SDA, I2C1_SCL),
gui(&gpioe1, &gpioe2),
en_tof_left(&gpioe1, (KartV1GPIOE::GPIOName) 0b1111111111111111), // to fix
en_tof_right(&gpioe2, (KartV1GPIOE::GPIOName) 0b1111111111111111), // to fix
tof_left(I2C1_SDA, I2C1_SCL, &en_tof_left),
tof_right(I2C1_SDA, I2C1_SCL, &en_tof_right),
road(&irLeft, &irCenter, &irRight),
trafficlight((ColorSensor*)&color, COLOR_THRESHOLD, 100),
obstacles(&tof_left, nullptr, &tof_right)
{
    motorBoard.setFrequency(20000);
    motorBoard.setCurrentLimit(50);
} 

#elif defined(ROOBOKART_V3)

Roobokart::Roobokart() :
motorBoard(F_E_PIN, STANDBY_RST_PIN, DIR_A_PIN, DIR_B_PIN, PWM_A_PIN, PWM_B_PIN, PWM_REF_PIN),
motorLeft(&motorBoard, 0, BRAKING_SPEED),
motorRight(&motorBoard, 1, BRAKING_SPEED),
movements(&motorLeft,&motorRight),
buzzer(BUZZER_PIN),
irLeft(LEFT_FRONT_IR, IR_L_MIN_VALUE, IR_L_MAX_VALUE),
irCenter(CENTER_FRONT_IR, IR_C_MIN_VALUE, IR_C_MAX_VALUE),
irRight_0(RIGHT_FRONT_0_IR, IR_R_0_MIN_VALUE, IR_R_0_MAX_VALUE),
irRight_1(RIGHT_FRONT_1_IR, IR_R_1_MIN_VALUE, IR_R_1_MAX_VALUE),
irRight_2(RIGHT_FRONT_2_IR, IR_R_2_MIN_VALUE, IR_R_2_MAX_VALUE),
color(I2C3_SDA, I2C3_SCL),
shift_register(SPI3_MOSI, SPI3_MISO, SPI3_SCK, SHIFT_REG_LE),
gyro(I2C1_SDA, I2C1_SCL),
gui(&shift_register),
en_tof_left(&shift_register, _74HC595::GPIO::GPIO_4),
en_tof_center(&shift_register, _74HC595::GPIO::GPIO_5),
en_tof_right(&shift_register, _74HC595::GPIO::GPIO_6),      
tof_left(I2C3_SDA, I2C3_SCL, &en_tof_left),
tof_center(I2C3_SDA, I2C3_SCL, &en_tof_center),
tof_right(I2C3_SDA, I2C3_SCL, &en_tof_right),
road(&irLeft, &irCenter, &irRight_1, &irRight_2),
trafficlight(&color, COLOR_THRESHOLD, 100),
obstacles(&tof_left, &tof_center, &tof_right)
{
    motorBoard.setFrequency(100);
    motorBoard.setCurrentLimit(100);
} 

#else
#endif

int Roobokart::init() 
{
    int err = 0;
    currentmode = KartMode::Startup;
    previousmode = KartMode::Startup;

    err = gui.init();
    if (err) return err;

    gui.kartLogo();

    printf("gyro.init()\r\n");
    err = gyro.init();        
    if (err) return err;
    
    printf("road.init()\r\n");
    err = road.init(); 
    if (err) return err;

    printf("trafficlight.init()\r\n");
    err = trafficlight.init();
    if (err) return err;

    #ifndef ROOBOKART_M1
    printf("tof_left.init(0x54)\r\n");
    err = tof_left.init(0x54);
    if (err) return err;

    #if defined(ROOBOKART_V3)
    printf("tof_center.init(0x56)\r\n");
    err = tof_center.init(0x56);
    if (err) return err;
    #endif

    printf("tof_right.init(0x58)\r\n");
    err = tof_right.init(0x58);  
    if (err) return err;

    printf("obstacles.init()\r\n");
    err = obstacles.init();
    #endif

    return err;
}

void Roobokart::setMode(const KartMode & nextmode)
{
    previousmode = currentmode;
    currentmode = nextmode;
}

const KartMode & Roobokart::getMode() const 
{
    return currentmode;
}