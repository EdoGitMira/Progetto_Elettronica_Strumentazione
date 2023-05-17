#ifndef ROOBOKART_V1_PINS_H
#define ROOBOKART_V1_PINS_H

/* DON'T MODIFY THE FOLLOWING    */
/* GENERAL ROOBOKART DEFINITIONS */
#include "mbed.h"
#include "roobokart_f401re.h"

//A0 is used by the motor shield
//A2 and A3 are busy

//Front IR sensors
#define CENTER_FRONT_IR  A1
#define LEFT_FRONT_IR    A4
#define RIGHT_FRONT_IR   A5

//#define RIGHT_FRONT_0_IR A5
//#define RIGHT_FRONT_1_IR A5
//#define RIGHT_FRONT_2_IR A5

/* XNucleoIKS01A2 MEMS */
#define IKS01A2_INT1    D11
#define IKS01A2_INT2    D12

/* TCS3200 Color Sensor */
#define TCS3200_S0  PB_14
#define TCS3200_S1  PB_13
#define TCS3200_S2  PB_1
#define TCS3200_S3  PB_15
#define TCS3200_OUT PB_2

/* XNucleo IHM12A1 */
#define F_E_PIN         PB_3    //D2
#define STANDBY_RST_PIN PB_6    //D9
#define DIR_A_PIN       D7      //D6
#define DIR_B_PIN       D6      //D7
#define PWM_A_PIN       D4      //D5
#define PWM_B_PIN       D5      //D4
#define PWM_REF_PIN     A0

/* BUZZER */
#define BUZZER_PIN      PC_9

/* HC05 */
#define BT_TX PA_9
#define BT_RX PA_10

#endif