#ifndef __ROOBOKART_BUILD_CHECK_H__
#define __ROOBOKART_BUILD_CHECK_H__

#ifndef __ROOBOKART_DEF_H__
#define ROOBOKART_VERSION_SELECTED
#endif

#if defined(ROOBOKART_V1)
#ifdef ROOBOKART_VERSION_SELECTED
#define ROOBOKART_MULTIPLE_VERSION_SELECTED
#endif
#define ROOBOKART_VERSION_SELECTED
#endif

#if defined(ROOBOKART_V3)
#ifdef ROOBOKART_VERSION_SELECTED
#define ROOBOKART_MULTIPLE_VERSION_SELECTED
#endif
#define ROOBOKART_VERSION_SELECTED
#endif

#if defined(ROOBOKART_M1)
#ifdef ROOBOKART_VERSION_SELECTED
#define ROOBOKART_MULTIPLE_VERSION_SELECTED
#endif
#define ROOBOKART_VERSION_SELECTED
#endif

#ifndef ROOBOKART_VERSION_SELECTED
#error Error: Please select a roobokart valid version before compiling. Look at "roobokart_def.h"
#endif

#ifdef ROOBOKART_MULTIPLE_VERSION_SELECTED
#error Error: Multiple roobokart version selected. Look at "roobokart_def.h"
#endif

#if defined(ROOBOKART_V1) || defined(ROOBOKART_V3)
#ifndef TARGET_NUCLEO_F401RE
#error Error: Invalid Board Target, select "NUCLEO_F401RE"
#endif
#elif defined(ROOBOKART_M1)
#ifndef TARGET_DISCO_L475VG_IOT01A
#error Error: Invalid Board Target, select "DISCO_L475VG_IOT01A"
#endif
#endif

#endif