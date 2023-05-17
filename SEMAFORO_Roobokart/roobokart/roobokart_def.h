#ifndef __ROOBOKART_DEF_H__
#define __ROOBOKART_DEF_H__

/*********************************************
* SPECIFIC ROOBOKART DEFINITIONS             *
* THE FOLLOWING PARAMETERS CAN BE MODIFIED   *
* EACH ROOBOKART COULD HAVE DIFFERENT VALUES *
*********************************************/

// include custom kart profile
//#include "profiles/RKV3_Custom1.h"

#ifndef ROOBOKART_PROFILE_SELECTED // Default Settings
//=================================================

// Profile Name
#define ROOBOKART_PROFILE_NAME "Default"

//Choose your Roobokart version
//#define ROOBOKART_V1
#define ROOBOKART_V3

// NAV PID
#define NAV_PID_SP 0.0f
#define NAV_PID_KP 10.0f
#define NAV_PID_KI 05.0f
#define NAV_PID_KD 02.0f  

// GYRO PID
#define GYRO_PID_KP 5.00f
#define GYRO_PID_KI 0.10f
#define GYRO_PID_KD 0.00f

// SAFETY
#define BRAKING_MAX_DIST 80
#define BRAKING_REV_DIST 50
#define BRAKING_MIN_DIST 40

// SPEED
#define BRAKING_SPEED -10
#define NAV_MAX_SPEED 45
#define NAV_MIN_SPEED 35
#define NAV_AVE_SPEED (float)(NAV_MIN_SPEED+NAV_MAX_SPEED)/2
#define RS_SEEK_SPEED NAV_AVE_SPEED
#define RS_ALIG_SPEED NAV_MIN_SPEED
#define RS_READ_SPEED NAV_MIN_SPEED
#define TL_SEEK_SPEED NAV_MIN_SPEED
#define      CR_SPEED NAV_MAX_SPEED

// CROSS ANGLE
#define   CROSS_LEFT_ANGLE 45
#define CROSS_CENTER_ANGLE -1
#define  CROSS_RIGHT_ANGLE -5

// TIMING
#define     GYRO_STAB_TIMEOUT 20000
#define TRAFFIC_LIGHT_TIMEOUT 1500
#define  CROSS_BEFORE_TIMEOUT 800
#define    CROSS_LEFT_TIMEOUT 2000
#define   CROSS_RIGHT_TIMEOUT 1000
#define   CROSS_FRONT_TIMEOUT 2000

// THRESHOLD
#define   GYRO_STAB_THRESHOLD 0.1f
#define       COLOR_THRESHOLD 0.4f

// IR OFFSETS
#define   IR_L_MIN_VALUE 0.0f
#define   IR_L_MAX_VALUE 1.0f
#define   IR_C_MIN_VALUE 0.0f
#define   IR_C_MAX_VALUE 1.0f
#define IR_R_0_MIN_VALUE 0.0f
#define IR_R_0_MAX_VALUE 1.0f
#define IR_R_1_MIN_VALUE 0.0f
#define IR_R_1_MAX_VALUE 1.0f
#define IR_R_2_MIN_VALUE 0.0f
#define IR_R_2_MAX_VALUE 1.0f

//=================================================
#endif  // End Default Settings

// This file check build settings
#include "roobokart_build_check.h"

// Include hardware configuration
#if defined(ROOBOKART_V1)
#include "roobokart_v1_pins.h"
#elif defined(ROOBOKART_V3)
#include "roobokart_v3_pins.h"
#endif

#endif