#ifndef __VL53L1X_H__
#define __VL53L1X_H__

#include "mbed.h"
#include "SafeI2C.h"
#include "PLTGPIO.h"
#include "RangeSensor.h"

class VL53L1X : public RangeSensor, protected SafeI2C
{
    PinName enable_pin;
    PLTDigitalOutBase * external_enable;   

    public:

    enum Exception : int
    {
        NO_ERROR = 0,                       /*!<No error. The Range is valid. */
        RANGESTATUS_SIGMA_FAIL = 1,         /*!<Sigma Fail. */
        RANGESTATUS_SIGNAL_FAIL = 2,        /*!<Signal fail. */
        RANGESTATUS_RANGE_VALID_MIN_RANGE_CLIPPED = 3,  /*!<Target is below minimum detection threshold. */
        RANGESTATUS_OUTOFBOUNDS_FAIL = 4,   /*!<Phase out of valid limits -  different to a wrap exit. */
        RANGESTATUS_HARDWARE_FAIL = 5,      /*!<Hardware fail. */
        RANGESTATUS_RANGE_VALID_NO_WRAP_CHECK_FAIL = 6, /*!<The Range is valid but the wraparound check has not been done. */
        RANGESTATUS_WRAP_TARGET_FAIL = 7,   /*!<Wrapped target - no matching phase in other VCSEL period timing. */
        RANGESTATUS_PROCESSING_FAIL	= 8,    /*!<Internal algo underflow or overflow in lite ranging. */
        RANGESTATUS_XTALK_SIGNAL_FAIL = 9,  /*!<Specific to lite ranging. */
        RANGESTATUS_SYNCRONISATION_INT = 10,/*!<1st interrupt when starting ranging in back to back mode. Ignore data. */
        RANGESTATUS_RANGE_VALID_MERGED_PULSE = 11, /*!<All Range ok but object is result of multiple pulses merging together. Used by RQL for merged pulse detection. */
        RANGESTATUS_TARGET_PRESENT_LACK_OF_SIGNAL = 12, /*!<Used  by RQL  as different to phase fail. */
        RANGESTATUS_MIN_RANGE_FAIL = 13,    /*!<User ROI input is not valid e.g. beyond SPAD Array.*/
        RANGESTATUS_RANGE_INVALID = 14,     /*!<lld returned valid range but negative value ! */
        ERROR_DEVICE_NOT_FOUND,             /*!<No device found at the selected address. */
        ERROR_DEVICE_NOT_COMPATIBLE,        /*!<Device found but not compatible */
        ERROR_DEVICE_INVALID_DATA,          /*!<Data recived from device are inconsistent. */
        ERROR_I2C_FAIL,                     /*!<I2C Fail. */
        ERROR_INVALID_DISTANCE_MODE,        /*!<Current range mode is wrong. */
        ERROR_INVALID_ARGUMENTS,            /*!<Method invalid parameters */
        RANGESTATUS_NONE = 255,             /*!<No Update. */
    };

    enum class Range : uint16_t
    {
        Short = 1,
        Long = 2
    };
    
    VL53L1X(PinName sda, PinName scl, const PinName & cs, const uint8_t & boot_address = 0x52, const int & frequency = 1000000);
    VL53L1X(PinName sda, PinName scl, PLTDigitalOutBase * cs, const uint8_t & boot_address = 0x52, const int & frequency = 1000000);
    
    int init(const uint8_t & address = 0x52);
    int read(uint32_t & distance) override;

    int setRangeMode(const Range & mode);
    int enable();
    int disable();
    
    bool isPresent();

    private:

    int private_init();

    /* VL53L1X API  */
    
    protected:

    //typedef int8_t VL53L1X_ERROR;
    typedef Exception VL53L1X_ERROR;
        
    /** use where fractional values are expected
    *
    * Given a floating point value f it's .16 bit point is (int)(f*(1<<16))*/
    typedef uint32_t FixPoint1616_t;

    /**
    *  @brief defines packed reading results type
    */
    typedef struct {
        uint8_t Status;		/*!< ResultStatus */
        uint16_t Distance;	/*!< ResultDistance */
        uint16_t Ambient;	/*!< ResultAmbient */
        uint16_t SigPerSPAD;/*!< ResultSignalPerSPAD */
        uint16_t NumSPADs;	/*!< ResultNumSPADs */
    } VL53L1X_Result_t;
    
    /**
    * @brief This function sets the sensor I2C address used in case multiple devices application, default address 0x52
    */
    VL53L1X_ERROR VL53L1X_SetI2CAddress(uint8_t new_address);

    /**
    * @brief This function loads the 135 bytes default values to initialize the sensor.
    * @param dev Device address
    * @return 0:success, != 0:failed
    */
    VL53L1X_ERROR VL53L1X_SensorInit();

    /**
    * @brief This function clears the interrupt, to be called after a ranging data reading
    * to arm the interrupt for the next data ready event.
    */
    VL53L1X_ERROR VL53L1X_ClearInterrupt();

    /**
    * @brief This function programs the interrupt polarity\n
    * 1=active high (default), 0=active low
    */
    VL53L1X_ERROR VL53L1X_SetInterruptPolarity(uint8_t IntPol);

    /**
    * @brief This function returns the current interrupt polarity\n
    * 1=active high (default), 0=active low
    */
    VL53L1X_ERROR VL53L1X_GetInterruptPolarity(uint8_t *pIntPol);

    /**
    * @brief This function starts the ranging distance operation\n
    * The ranging operation is continuous. The clear interrupt has to be done after each get data to allow the interrupt to raise when the next data is ready\n
    * 1=active high (default), 0=active low, use SetInterruptPolarity() to change the interrupt polarity if required.
    */
    VL53L1X_ERROR VL53L1X_StartRanging();

    /**
    * @brief This function stops the ranging.
    */
    VL53L1X_ERROR VL53L1X_StopRanging();

    /**
    * @brief This function checks if the new ranging data is available by polling the dedicated register.
    * @param : isDataReady==0 -> not ready; isDataReady==1 -> ready
    */
    VL53L1X_ERROR VL53L1X_CheckForDataReady(uint8_t *isDataReady);

    /**
    * @brief This function programs the timing budget in ms.
    * Predefined values = 15, 20, 33, 50, 100(default), 200, 500.
    */
    VL53L1X_ERROR VL53L1X_SetTimingBudgetInMs(uint16_t TimingBudgetInMs);

    /**
    * @brief This function returns the current timing budget in ms.
    */
    VL53L1X_ERROR VL53L1X_GetTimingBudgetInMs(uint16_t *pTimingBudgetInMs);

    /**
    * @brief This function programs the distance mode (1=short, 2=long(default)).
    * Short mode max distance is limited to 1.3 m but better ambient immunity.\n
    * Long mode can range up to 4 m in the dark with 200 ms timing budget.
    */
    VL53L1X_ERROR VL53L1X_SetDistanceMode(uint16_t DistanceMode);

    /**
    * @brief This function returns the current distance mode (1=short, 2=long).
    */
    VL53L1X_ERROR VL53L1X_GetDistanceMode(uint16_t *pDistanceMode);

    /**
    * @brief This function programs the Intermeasurement period in ms\n
    * Intermeasurement period must be >/= timing budget. This condition is not checked by the API,
    * the customer has the duty to check the condition. Default = 100 ms
    */
    VL53L1X_ERROR VL53L1X_SetInterMeasurementInMs(uint32_t InterMeasurementInMs);

    /**
    * @brief This function returns the Intermeasurement period in ms.
    */
    VL53L1X_ERROR VL53L1X_GetInterMeasurementInMs(uint16_t * pIM);

    /**
    * @brief This function returns the boot state of the device (1:booted, 0:not booted)
    */
    VL53L1X_ERROR VL53L1X_BootState(uint8_t *state);

    /**
    * @brief This function returns the sensor id, sensor Id must be 0xEACC
    */
    VL53L1X_ERROR VL53L1X_GetSensorId(uint16_t *id);

    /**
    * @brief This function returns the distance measured by the sensor in mm
    */
    VL53L1X_ERROR VL53L1X_GetDistance(uint32_t *distance);

    /**
    * @brief This function returns the returned signal per SPAD in kcps/SPAD.
    * With kcps stands for Kilo Count Per Second
    */
    VL53L1X_ERROR VL53L1X_GetSignalPerSpad(uint16_t *signalPerSp);

    /**
    * @brief This function returns the ambient per SPAD in kcps/SPAD
    */
    VL53L1X_ERROR VL53L1X_GetAmbientPerSpad(uint16_t *amb);

    /**
    * @brief This function returns the returned signal in kcps.
    */
    VL53L1X_ERROR VL53L1X_GetSignalRate(uint16_t *signalRate);

    /**
    * @brief This function returns the current number of enabled SPADs
    */
    VL53L1X_ERROR VL53L1X_GetSpadNb(uint16_t *spNb);

    /**
    * @brief This function returns the ambient rate in kcps
    */
    VL53L1X_ERROR VL53L1X_GetAmbientRate(uint16_t *ambRate);

    /**
    * @brief This function returns the ranging status error \n
    * (0:no error, 1:sigma failed, 2:signal failed, ..., 7:wrap-around)
    */
    VL53L1X_ERROR VL53L1X_GetRangeStatus(uint8_t *rangeStatus);

    /**
    * @brief This function returns measurements and the range status in a single read access
    */
    VL53L1X_ERROR VL53L1X_GetResult(VL53L1X_Result_t *pResult);

    /**
    * @brief This function programs the offset correction in mm
    * @param OffsetValue:the offset correction value to program in mm
    */
    VL53L1X_ERROR VL53L1X_SetOffset(int16_t OffsetValue);

    /**
    * @brief This function returns the programmed offset correction value in mm
    */
    VL53L1X_ERROR VL53L1X_GetOffset(int16_t *Offset);

    /**
    * @brief This function programs the xtalk correction value in cps (Count Per Second).\n
    * This is the number of photons reflected back from the cover glass in cps.
    */
    VL53L1X_ERROR VL53L1X_SetXtalk(uint16_t XtalkValue);

    /**
    * @brief This function returns the current programmed xtalk correction value in cps
    */
    VL53L1X_ERROR VL53L1X_GetXtalk(uint16_t *Xtalk);

    /**
    * @brief This function programs the threshold detection mode\n
    * Example:\n
    * VL53L1X_SetDistanceThreshold(dev,100,300,0,1): Below 100 \n
    * VL53L1X_SetDistanceThreshold(dev,100,300,1,1): Above 300 \n
    * VL53L1X_SetDistanceThreshold(dev,100,300,2,1): Out of window \n
    * VL53L1X_SetDistanceThreshold(dev,100,300,3,1): In window \n
    * @param   dev : device address
    * @param  	ThreshLow(in mm) : the threshold under which one the device raises an interrupt if Window = 0
    * @param 	ThreshHigh(in mm) :  the threshold above which one the device raises an interrupt if Window = 1
    * @param   Window detection mode : 0=below, 1=above, 2=out, 3=in
    * @param   IntOnNoTarget = 0 (No longer used - just use 0)
    */
    VL53L1X_ERROR VL53L1X_SetDistanceThreshold(uint16_t ThreshLow,
                    uint16_t ThreshHigh, uint8_t Window,
                    uint8_t IntOnNoTarget);

    /**
    * @brief This function returns the window detection mode (0=below; 1=above; 2=out; 3=in)
    */
    VL53L1X_ERROR VL53L1X_GetDistanceThresholdWindow(uint16_t *window);

    /**
    * @brief This function returns the low threshold in mm
    */
    VL53L1X_ERROR VL53L1X_GetDistanceThresholdLow(uint16_t *low);

    /**
    * @brief This function returns the high threshold in mm
    */
    VL53L1X_ERROR VL53L1X_GetDistanceThresholdHigh(uint16_t *high);

    /**
    * @brief This function programs the ROI (Region of Interest)\n
    * The ROI position is centered, only the ROI size can be reprogrammed.\n
    * The smallest acceptable ROI size = 4\n
    * @param X:ROI Width; Y=ROI Height
    */
    VL53L1X_ERROR VL53L1X_SetROI(uint16_t X, uint16_t Y);

    /**
    *@brief This function returns width X and height Y
    */
    VL53L1X_ERROR VL53L1X_GetROI_XY(uint16_t *ROI_X, uint16_t *ROI_Y);

    /**
    *@brief This function programs the new user ROI center, please to be aware that there is no check in this function.
    *if the ROI center vs ROI size is out of border the ranging function return error #13
    */
    VL53L1X_ERROR VL53L1X_SetROICenter(uint8_t ROICenter);

    /**
    *@brief This function returns the current user ROI center
    */
    VL53L1X_ERROR VL53L1X_GetROICenter(uint8_t *ROICenter);

    /**
    * @brief This function programs a new signal threshold in kcps (default=1024 kcps\n
    */
    VL53L1X_ERROR VL53L1X_SetSignalThreshold(uint16_t signal);

    /**
    * @brief This function returns the current signal threshold in kcps
    */
    VL53L1X_ERROR VL53L1X_GetSignalThreshold(uint16_t *signal);

    /**
    * @brief This function programs a new sigma threshold in mm (default=15 mm)
    */
    VL53L1X_ERROR VL53L1X_SetSigmaThreshold(uint16_t sigma);

    /**
    * @brief This function returns the current sigma threshold in mm
    */
    VL53L1X_ERROR VL53L1X_GetSigmaThreshold(uint16_t *signal);

    /**
    * @brief This function performs the temperature calibration.
    * It is recommended to call this function any time the temperature might have changed by more than 8 deg C
    * without sensor ranging activity for an extended period.
    */
    VL53L1X_ERROR VL53L1X_StartTemperatureUpdate();

    /* VL53L1X CALIBRATION */
    
    /**
    * @brief This function performs the offset calibration.\n
    * The function returns the offset value found and programs the offset compensation into the device.
    * @param TargetDistInMm target distance in mm, ST recommended 100 mm
    * Target reflectance = grey17%
    * @return 0:success, !=0: failed
    * @return offset pointer contains the offset found in mm
    */
    VL53L1X_ERROR VL53L1X_CalibrateOffset(uint16_t TargetDistInMm, int16_t *offset);

    /**
    * @brief This function performs the xtalk calibration.\n
    * The function returns the xtalk value found and programs the xtalk compensation to the device
    * @param TargetDistInMm target distance in mm\n
    * The target distance : the distance where the sensor start to "under range"\n
    * due to the influence of the photons reflected back from the cover glass becoming strong\n
    * It's also called inflection point\n
    * Target reflectance = grey 17%
    * @return 0: success, !=0: failed
    * @return xtalk pointer contains the xtalk value found in cps (number of photons in count per second)
    */
    VL53L1X_ERROR VL53L1X_CalibrateXtalk(uint16_t TargetDistInMm, uint16_t *xtalk);

    /* VL53L1X I2C */

    /** @brief VL53L1_WriteMulti() definition.\n
    * To be implemented by the developer
    */
    VL53L1X_ERROR VL53L1_WriteMulti(uint16_t index, uint8_t *pdata, uint32_t count);
    /** @brief VL53L1_ReadMulti() definition.\n
    * To be implemented by the developer
    */
    VL53L1X_ERROR VL53L1_ReadMulti(uint16_t index, uint8_t *pdata, uint32_t count);
    /** @brief VL53L1_WrByte() definition.\n
    * To be implemented by the developer
    */
    VL53L1X_ERROR VL53L1_WrByte(uint16_t index, uint8_t data);
    /** @brief VL53L1_WrWord() definition.\n
    * To be implemented by the developer
    */
    VL53L1X_ERROR VL53L1_WrWord(uint16_t index, uint16_t data);
    /** @brief VL53L1_WrDWord() definition.\n
    * To be implemented by the developer
    */
    VL53L1X_ERROR VL53L1_WrDWord(uint16_t index, uint32_t data);
    /** @brief VL53L1_RdByte() definition.\n
    * To be implemented by the developer
    */
    VL53L1X_ERROR VL53L1_RdByte(uint16_t index, uint8_t *pdata);
    /** @brief VL53L1_RdWord() definition.\n
    * To be implemented by the developer
    */
    VL53L1X_ERROR VL53L1_RdWord(uint16_t index, uint16_t *pdata);
    /** @brief VL53L1_RdDWord() definition.\n
    * To be implemented by the developer
    */
    VL53L1X_ERROR VL53L1_RdDWord(uint16_t index, uint32_t *pdata);
    /** @brief VL53L1_WaitMs() definition.\n
    * To be implemented by the developer
    */
    //VL53L1X_ERROR VL53L1_WaitMs(int32_t wait_ms);

};

#endif