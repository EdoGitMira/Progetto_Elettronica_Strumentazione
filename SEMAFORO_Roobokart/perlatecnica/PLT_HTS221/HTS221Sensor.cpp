
/* Includes ------------------------------------------------------------------*/


#include "HTS221Sensor.h"
#include "HTS221_driver.h"


/* Class Implementation ------------------------------------------------------*/

/** Constructor
 * @param i2c object of an helper class which handles the I2C peripheral
 * @param address the address of the component's instance
 */
HTS221Sensor::HTS221Sensor(PinName sda, PinName scl, uint8_t address, const int & frequency)
: SafeI2C(sda, scl, address, frequency)
{
    //assert(i2c);    
};

/**
 * @brief     Initializing the component.
 * @param[in] init pointer to device specific initalization structure.
 * @retval    "0" in case of success, an error code otherwise.
 */
int HTS221Sensor::init()
{
    /* Power down the device */
    if (HTS221_DeActivate((void *)this) == HTS221_ERROR) {        
        return 1;
    }

    /* Enable BDU */
    if (HTS221_Set_BduMode((void *)this, HTS221_ENABLE) == HTS221_ERROR) {
        return 1;
    }

    if (setODR(1.0f) == 1) {
        return 1;
    }

    return 0;
}

/**
 * @brief  Enable HTS221
 * @retval 0 in case of success, an error code otherwise
 */
int HTS221Sensor::enable(void)
{
    /* Power up the device */
    if (HTS221_Activate((void *)this) == HTS221_ERROR) {
        return 1;
    }

    return 0;
}

/**
 * @brief  Disable HTS221
 * @retval 0 in case of success, an error code otherwise
 */
int HTS221Sensor::disable(void)
{
    /* Power up the device */
    if (HTS221_DeActivate((void *)this) == HTS221_ERROR) {
        return 1;
    }

    return 0;
}

/**
 * @brief  Read ID address of HTS221
 * @param  id the pointer where the ID of the device is stored
 * @retval 0 in case of success, an error code otherwise
 */
int HTS221Sensor::getDeviceID(uint8_t & id)
{
    /* Read WHO AM I register */
    if (HTS221_Get_DeviceID((void *)this, &id) == HTS221_ERROR) {
        return 1;
    }
    return 0;
}

/**
 * @brief  Reboot memory content of HTS221
 * @param  None
 * @retval 0 in case of success, an error code otherwise
 */
int HTS221Sensor::reset(void)
{
    uint8_t tmpreg;

    /* Read CTRL_REG2 register */
    if (read_reg(HTS221_CTRL_REG2, &tmpreg) != 0) {
        return 1;
    }

    /* Enable or Disable the reboot memory */
    tmpreg |= (0x01 << HTS221_BOOT_BIT);

    /* Write value to MEMS CTRL_REG2 regsister */
    if (write_reg(HTS221_CTRL_REG2, tmpreg) != 0) {
        return 1;
    }

    return 0;
}

/**
 * @brief  Read HTS221 output register, and calculate the humidity
 * @param  pfData the pointer to data output
 * @retval 0 in case of success, an error code otherwise
 */
int HTS221Sensor::getHumidity(float & pfData)
{
    uint16_t uint16data = 0;

    /* Read data from HTS221. */
    if (HTS221_Get_Humidity((void *)this, &uint16data) == HTS221_ERROR) {
        return 1;
    }

    pfData = (float)uint16data / 10.0f;

    return 0;
}

/**
 * @brief  Read HTS221 output register, and calculate the temperature
 * @param  pfData the pointer to data output
 * @retval 0 in case of success, an error code otherwise
 */
int HTS221Sensor::getTemperature(float & pfData)
{
    int16_t int16data = 0;

    /* Read data from HTS221. */
    if (HTS221_Get_Temperature((void *)this, &int16data) == HTS221_ERROR) {
        return 1;
    }

    pfData = (float)int16data / 10.0f;

    return 0;
}

/**
 * @brief  Read HTS221 output register, and calculate the humidity
 * @param  odr the pointer to the output data rate
 * @retval 0 in case of success, an error code otherwise
 */
int HTS221Sensor::getODR(float & odr)
{
    HTS221_Odr_et odr_low_level;

    if (HTS221_Get_Odr((void *)this, &odr_low_level) == HTS221_ERROR) {
        return 1;
    }

    switch (odr_low_level) {
        case HTS221_ODR_ONE_SHOT:
            odr =  0.0f;
            break;
        case HTS221_ODR_1HZ     :
            odr =  1.0f;
            break;
        case HTS221_ODR_7HZ     :
            odr =  7.0f;
            break;
        case HTS221_ODR_12_5HZ  :
            odr = 12.5f;
            break;
        default                 :
            odr = -1.0f;
            return 1;
    }

    return 0;
}

/**
 * @brief  Set ODR
 * @param  odr the output data rate to be set
 * @retval 0 in case of success, an error code otherwise
 */
int HTS221Sensor::setODR(const float & odr)
{
    HTS221_Odr_et new_odr;

    new_odr = (odr <= 1.0f) ? HTS221_ODR_1HZ
              : (odr <= 7.0f) ? HTS221_ODR_7HZ
              :                   HTS221_ODR_12_5HZ;

    if (HTS221_Set_Odr((void *)this, new_odr) == HTS221_ERROR) {
        return 1;
    }

    return 0;
}


/**
 * @brief Read the data from register
 * @param reg register address
 * @param data register data
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
int HTS221Sensor::read_reg(uint8_t reg, uint8_t *data)
{

    if (HTS221_read_reg((void *)this, reg, 1, data) == HTS221_ERROR) {
        return 1;
    }

    return 0;
}

/**
 * @brief Write the data to register
 * @param reg register address
 * @param data register data
 * @retval 0 in case of success
 * @retval 1 in case of failure
 */
int HTS221Sensor::write_reg(uint8_t reg, uint8_t data)
{

    if (HTS221_write_reg((void *)this, reg, 1, &data) == HTS221_ERROR) {
        return 1;
    }

    return 0;
}

uint8_t HTS221_io_write(void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite)
{
    return ((HTS221Sensor *)handle)->io_write(pBuffer, WriteAddr, nBytesToWrite);
}

uint8_t HTS221_io_read(void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead)
{
    return ((HTS221Sensor *)handle)->io_read(pBuffer, ReadAddr, nBytesToRead);
}

/**
* @brief Utility function to read data.
* @param  pBuffer: pointer to data to be read.
* @param  RegisterAddr: specifies internal address register to be read.
* @param  NumByteToRead: number of bytes to be read.
* @retval 0 if ok, an error code otherwise.
*/
uint8_t HTS221Sensor::io_read(uint8_t *pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead)
{
    return (uint8_t) SafeI2C::readMulti8(RegisterAddr, pBuffer, NumByteToRead);
}

/**
* @brief Utility function to write data.
* @param  pBuffer: pointer to data to be written.
* @param  RegisterAddr: specifies internal address register to be written.
* @param  NumByteToWrite: number of bytes to write.
* @retval 0 if ok, an error code otherwise.
*/
uint8_t HTS221Sensor::io_write(uint8_t *pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite)
{ 
    return (uint8_t) SafeI2C::writeMulti8(RegisterAddr, pBuffer, NumByteToWrite);
}