#ifndef COLOR_SENSOR_ADVANCED_H
#define COLOR_SENSOR_ADVANCED_H

#include "mbed.h"
#include "SafeI2C.h"
#include "ColorSensor.h"
#include <cstdint>

#define STALS_ALS_MAX_CHANNELS      6              /*!< Number of channels of the STALS */
#define UID_LEN					    16
#define VD6283_CHANNEL_NB			STALS_ALS_MAX_CHANNELS
#define VD6281_DEFAULT_I2C_ADDR		0x20
#define VD_AC_CHANNELS				(VD_ALL_CHANNELS & ~STALS_CHANNEL_6)

class VD6283 : protected SafeI2C, public ColorSensor
{
    public:
    /** 
    * @enum STALS_ErrCode_t
    * 
    * This enumeration is aimed at defining the different errors that can be returned by the STALS driver
    */
    typedef enum  {
        STALS_NO_ERROR = 0,                        /*!< No error */
        STALS_ERROR_INVALID_PARAMS,                /*!< Provided parameters are invalid*/
        STALS_ERROR_INIT,                          /*!< Error in the initialization of the VD621 device*/
        STALS_ERROR_TIME_OUT,                      /*!< A time out has expired before an operation was completed */
        STALS_ERROR_INVALID_DEVICE_ID,             /*!< The Provided device identifier is invalid*/
        STALS_ERROR_WRITE,                         /*!< The trial to write on the I2C bus has failed*/
        STALS_ERROR_READ,                          /*!< The trial to read from the I2C bus has failed*/
        STALS_ERROR_ALREADY_STARTED,               /*!< The device is already started */
        STALS_ERROR_NOT_STARTED,                   /*!< The device is not started */
        STALS_ERROR_NOT_SUPPORTED,                 /*!< The called function is not supported, likely because not yet implemented*/
        STALS_ERROR_FNCT_DEPRECATED,               /*!< The called function is deprecated*/
        STALS_ERROR_LAST_ERROR_CODE,
    } STALS_ErrCode_t;

    protected:

    /** 
    * @struct STALS_Als_t 
    * 
    * This structure is aimed at defining the parameters providing the event counts values of the last light integration, for the selected channels
    */
    struct STALS_Als_t {
        uint8_t Channels;                                /*!< Flag to be ORED by the driver client in order to understand what are the channels that provide a valid value. 0x3F means that all the channels are valid*/
        uint32_t CountValue[STALS_ALS_MAX_CHANNELS];     /*!< Array providing the event counts value for each of the selected channels. This is value after per device calibration.*/
        uint32_t CountValueRaw[STALS_ALS_MAX_CHANNELS];  /*!< Array providing the event counts value for each of the selected channels. This is value before per device calibration.*/
        uint16_t Gains[STALS_ALS_MAX_CHANNELS];          /*!< Array providing the gains used for measure. */
    };
    
    /** 
    * @enum STALS_Channel_Id_t
    * 
    * Constants listing the channels of the device.
    */
    typedef enum : uint8_t {
        STALS_CHANNEL_1             = 0x01,        /*!< channel 1*/
        STALS_CHANNEL_2             = 0x02,        /*!< channel 2*/
        STALS_CHANNEL_3             = 0x04,        /*!< channel 3*/
        STALS_CHANNEL_4             = 0x08,        /*!< channel 4*/
        STALS_CHANNEL_5             = 0x10,        /*!< channel 5*/
        STALS_CHANNEL_6             = 0x20,        /*!< channel 6*/
        STALS_CHANNEL_ALL           = 0x3F,        /*!< all channels*/
    } STALS_Channel_Id_t;

    /** 
    * @enum STALS_Control_t 
    * 
    * This enumeration is aimed at defining the Enable and disable flags
    */
    typedef enum
    {                                      
        STALS_CONTROL_DISABLE = 0,                  /*!< Disable the feature*/
        STALS_CONTROL_ENABLE = 1,                   /*!< Enable the feature*/
    } STALS_Control_t;
    
    /** 
    * @struct STALS_FlickerInfo_t
    * 
    * This structure contains the fields filled by the driver and that contains the discovered information related to the flicker 
    */
    struct STALS_FlickerInfo_t {
        uint32_t Frequency;                          /*!< Value of the frequency*/
        uint8_t  ConfidenceLevel;                    /*!< Confidence on the relevance of the measured value.*/
        uint8_t  IsMeasureFinish;                    /*!< Value is 1 if measure is finish*/
    };

    /** 
    * @enum STALS_Mode_t 
    * 
    * This enumeration is aimed at defining the different behavior modes of the STALS device
    */
    typedef enum  {
        /*!
        * Single shot mode. In this mode, the STALS records and accumulates the light for 1 single period. \n
        * The STALS driver client is responsible for starting the next integration period, by calling the STALS_Start function again 
        */    
        STALS_MODE_ALS_SINGLE_SHOT  = 0,

        /*!
        * Synchronous mode. This mode, is a continuous measurement mode meaning that this needs to be stopped by calling \ref STALS_Stop.
        * But an handshake is necessary to have the registers updated with the values of the on going measurement. This handshake is performed by 
        * the \ref STALS_GetAlsValues function\n
        * Two options are available for the handshake \n
        * 1. by I2C reading. in this case the AC data is available on the GPIO1 pin
        * 2. by interrupt acknowledgment. in this case the AC data is NOT available 
        */    
        STALS_MODE_ALS_SYNCHRONOUS  = 1,

        /*!
        * Flicker mode. This mode outputs PDM on the GPIO1 or GPIO2 pin.
        */    
        STALS_MODE_FLICKER          = 2
    } STALS_Mode_t;

    /** 
    * @enum STALS_Control_Id_t
    * 
    * This enumeration is aimed at defining the different parameters identifiers 
    */
    typedef enum STALS_Control_Id_t {
        /*!
        * Control to be used to enable or disable the pedestal\n
        * To enable the pedestal : \ref STALS_SetControl(pHandle, STALS_PEDESTAL_ENABLE, STALS_CONTROL_ENABLE);\n
        * To disable the pedestal : \ref STALS_SetControl(pHandle, STALS_PEDESTAL_ENABLE, STALS_CONTROL_DISABLE);\n
        * To know is the pedestal is enabled : \n
        * enum  STALS_Control_t Flag; \ref STALS_GetControl(pHandle, STALS_PEDESTAL_ENABLE, &Flag);
        */
        STALS_PEDESTAL_ENABLE       = 0,        

        /*!
        * Control to set the value of the pedestal\n
        * For example : \ref STALS_SetControl(pHandle, STALS_PEDESTAL_VALUE, 6);
        */
        STALS_PEDESTAL_VALUE        = 1,           

        /*!
        * Control to set if dark is output. For VD628x dark count will be output on channel 2.
        */
        STALS_OUTPUT_DARK_ENABLE    = 3,

        /*!
        * Control to set drive current in sda pad in mA when device is driving sda line.
        * Possible values for VD628x are 4, 8, 12, 16, 20 mA. 
        */
        STALS_SDA_DRIVE_VALUE_MA    = 4,

        /*!
        * Control to read saturation counter value.
        */
        STALS_SATURATION_VALUE      = 5,

        /*!
        * Control to configure als autogain feature\n
        * Control can be change only when als is stopped.\n
        * Set/Get ControlValue has the following layout\n
        *      [0] : enable bit (use STALS_CONTROL_DISABLE/STALS_CONTROL_ENABLE)\n
        *   [19:8] : ORED value of the \ref STALS_Channel_Id_t channels that will be under als autogain control\n
        *  [31:20] : master als autogain channel to use if any.\n
        *\n
        * ControlValue must follow below constraints else STALS_ERROR_INVALID_PARAMS will be return :
        *          - [19:8] must contain at least one channel.
        *          - [19:8] must not contain an invalid channel.
        *          - [19:8] channels in this field must be included in Channels parameter of at STALS_Start() call.
        *          - [31:20] must contain only one valid channel or no channel.
        *          - [31:20] channel in this field must be present in [19:8]
        */
        STALS_ALS_AUTOGAIN_CTRL     = 6,

        /*!
        * Control to set if OTP information is use by driver. It is enable by default. Unless you know what you are doing don't change this value\n
        * For example : \ref STALS_SetControl(pHandle, STALS_OTP_USAGE_ENABLE, STALS_CONTROL_DISABLE).
        */
        STALS_OTP_USAGE_ENABLE      = 128,

        /*!
        * Control to get/set workaround state:
        * - use STALS_SetControl() to enable/disable given workaround. Msb bit of
        *   ControlValue control if wa is enable or disable. Others bits are the wa
        *   number.
        * - use STALS_GetControl() to get wa status. pControlValue is then an inout
        *   parameter. you call it with the wa for which you want to know state. on
        *   exit you read msb bit to know wa status.
        */
        STALS_WA_STATE              = 129
    } STALS_Control_Id_t;

    /** 
    * @enum STALS_Mode_t 
    * 
    * This enumeration is aimed at defining the different behavior modes of the STALS device
    */
    typedef enum
    {
        STALS_FLICKER_OUTPUT_ANALOG = 0,            /*!< Analog. DEPRECATED, use STALS_FLICKER_OUTPUT_ANALOG_CFG_1 instead.*/
        STALS_FLICKER_OUTPUT_ANALOG_CFG_1 = 0,      /*!< Analog. PDM is output to GPIO1 through pad internal resistance.*/
        STALS_FLICKER_OUTPUT_DIGITAL_PDM = 1,       /*!< PDM Digital. This modes needs an external clock to feed the device through the corresponding pin.*/
        STALS_FLICKER_OUTPUT_ANALOG_CFG_2 = 2,      /*!< Analog. PDM is output to GPIO2.*/
        STALS_FLICKER_OUTPUT_ZC_CFG_1 = 3,          /*!< Zero crossing. Zero crossing is output to GPIO1. Supported by VD6283. */
        STALS_FLICKER_OUTPUT_ZC_CFG_2 = 4,          /*!< Zero crossing. Zero crossing is output to GPIO2. Supported by VD6283. */
    } STALS_FlickerOutputType_t;

    typedef enum {
        DEV_FREE = 0,
        DEV_INIT,
        DEV_ALS_RUN,
        DEV_FLICKER_RUN,
        DEV_BOTH_RUN
    } dev_state;
    
    struct VD6283_device {
        dev_state st;
        STALS_Mode_t als_started_mode;
        uint8_t dc_chan_en;
        uint8_t ac_chan_en;
        struct { uint8_t chan; } als;
        struct { uint8_t chan; } flk;
        uint16_t gains[VD6283_CHANNEL_NB];
        STALS_FlickerOutputType_t flicker_output_type;
        STALS_Control_t is_otp_usage_enable;
        STALS_Control_t is_output_dark_enable;
        uint32_t als_autogain_ctrl;
        uint64_t otp_bits[2];
        struct {
            uint16_t hf_trim;
            uint8_t lf_trim;
            uint8_t filter_config;
            uint8_t filter_index;
            uint8_t gains[STALS_ALS_MAX_CHANNELS];
            uint8_t otp_version;
        } otp;
        char uid[UID_LEN];
    };


    protected:
    
    //**************************************************************
    /**
    * This function Initializes the STALS driver 
    *
    * @note SlaveAddress. If set to the default address of the device, then the irq pin does not need to be set to low.\n
    * WARNING : to set a new I2C slave address, the GPIO1 pin MUST be set to low and the Init function will then perform an I2C transaction with this I2C slave address. This transaction, being the first one after the power up of the device,
    * will set this new slave address and all further I2C address shall be performed with this I2C slave address.
    * 
    * @note A call to the \ref STALS_Init function shall be done to set the VD628x device in IDLE mode.
    * @note The VD628x device needs a delay between powering it and calling \ref STALS_Init. Please refer to the device user manual for more details.
    * 
    * \retval  STALS_NO_ERROR                Success
    * \retval  STALS_ERROR_INIT              Could not initialize the driver
    * \retval  STALS_ERROR_INVALID_PARAMS    At least one of the provided parameters to the function is invalid
    * \retval  STALS_ERROR_WRITE             Could not write any data into the device through I2C
    */
    STALS_ErrCode_t STALS_Init();

    /**
    * This function terminates the provided STALS driver instance
    * 
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    */
    STALS_ErrCode_t STALS_Term();


    /**
    * This function sets the exposure time into the device, after having tuned to the closest value that the device can support.
    * Note that a fixed readout period of ~6 ms takes place just after exposure time, needed by the device to set the event count values in the registers
    * It also returns the actual applied value in the device
    *
    * For VD628x  possible values are multiples of 1.6 ms with a range of 1.6 ms to 1.6 s.
    *
    * @note Note Exposure time is irrelevant for flicker detection.
    * 
    * @param ExpoTimeInUs                     Exposure time in microseconds
    * @param pAppliedExpoTimeUs               Pointer on in which the value of the actual exposure time is returned
    * 
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    * \retval  STALS_ERROR_ALREADY_STARTED    Exposure can not be set when the device is running
    * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
    */
    STALS_ErrCode_t STALS_SetExposureTime(uint32_t ExpoTimeInUs, uint32_t *pAppliedExpoTimeUs);

    /**
    * This function returns the actual exposure time
    *
    * @param pAppliedExpoTimeUs               Pointer in which the value of the actual exposure time is returned
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    */
    STALS_ErrCode_t STALS_GetExposureTime(uint32_t *pAppliedExpoTimeUs);

    /**
    * This function sets an inter measurement time into the device, after having tuned to the closest value that the device can support.
    * It also returns the actual applied value in the device
    *
    * @param InterMeasurmentInUs              Inter measurement
    * @param pAppliedInterMeasurmentInUs      Pointer on in which the value of the actual inter measurement time is returned
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    * \retval  STALS_ERROR_ALREADY_STARTED    Inter measurement can not be set when the device is running
    * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
    */
    STALS_ErrCode_t STALS_SetInterMeasurementTime(uint32_t InterMeasurmentInUs, uint32_t *pAppliedInterMeasurmentInUs);

    /**
    * This function returns the actual inter measurement
    *
    * @param pAppliedInterMeasurmentInUs      Pointer on in which the value of the actual inter measurement time is returned
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    */
    STALS_ErrCode_t STALS_GetInterMeasurementTime(uint32_t *pAppliedInterMeasurmentInUs);

    /**
    * This function sets an analog gain on the provided channel id.
    *
    * @note The gain impacts the signal output amplitude, but not the values returned by STALS_GetFlicker, 
    * unless the gain is exceeds a maximum value that will flatten the signal output because of its impact on saturation
    *
    * @param ChannelId                        this id identifies the channel number. See \ref STALS_Channel_Id_t 
    * @param Gain                             Gain in 8.8 fixed point unit
    * @param pAppliedGain                     Pointer in which the value of the actual gain applied in the device is returned. Value in 8.8 fixed point unit
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    * \retval  STALS_ERROR_ALREADY_STARTED    Gain can not be set when the device is running
    * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
    * \retval  STALS_ERROR_READ               Could not read any data from the device through I2C
    */
    STALS_ErrCode_t STALS_SetGain(STALS_Channel_Id_t ChannelId, uint16_t Gain, uint16_t *pAppliedGain);

    /**
    * This function gets the actual gain applied in the device
    *
    * @param ChannelId                        this Id identifies the channel number. see \ref STALS_Channel_Id_t 
    * @param pAppliedGain                     Pointer in which the value of the actual gain applied in the device is returned. Value in 8.8 fixed point unit
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    */
    STALS_ErrCode_t STALS_GetGain(STALS_Channel_Id_t ChannelId, uint16_t *pAppliedGain);

    /**
    * This function set GPIO2 and GPIO1 gpio's settings according to FlickerOutputType. Note that those parameters are
    * effectively applied when flicker is started.
    *
    * @param FlickerOutputType                Data output type for the flicker mode. see \ref STALS_FlickerOutputType_t
    * 
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
    * \retval  STALS_ERROR_READ               Could not read any data from the device through I2C
    */
    STALS_ErrCode_t STALS_SetFlickerOutputType(STALS_FlickerOutputType_t FlickerOutputType);
    
    /**
    * This function starts the device
    *
    * @param Mode                             Mode. shall be \ref STALS_MODE_ALS_SINGLE_SHOT, \ref STALS_MODE_ALS_SYNCHRONOUS or \ref STALS_MODE_FLICKER\n
    * @param Channels.                        For the ALS modes, this is an ORED value of the \ref STALS_Channel_Id_t channels.
    *                                         For the flicker mode, this is one of the \ref STALS_Channel_Id_t channels.\n 
    *
    * @note As the ALS and FLICKER modes can run independently, Two consecutive calls to this START function with \ref STALS_MODE_ALS_SYNCHRONOUS and \ref STALS_MODE_FLICKER modes are permitted.
    * 
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    * \retval  STALS_ALREADY_STARTED          The device is already running in the provided mode or in an incompatible mode
    * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
    */
    STALS_ErrCode_t STALS_Start(STALS_Mode_t Mode, STALS_Channel_Id_t Channels);

    /**
    * This function stops the device
    *
    * @param Mode                             Mode. shall be \ref STALS_MODE_ALS_SINGLE_SHOT, \ref STALS_MODE_ALS_SYNCHRONOUS or \ref STALS_MODE_FLICKER.
    *
    * @note note
    * 
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    * \retval  STALS_ALREADY_NOT_STARTED      The device is not running the provided mode
    * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
    */
    STALS_ErrCode_t STALS_Stop(STALS_Mode_t Mode);

    /**
    * This function that provides the event counts values for the selected channels
    *
    * @param Channels                         an ORED value of the \ref STALS_Channel_Id_t that permits to select the channels from which the event counts values are to be retrieved.
    * @param pAlsValue                        Pointer on a structure storing the counted events values
    * @param pMeasureValid                    Pointer on a flag telling if the measurement is valid. 
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    * \retval  STALS_ERROR_WRITE              Could not write any data into the device through I2C
    * \retval  STALS_ERROR_READ               Could not read any data from the device through I2C
    */
    STALS_ErrCode_t STALS_GetAlsValues(STALS_Channel_Id_t Channels, struct STALS_Als_t * pAlsValue, uint8_t * pMeasureValid);


    /**
    * This function sets a control to the STALS driver
    *
    * @param ParamId                          Identifier of the param provided
    * @param ControlValue                     The value of the control 
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    */
    STALS_ErrCode_t STALS_SetControl(STALS_Control_Id_t ControlId, uint32_t ControlValue);

    /**
    * This function gets a control from the STALS driver
    *
    * @param ParamId                          Identifier of the param provided
    * @param pControlValue                    Pointer on a parameter in which the parameter value is set
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    */
    STALS_ErrCode_t STALS_GetControl(STALS_Control_Id_t ControlId, uint32_t * pControlValue);

    /**
    * This function return device unique id. Unique id is a null terminated string.
    *
    * @param Uid                              Pointer on a char pointer in which address of uid string will be set.
    *
    * \retval  STALS_NO_ERROR                 Success
    * \retval  STALS_ERROR_INVALID_PARAMS     At least one of the provided parameters to the function is invalid
    */
    STALS_ErrCode_t STALS_GetUid(char ** pUid);

    //*********************************
    
    VD6283_device handle_dev;
	STALS_Als_t meas;

    public:
    VD6283() = delete;
    VD6283(PinName sda, PinName scl, const uint8_t & address = 0x20, const int & frequency = 1000000);
    ~VD6283();

    int init() override;

    STALS_ErrCode_t getExposure(uint32_t & exposure);
    STALS_ErrCode_t setExposure(const uint32_t & exposure);
    
    STALS_ErrCode_t getGainChannel(STALS_Channel_Id_t channel, uint16_t & gain);
    STALS_ErrCode_t setGainChannel(STALS_Channel_Id_t channel, const uint16_t & next_gain);

    STALS_ErrCode_t setDarkOutput(bool enabled);

    STALS_ErrCode_t read();

    int read(const Filter & filter, Result & out, bool raw = false) override;

    inline uint32_t getIR() { return getColor(4); }
    inline uint32_t getRed() { return getColor(0); }    
    inline uint32_t getBlue() { return getColor(2); }
    inline uint32_t getGreen() { return getColor(3); }
    inline uint32_t getClear() { return getColor(5); }
    inline uint32_t getVisible() { return getColor(1); }

    inline uint32_t getRawIR() { return getRawColor(4); }
    inline uint32_t getRawRed() { return getRawColor(0); }    
    inline uint32_t getRawBlue() { return getRawColor(2); }
    inline uint32_t getRawGreen() { return getRawColor(3); }
    inline uint32_t getRawClear() { return getRawColor(5); }
    inline uint32_t getRawVisible() { return getRawColor(1); }    

    private:
    STALS_ErrCode_t STALS_WrByte(uint8_t index, uint8_t data);
    STALS_ErrCode_t STALS_RdByte(uint8_t index, uint8_t *data);
    STALS_ErrCode_t STALS_RdMultipleBytes(uint8_t index, uint8_t *data, int nb);

    inline uint32_t getColor(const uint8_t & channel)
    {
        return meas.CountValue[channel] >> 8;
    }
    inline uint32_t getRawColor(const uint8_t & channel)
    {
        return meas.CountValueRaw[channel] >> 8;
    }

    //int is_cut_valid();
    int fixup_otp_filter_config();
    int channelId_2_index(STALS_Channel_Id_t ChannelId);

    void cook_values(struct STALS_Als_t *pAlsValue);
    void cook_values_slope(struct STALS_Als_t *pAlsValue);
    void put_device();

    uint8_t dev_disable_dc_chan_en_for_mode(STALS_Mode_t mode);

    STALS_ErrCode_t acknowledge_irq();
    STALS_ErrCode_t ac_mode_update(uint8_t mask, uint8_t data);
    STALS_ErrCode_t clamp_enable();
    STALS_ErrCode_t check_supported_device();
    STALS_ErrCode_t dev_configuration();
    STALS_ErrCode_t dev_disable_channels_for_mode(STALS_Mode_t mode);
    STALS_ErrCode_t dev_enable_channels_for_mode(STALS_Mode_t mode, uint8_t channels);
    STALS_ErrCode_t dev_sw_reset();
    STALS_ErrCode_t disable_flicker_output_mode();
    STALS_ErrCode_t enable_flicker_output_mode();
    STALS_ErrCode_t get_channel_gain(int c, uint16_t *pAppliedGain);
    STALS_ErrCode_t get_als_autogain_ctrl(uint32_t *value);
    STALS_ErrCode_t get_exposure(uint32_t *pAppliedExpoTimeUs);
    STALS_ErrCode_t get_output_dark_enable(uint32_t *value);
    STALS_ErrCode_t get_otp_usage_enable(uint32_t *value);
    STALS_ErrCode_t get_pedestal_enable(uint32_t *value);
    STALS_ErrCode_t get_pedestal_value(uint32_t *value);
    STALS_ErrCode_t get_saturation_value(uint32_t *value);
    STALS_ErrCode_t get_sda_drive_value(uint32_t *value);
    STALS_ErrCode_t is_data_ready(uint8_t *is_data_ready);
    STALS_ErrCode_t otp_generate_uid();
    STALS_ErrCode_t otp_read(int bit_start, int bit_nb, uint32_t *otp, int bit_swap);
    STALS_ErrCode_t otp_read_bank(int bank, int bit_start, int bit_end, uint32_t *opt);
    STALS_ErrCode_t opt_read_init();
    STALS_ErrCode_t otp_read_param_details();
    STALS_ErrCode_t otp_read_param();
    STALS_ErrCode_t otp_reset();
    STALS_ErrCode_t read_channel(int chan, uint32_t *measure);
    STALS_ErrCode_t select_pd();
    STALS_ErrCode_t set_als_autogain_ctrl(uint32_t value);
    STALS_ErrCode_t set_channel_gain(int c,	uint16_t Gain);
    STALS_ErrCode_t set_dithering();
    STALS_ErrCode_t set_exposure(uint32_t ExpoTimeInUs/*, int is_cache_updated*/);
    STALS_ErrCode_t set_otp_usage_enable(uint32_t value);
    STALS_ErrCode_t set_output_dark_enable(uint32_t value);
    STALS_ErrCode_t set_pedestal_enable(uint32_t value);
    STALS_ErrCode_t set_pedestal_value(uint32_t value);
    STALS_ErrCode_t set_sda_drive_value(uint32_t value);
    STALS_ErrCode_t start_als(uint8_t channels, STALS_Mode_t mode);
    STALS_ErrCode_t start_flicker(uint8_t channels);
    STALS_ErrCode_t start_single_shot(uint8_t channels);
    STALS_ErrCode_t start_synchronous(uint8_t channels);
    STALS_ErrCode_t stop_als(STALS_Mode_t mode);
    STALS_ErrCode_t stop_flicker();
    STALS_ErrCode_t stop_single_shot();
    STALS_ErrCode_t stop_synchronous();
    STALS_ErrCode_t trim_oscillators();
};

#endif