/**
 ******************************************************************************
 * @file    LSM303AGRAccSensor.h
 * @author  CLab
 * @version V1.0.0
 * @date    5 August 2016
 * @brief   Abstract Class of an LSM303AGR accelerometer sensor.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Prevent recursive inclusion -----------------------------------------------*/

#ifndef __LSM303AGRAccSensor_H__
#define __LSM303AGRAccSensor_H__


/* Includes ------------------------------------------------------------------*/

#include "SafeI2C.h"
#include "LSM303AGR_acc_driver.h"
#include <assert.h>

/* Defines -------------------------------------------------------------------*/
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_2G_NORMAL_MODE               3.900f  /**< Sensitivity value for 2 g full scale and normal mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_2G_HIGH_RESOLUTION_MODE      0.980f  /**< Sensitivity value for 2 g full scale and high resolution mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_2G_LOW_POWER_MODE           15.630f  /**< Sensitivity value for 2 g full scale and low power mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_4G_NORMAL_MODE               7.820f  /**< Sensitivity value for 4 g full scale and normal mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_4G_HIGH_RESOLUTION_MODE      1.950f  /**< Sensitivity value for 4 g full scale and high resolution mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_4G_LOW_POWER_MODE           31.260f  /**< Sensitivity value for 4 g full scale and low power mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_8G_NORMAL_MODE              15.630f  /**< Sensitivity value for 8 g full scale and normal mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_8G_HIGH_RESOLUTION_MODE      3.900f  /**< Sensitivity value for 8 g full scale and high resolution mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_8G_LOW_POWER_MODE           62.520f  /**< Sensitivity value for 8 g full scale and low power mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_16G_NORMAL_MODE             46.900f  /**< Sensitivity value for 16 g full scale and normal mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_16G_HIGH_RESOLUTION_MODE    11.720f  /**< Sensitivity value for 16 g full scale and high resolution mode [mg/LSB] */
#define LSM303AGR_ACC_SENSITIVITY_FOR_FS_16G_LOW_POWER_MODE         187.580f  /**< Sensitivity value for 16 g full scale and low power mode [mg/LSB] */

/* Class Declaration ---------------------------------------------------------*/

/**
 * Class of an LSM303AGR Inertial Measurement Unit (IMU) 6 axes
 * sensor.
 */
class LSM303AGRAccSensor : protected SafeI2C
{    
    uint8_t _is_enabled;
    float _last_odr;
    
    public:
    LSM303AGRAccSensor(PinName sda, PinName scl, uint8_t address=LSM303AGR_ACC_I2C_ADDRESS, const int & frequency = 1000000);
    int init(void *init);
    int read_id(uint8_t *id);
    int get_x_axes(int32_t *pData);
    int get_x_axes_raw(int16_t *pData);
    int get_x_sensitivity(float *pfData);
    int get_x_odr(float *odr);
    int set_x_odr(float odr);
    int get_x_fs(float *fullScale);
    int set_x_fs(float fullScale);
    int enable(void);
    int disable(void);
    int read_reg(uint8_t reg, uint8_t *data);
    int write_reg(uint8_t reg, uint8_t data);
    
    /**
     * @brief Utility function to read data.
     * @param  pBuffer: pointer to data to be read.
     * @param  RegisterAddr: specifies internal address register to be read.
     * @param  NumByteToRead: number of bytes to be read.
     * @retval 0 if ok, an error code otherwise.
     */
    uint8_t io_read(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead)
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
    uint8_t io_write(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite)
    {               
        return (uint8_t) SafeI2C::writeMulti8(RegisterAddr, pBuffer, NumByteToWrite);
    }

  private:
    int set_x_odr_when_enabled(float odr);
    int set_x_odr_when_disabled(float odr);
    int get_x_sensitivity_normal_mode(float *sensitivity );
    int get_x_sensitivity_lp_mode(float *sensitivity );
    int get_x_sensitivity_hr_mode(float *sensitivity );

};


uint8_t LSM303AGR_ACC_io_write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite );
uint8_t LSM303AGR_ACC_io_read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead );


#endif
