#ifndef __ROOBOKART_F401RE_H__
#define __ROOBOKART_F401RE_H__

/* UNUSED */
#undef LED2
#undef LED3
#undef LED4
#undef LED_RED

/* BUSES */
#undef  I2C_SDA
#define I2C_SDA D14

#undef  I2C_SCL
#define I2C_SCL D15

#undef  I2C1_SDA
#define I2C1_SDA I2C_SDA

#undef  I2C1_SCL
#define I2C1_SCL I2C_SCL

#undef  I2C2_SDA
#define I2C2_SDA PB_3

#undef  I2C2_SCL
#define I2C2_SCL PB_10

#undef  I2C3_SDA
#define I2C3_SDA PB_4

#undef  I2C3_SCL
#define I2C3_SCL PA_8

#undef  I2C3_SDA
#define I2C3_SDA PB_4

#undef  I2C3_SCL
#define I2C3_SCL PA_8

#undef SPI3_MOSI
#define SPI3_MOSI PC_12

#undef SPI3_MISO
#define SPI3_MISO PC_11

#undef SPI3_SCK
#define SPI3_SCK  PC_10

#endif