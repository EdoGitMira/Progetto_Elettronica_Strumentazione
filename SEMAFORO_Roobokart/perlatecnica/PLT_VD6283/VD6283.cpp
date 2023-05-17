#include "VD6283.h"
#include <cstdint>


//**********************************************************

// my define

#define VD6283_EXPOSURE_STEP_SIZE_US 1600

//**********************************************************

#ifndef BIT
#define BIT(b)							(1UL << (b))
#endif

#define VD6283_DEVICE				        0x70
#define VD6283_REVISION		                0xBD

#define VD6283_DEVICE_ID					0x00
#define VD6283_REVISION_ID					0x01
#define VD6283_IRQ_CTRL_ST					0x02
#define VD6283_ALS_CTRL						0x03
#define VD6283_ALS_START					BIT(0)
#define VD6283_ALS_CONTINUOUS				BIT(1)
#define VD6283_ALS_CONTINUOUS_SLAVED		BIT(2)
#define VD6283_CONTINUOUS_PERIOD			0x04

#define VD6283_CHANNELx_MM(c)				(0x06 + 4 * (c))
#define VD6283_CHANNELx_LM(c)				(0x07 + 4 * (c))
#define VD6283_CHANNELx_LL(c)				(0x08 + 4 * (c))

#define VD6283_EXPOSURE_M					0x1d
#define VD6283_EXPOSURE_L					0x1e
#define VD6283_CHANNEL_VREF(chan)			(0x25 + (chan))

#define VD6283_AC_EN						0x2D
#define VD6283_DC_EN						0x2E
#define VD6283_AC_CLAMP_EN					0x2F
#define VD6283_DC_CLAMP_EN					0x30
#define VD6283_AC_MODE						0x31
#define AC_EXTRACTOR						BIT(0)
#define AC_EXTRACTOR_ENABLE					BIT(0)
#define AC_EXTRACTOR_DISABLE				0
#define AC_CHANNEL_SELECT					0x0e
#define PDM_SELECT_OUTPUT					BIT(4)
#define PDM_SELECT_GPIO1					0
#define PDM_SELECT_GPIO2					BIT(4)
#define PDM_SELECT_CLK						BIT(5)
#define PDM_SELECT_INTERNAL_CLK				0
#define PDM_SELECT_EXTERNAL_CLK				BIT(5)
#define AC_PEDESTAL						    BIT(6)
#define AC_PEDESTAL_ENABLE					0
#define AC_PEDESTAL_DISABLE					BIT(6)
#define VD6283_AC_PEDESTAL					0x32
#define VD6283_PEDESTAL_VALUE_MASK			0x07
#define VD6283_AC_SAT_METRIC_M				0x33
#define VD6283_AC_SAT_METRIC_L				0x34
#define VD6283_AC_ACC_PERIODS_M				0x35
#define VD6283_AC_ACC_PERIODS_L				0x36
#define VD6283_AC_NB_PERIODS				0x37
#define VD6283_AC_AMPLITUDE_M				0x38
#define VD6283_AC_AMPLITUDE_L				0x39

#define VD6283_SDA_DRIVE					0x3c
#define VD6283_SDA_DRIVE_MASK				0x07
#define VD6283_OSC10M						0x3d
#define VD6283_OSC10M_TRIM_M				0x3e
#define VD6283_OSC10M_TRIM_L				0x3f
#define VD6283_OSC50K_TRIM					0x40

#define VD6283_INTR_CFG						0x41
#define VD6283_DTEST_SELECTION				0x47

#define VD6283_SEL_PD_x(c)					(0x6B + (c))

#define VD6283_OTP_CTRL1					0x58

#define VD6283_OTP_STATUS					0x5a
#define VD6283_OTP1_DATA_READY				BIT(1)
#define VD6283_OTP2_DATA_READY				BIT(3)
#define VD6283_OTP_DATA_READY				(VD6283_OTP1_DATA_READY | VD6283_OTP2_DATA_READY)

#define VD6283_OTP_BANK_0					0x5b
#define VD6283_OTP_BANK_1					0x63

#define VD6283_SPARE_0						0x71
#define VD6283_SPARE_0_OUT_GPIO2			0
#define VD6283_SPARE_0_IN_GPIO2				BIT(0)
#define VD6283_SPARE_0_ZC_GPIO2				BIT(1)
#define VD6283_SPARE_0_ZC_GPIO1				BIT(2)
#define VD6283_SPARE_1						0x72

#define VD6283_OSC_DITHERING_CTRL			0x76
#define VD6283_OSC_DITHERING_STEP_VALUE		0x77
#define VD6283_OSC_DITHERING_STEP_DURATION	0x78
#define VD6283_OSC_DITHERING_STEP_NUMBER	0x79

#define VD6283_GLOBAL_RESET					0xFE

//**********************************************************

#define POPCOUNT(a)				            __builtin_popcount(a)
#define CTZ(a)					            __builtin_ctz(a)
#define div64_u64(a, b)				        ((a) / (b))

//*********************************************************

#define ARRAY_SIZE(a)				        (sizeof(a) / sizeof(a[0]))

#define VD6283_DC_CHANNELS_MASK			    0x1f
#define VD6283_AC_CHANNELS_MASK			    0x20

#define VD6283_DEFAULT_HF_TRIM			    0x0e3
#define VD6283_DEFAULT_LF_TRIM			    0x07
#define VD6283_DEFAULT_GAIN			        0x80
#define VD6283_DEFAULT_FILTER_CONFIG	    2

#define VD6283_DEFAULT_OTP_VERSION	        0x15

#define MAX(a, b)				            ((a) > (b) ? (a) : (b))
#define MIN(a, b)				            ((a) < (b) ? (a) : (b))

#define VD6283_DEFAULT_FILTER_INDEX		    2

//**********************************************************

/* als autogain helpers */
#define ALS_AUTO_CHANNELS_MASK			    (0x00000fff)
#define ALS_AUTO_VALUE_2_CHANNELS(v)		(((v) >>  8) & ALS_AUTO_CHANNELS_MASK)
#define ALS_AUTO_VALUE_2_MASTER_CHANNEL(v)	(((v) >> 20) & ALS_AUTO_CHANNELS_MASK)
#define ALS_AUTO_ALLOWED_BITS			    0xffffff01

//**********************************************************

#define CHECK_NULL_PTR(a) \
do { \
	if (!a) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECK_CHANNEL_VALID(idx) \
do { \
	if ((idx) >= VD6283_CHANNEL_NB) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECK_SINGLE_CHANNEL(scn) \
do { \
	if (POPCOUNT(scn) != 1) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECH_DEV_ST_INIT(d) \
do { \
	if (d->st != DEV_INIT) \
		return STALS_ERROR_ALREADY_STARTED; \
} while (0)

#define CHECK_DEV_ST_FLICKER_STARTED(d) \
do { \
	if (d->st != DEV_FLICKER_RUN && d->st != DEV_BOTH_RUN) \
		return STALS_ERROR_NOT_STARTED; \
} while (0)

#define CHECH_DEV_ST_FLICKER_NOT_STARTED(d) \
do { \
	if (d->st == DEV_FLICKER_RUN) \
		return STALS_ERROR_ALREADY_STARTED; \
	if (d->st == DEV_BOTH_RUN) \
		return STALS_ERROR_ALREADY_STARTED; \
} while (0)

#define CHECK_DEV_ST_ALS_STARTED(d) \
do { \
	if (d->st != DEV_ALS_RUN && d->st != DEV_BOTH_RUN) \
		return STALS_ERROR_NOT_STARTED; \
} while (0)

#define CHECH_DEV_ST_ALS_NOT_STARTED(d) \
do { \
	if (d->st == DEV_ALS_RUN) \
		return STALS_ERROR_ALREADY_STARTED; \
	if (d->st == DEV_BOTH_RUN) \
		return STALS_ERROR_ALREADY_STARTED; \
} while (0)

#define CHECK_CHANNEL_MASK_VALID_FOR_ALS(m) \
do { \
	/* check there is no invalid channel selected */  \
	if ((m) & ~((1 << STALS_ALS_MAX_CHANNELS) - 1)) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECK_CHANNEL_MASK_VALID(m) \
do { \
	/* check there is no invalid channel selected */  \
	if ((m) & ~((1 << STALS_ALS_MAX_CHANNELS) - 1)) \
		return STALS_ERROR_INVALID_PARAMS; \
	/* check there is at least one channel */ \
	if (!(m)) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

#define CHECK_CHANNEL_NOT_IN_USE(d, c) \
do { \
	if (d->als.chan & (c)) \
		return STALS_ERROR_INVALID_PARAMS; \
	if (d->flk.chan & (c)) \
		return STALS_ERROR_INVALID_PARAMS; \
} while (0)

//**********************************************************

static const uint16_t GainRange[] = {
	0x42AB,		/*   66, 67 */
	0x3200,		/*   50, 00 */
	0x2154,		/*   33, 33 */
	0x1900,		/*   25, 00 */
	0x10AB,		/*   16, 67 */
	0x0A00,		/*   10, 00 */
	0x0723,		/*    7, 14 */
	0x0500,		/*    5, 00 */
	0x0354,		/*    3, 33 */
	0x0280,		/*    2, 50 */
	0x01AB,		/*    1, 67 */
	0x0140,		/*    1, 25 */
	0x0100,		/*    1, 00 */
	0x00D4,		/*    0, 83 */
	0x00B5		/*    0, 71 */
};

static const uint16_t GainRangeThreshold[] = {
	0x3A56,
	0x29AB,
	0x1D2B,
	0x14D6,
	0x0D56,
	0x0892,
	0x0612,
	0x042B,
	0x02EB,
	0x0216,
	0x0176,
	0x0121,
	0x00EB,
	0x00C5
};

//***********************************************

VD6283::VD6283(PinName sda, PinName scl, const uint8_t & address, const int & frequency)
: SafeI2C(sda, scl, address << 1, frequency)
{
    memset(&handle_dev, 0, sizeof(handle_dev));
	//handle_dev.client = i2c_dev;
    //handle_dev.dev_7bits_i2c_addr = VD6281_DEFAULT_I2C_ADDR;	
	handle_dev.st = DEV_INIT;
	handle_dev.is_otp_usage_enable = STALS_CONTROL_ENABLE;
	handle_dev.is_output_dark_enable = STALS_CONTROL_DISABLE;
	//handle_dev.exposure = 80000;
}

VD6283::~VD6283()
{
    STALS_Term();
}

int VD6283::init()
{
    STALS_ErrCode_t error;
    uint32_t current_exposure;
    uint16_t current_gain;
    
    error = STALS_Init();
    if (error) return error;

    error = STALS_SetExposureTime(100000, &current_exposure);
    //printf("Exposure set to %u us\n", current_exposure);
    if (error) return error;

    for(int c = 0; c < STALS_ALS_MAX_CHANNELS; c++) {
        error = STALS_SetGain((STALS_Channel_Id_t)(1 << c), 0x0100, &current_gain);
        //printf(" Channel%d gain set to 0x%04x\n", c + 1, current_gain);
        if (error) return error;
    }

    

    return STALS_ErrCode_t::STALS_NO_ERROR;
} 

VD6283::STALS_ErrCode_t VD6283::getExposure(uint32_t & exposure)
{  
    return STALS_GetExposureTime(&exposure);
}

VD6283::STALS_ErrCode_t VD6283::setExposure(const uint32_t & exposure)
{
    uint32_t current_exposure;
    return STALS_SetExposureTime(exposure, &current_exposure);
}

VD6283::STALS_ErrCode_t VD6283::getGainChannel(STALS_Channel_Id_t channel, uint16_t & gain)
{
    return STALS_GetGain(channel, &gain);   
}

VD6283::STALS_ErrCode_t VD6283::setGainChannel(STALS_Channel_Id_t channel, const uint16_t & next_gain)
{
    uint16_t gain;
    return STALS_SetGain(channel, next_gain, &gain);
}

VD6283::STALS_ErrCode_t VD6283::setDarkOutput(bool enabled)
{
    uint32_t value;
    STALS_ErrCode_t err;
    err = STALS_GetControl(STALS_OUTPUT_DARK_ENABLE, &value);
    if (err) return err;
    return STALS_SetControl(STALS_OUTPUT_DARK_ENABLE, 1 - value);   
}

int VD6283::read(const Filter & filter, Result & out, bool raw)
{
    int error = 0;

    resetResult(out);

    raw = false;

    error = read();
    if (error) return error;

    if (filter & Filter::Red)
    {
        if (raw) out.red = getRawRed();
        else out.red = getRed();
    }

    if (filter & Filter::Green)
    {
        if (raw) out.green = getRawGreen();
        else out.green = getGreen();
    }

    if (filter & Filter::Blue)
    {
        if (raw) out.blue = getRawBlue();
        else out.blue = getBlue();
    }

    if (filter & Filter::Clear)
    {
        if (raw) out.clear = getRawClear();
        else out.clear = getClear();
    }

    if (filter & Filter::Visible)
    {
        if (raw) out.visible = getRawClear();
        else out.visible = getClear();
    }

    if (filter & Filter::IR)
    {
        if (raw) out.ir = getRawIR();
        else out.ir = getIR();
    }

    if (filter & Filter::UV )
    {
        error = 1;
    }

    return error;
}

//************************************************************************


VD6283::STALS_ErrCode_t VD6283::STALS_WrByte(uint8_t index, uint8_t data)
{    
    int res;
    res = SafeI2C::writeByte8(index, data);
    if (res) return VD6283::STALS_ERROR_WRITE;
    return VD6283::STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::STALS_RdByte(uint8_t index, uint8_t *data)
{
    int res;
    res = SafeI2C::readByte8(index, *data);
    if (res) return VD6283::STALS_ERROR_READ;
    return VD6283::STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::STALS_RdMultipleBytes(uint8_t index, uint8_t *data, int nb)
{
    int res;    
    res = SafeI2C::readMulti8(index, data, nb);
    if (res) return VD6283::STALS_ERROR_READ;
    return VD6283::STALS_NO_ERROR;
}

//************************************************************************

int VD6283::channelId_2_index(STALS_Channel_Id_t ChannelId)
{
	return POPCOUNT(ChannelId) == 1 ? CTZ(ChannelId) : STALS_ALS_MAX_CHANNELS;
}

VD6283::STALS_ErrCode_t VD6283::otp_read_bank(int bank, int bit_start, int bit_end, uint32_t *opt)
{
	int bit_nb = bit_end - bit_start + 1;
	uint32_t bit_mask = (1 << bit_nb) - 1;

	*opt = (handle_dev.otp_bits[bank] >> bit_start) & bit_mask;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::otp_read(int bit_start, int bit_nb, uint32_t *otp, int bit_swap)
{
	STALS_ErrCode_t res;
	const int bit_end = bit_start + bit_nb - 1;
	uint32_t bank0 = 0;
	uint32_t bank1 = 0;
	uint32_t result;
	uint32_t result_swap = 0;
	int bank1_shift = 0;
	int i;

	if (bit_nb > 24 || bit_end >= 120) {
			return STALS_ERROR_INVALID_PARAMS;
	}

	if (bit_start < 64) {
		res = otp_read_bank(0, bit_start, MIN(63, bit_end), &bank0);
		if (res)
			return res;
		bank1_shift = MIN(63, bit_end) - bit_start + 1;
	}
	if (bit_end >= 64) {
		res = otp_read_bank(1, MAX(0, bit_start - 64), bit_end - 64, &bank1);
		if (res)
			return res;
	}

	result = bank0 | (bank1 << bank1_shift);
	if (bit_swap) {
		result_swap = 0;
		for (i = 0; i < bit_nb; i++) {
			if ((result >> i) & 1)
				result_swap |= 1 << (bit_nb - 1 - i);
		}
	}
	*otp = bit_swap ? result_swap : result;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::check_supported_device()
{
	STALS_ErrCode_t res;
    uint8_t device_id;
    uint8_t revision_id;

	res = STALS_RdByte(VD6283_DEVICE_ID, &(device_id));
	if (res) return res;
	
    res = STALS_RdByte(VD6283_REVISION_ID, &(revision_id));
	if (res) return res;

	if ((device_id == VD6283_DEVICE) && (revision_id == VD6283_REVISION)) return STALS_NO_ERROR;

	return STALS_ERROR_INIT;
}

VD6283::STALS_ErrCode_t VD6283::is_data_ready(uint8_t *is_data_ready)
{
	STALS_ErrCode_t res;
	uint8_t isr_ctrl_st;

	res = STALS_RdByte(VD6283_IRQ_CTRL_ST, &isr_ctrl_st);
	*is_data_ready = !(isr_ctrl_st & 0x02);

	return res;
}

VD6283::STALS_ErrCode_t VD6283::ac_mode_update(uint8_t mask, uint8_t data)
{
	STALS_ErrCode_t res;
	uint8_t ac_mode;

	res = STALS_RdByte(VD6283_AC_MODE, &ac_mode);
	if (res)
		return res;
	ac_mode = (ac_mode & ~mask) | (data & mask);
	return STALS_WrByte(VD6283_AC_MODE, ac_mode);
}

VD6283::STALS_ErrCode_t VD6283::read_channel(int chan, uint32_t *measure)
{
	STALS_ErrCode_t res;
	uint8_t values[3];

	res = STALS_RdMultipleBytes(VD6283_CHANNELx_MM(chan),	values, 3);
	if (res)
		return res;

	*measure = (values[0] << 16) + (values[1] << 8) + values[2];

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::acknowledge_irq()
{
	STALS_ErrCode_t res;

	res = STALS_WrByte(VD6283_IRQ_CTRL_ST, 1);
	if (res)
		return res;
	res = STALS_WrByte(VD6283_IRQ_CTRL_ST, 0);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::dev_sw_reset()
{
	STALS_ErrCode_t res;

	res = STALS_WrByte(VD6283_GLOBAL_RESET, 1);
	if (res)
		return res;
	res = STALS_WrByte(VD6283_GLOBAL_RESET, 0);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::otp_reset()
{
	STALS_ErrCode_t res;
	uint8_t status;
	int max_loop_nb = 100;

	res = STALS_WrByte(VD6283_OTP_CTRL1, 0);
	if (res)
		return res;

	do {
		res = STALS_RdByte(VD6283_OTP_STATUS, &status);
		if (res)
			return res;
	} while ((status & VD6283_OTP_DATA_READY) !=
		 VD6283_OTP_DATA_READY && --max_loop_nb);
	if (!max_loop_nb)
		return STALS_ERROR_TIME_OUT;

	return STALS_NO_ERROR;
}

static uint8_t byte_bit_reversal(uint8_t d)
{
	d = ((d & 0xaa) >> 1) | ((d & 0x55) << 1);
	d = ((d & 0xcc) >> 2) | ((d & 0x33) << 2);
	d = ((d & 0xf0) >> 4) | ((d & 0x0f) << 4);
	return d;
}

VD6283::STALS_ErrCode_t VD6283::opt_read_init()
{
	STALS_ErrCode_t res;
	uint8_t reg[8];
	int i;

	handle_dev.otp_bits[0] = 0;

	res = STALS_RdMultipleBytes(VD6283_OTP_BANK_0, reg, 8);
	if (res)
		return res;

	for (i = 0; i < 7; i++) {
		handle_dev.otp_bits[0] |=
			(uint64_t) byte_bit_reversal(reg[i]) << (i * 8);
	}
	reg[7] = byte_bit_reversal(reg[7]) >> 4;
	handle_dev.otp_bits[0] |=  (uint64_t) reg[7] << 56;

	res = STALS_RdMultipleBytes(VD6283_OTP_BANK_1, reg, 8);
	if (res)
		return res;

	reg[0] = byte_bit_reversal(reg[0]);
	handle_dev.otp_bits[0] |=  (uint64_t) (reg[0] & 0x0f) << 60;
	handle_dev.otp_bits[1] = reg[0] >> 4;

	for (i = 1; i < 7; i++) {
		handle_dev.otp_bits[1] |=
			(uint64_t) byte_bit_reversal(reg[i]) << (i * 8 - 4);
	}
	reg[7] = byte_bit_reversal(reg[7]) >> 4;
	handle_dev.otp_bits[1] |=  (uint64_t) reg[7] << 52;

	return STALS_NO_ERROR;
}

int VD6283::fixup_otp_filter_config()
{
	uint32_t check[3];
	int res;

	res = otp_read(80, 16, &check[0], 0);
	if (res)
		return 0;
	res = otp_read(96, 14, &check[1], 0);
	if (res)
		return 0;
	res = otp_read(75, 5,  &check[2], 1);
	if (res)
		return 0;

	if (check[0] != 0x56c1)
		return 0;
	if (check[1] != 0x0364)
		return 0;

	if (check[2] < 1 || check[2] > 6)
		return 0;

	return 1;
}

VD6283::STALS_ErrCode_t VD6283::otp_read_param_details()
{
	STALS_ErrCode_t res;
	uint32_t otp_data;
	int i;

	res = otp_read(51, 9, &otp_data, 1);
	if (res)
		goto error;
	handle_dev.otp.hf_trim = otp_data;
	res = otp_read(116, 4, &otp_data, 1);
	if (res)
		goto error;
	handle_dev.otp.lf_trim = otp_data;
	res = otp_read(48, 3, &otp_data, 1);
	if (res)
		goto error;
	handle_dev.otp.filter_config = otp_data;
	for (i = 0; i < STALS_ALS_MAX_CHANNELS; i++) {
		res = otp_read(i * 8, 8, &otp_data, 1);
		if (res)
			goto error;
		/* 0 is not a valid value */
		handle_dev.otp.gains[i] = otp_data ? otp_data : VD6283_DEFAULT_GAIN;
	}

	if (fixup_otp_filter_config())
		handle_dev.otp.filter_config = 0x03;

	return STALS_NO_ERROR;

error:
	return res;
}

VD6283::STALS_ErrCode_t VD6283::otp_read_param()
{
	STALS_ErrCode_t res;
	uint32_t otp_data;
	int i;

	/* default values in case we don't recognize otp version */
	handle_dev.otp.hf_trim = VD6283_DEFAULT_HF_TRIM;
	handle_dev.otp.lf_trim = VD6283_DEFAULT_LF_TRIM;
	handle_dev.otp.filter_config = VD6283_DEFAULT_FILTER_CONFIG;
	handle_dev.otp.filter_index = VD6283_DEFAULT_FILTER_INDEX;
	handle_dev.otp.otp_version = VD6283_DEFAULT_OTP_VERSION;
	for (i = 0; i < STALS_ALS_MAX_CHANNELS; i++)
		handle_dev.otp.gains[i] = VD6283_DEFAULT_GAIN;

	/* read otp version */
	res = otp_read(113, 3, &otp_data, 1);
	if (res)
		goto error;
	/* For Rainbow2 extend OTP Version */
	otp_data = otp_data+0x10;
	handle_dev.otp.otp_version = otp_data;

	res = otp_read_param_details();
	handle_dev.otp.filter_index = VD6283_DEFAULT_FILTER_INDEX;

error:
	return res;
}

static char nible_to_ascii(uint8_t nible)
{
	return nible < 10 ? '0' + nible : 'a' + nible - 10;
}

static char encode_nible(uint8_t nible, uint8_t *xor_reg)
{
	uint8_t prev_xor_reg = *xor_reg;

	*xor_reg = nible ^ *xor_reg;

	return nible_to_ascii(nible ^ prev_xor_reg);
}

VD6283::STALS_ErrCode_t VD6283::otp_generate_uid()
{
	STALS_ErrCode_t res;
	uint32_t otp_data;
	uint8_t xor_reg = 0;
	uint8_t nible;
	int index = 0;
	int i;

	/* uid will have 14 chars */
	/* encode first nible with otp version first */
	res = otp_read(113, 3, &otp_data, 1);
	nible = otp_data << 1;
	res = (STALS_ErrCode_t) (res | otp_read(60, 1, &otp_data, 1));
	if (res)
		goto error;
	nible |= otp_data;
	handle_dev.uid[index++] = encode_nible(nible, &xor_reg);
	/* next nibles follow otp layout */
	for (i = 61; i < 112; i += 4) {
		res = otp_read(i, 4, &otp_data, 1);
		if (res)
			goto error;
		handle_dev.uid[index++] = encode_nible(otp_data, &xor_reg);
	}

	return STALS_NO_ERROR;

error:
	return res;
}

VD6283::STALS_ErrCode_t VD6283::trim_oscillators()
{
	STALS_ErrCode_t res;
	uint16_t hf_trim = handle_dev.is_otp_usage_enable ? handle_dev.otp.hf_trim :
		VD6283_DEFAULT_HF_TRIM;
	uint8_t lf_trim = handle_dev.is_otp_usage_enable ? handle_dev.otp.lf_trim :
		VD6283_DEFAULT_LF_TRIM;

	/* 10.24Mhz */
	res = STALS_WrByte(VD6283_OSC10M, 1);
	if (res) return res;

	res = STALS_WrByte(VD6283_OSC10M_TRIM_M, (hf_trim >> 8) & 0x01);
	if (res) return res;

	res = STALS_WrByte(VD6283_OSC10M_TRIM_L, (hf_trim >> 0) & 0xff);
	if (res) return res;

	/* 48.7 Khz */
	res = STALS_WrByte(VD6283_OSC50K_TRIM, lf_trim & 0x0f);
	if (res) return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::clamp_enable()
{
	STALS_ErrCode_t res;

	res = STALS_WrByte(VD6283_AC_CLAMP_EN, 1);
	if (res)
		return res;
	res = STALS_WrByte(VD6283_DC_CLAMP_EN, 0x1f);
	if (res)
		return res;
	/* set count to be independent of pd nummbers */
	res = STALS_WrByte(VD6283_SPARE_1, 0x3f);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::select_pd()
{
	uint8_t pds[] = {0x07, 0x07, 0x07, 0x1f, 0x0f, 0x1f};
	STALS_ErrCode_t res;
	int c;

	pds[3] = 0x0d;
	for (c = 0; c < STALS_ALS_MAX_CHANNELS; c++) {
		res = STALS_WrByte(VD6283_SEL_PD_x(c), pds[c]);
		if (res)
			return res;
	}

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::set_dithering()
{
	STALS_ErrCode_t res;

	res = STALS_WrByte(VD6283_OSC_DITHERING_STEP_VALUE, 1);
	if (res)
		return res;
	res = STALS_WrByte(VD6283_OSC_DITHERING_STEP_DURATION, 1);
	if (res)
		return res;
	res = STALS_WrByte(VD6283_OSC_DITHERING_STEP_NUMBER, 0x09);
	if (res)
		return res;
	res = STALS_WrByte(VD6283_OSC_DITHERING_CTRL, 1);
	if (res)
		return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::dev_configuration()
{
	STALS_ErrCode_t res;
	int c;

	res = otp_reset();
	if (res) return res;

	res = opt_read_init();
	if (res) return res;

	res = otp_read_param();
	if (res) return res;

	res = otp_generate_uid();
	if (res) return res;

	res = trim_oscillators();
	if (res) return res;

	res = clamp_enable();
	if (res) return res;

	res = select_pd();
	if (res) return res;

	for (c = 0; c < STALS_ALS_MAX_CHANNELS; c++) {
		res = set_channel_gain(c, 0x0100);
		if (res) return res;
	}

	res = set_pedestal_value(3);
	if (res) return res;

	res = set_dithering();
	if (res) return res;

	res = STALS_WrByte(VD6283_SPARE_0,	VD6283_SPARE_0_IN_GPIO2);
	if (res) return res;

	return STALS_NO_ERROR;
}

void VD6283::put_device()
{
	dev_sw_reset();
	/* reset otp to avoid high consumption */
	otp_reset();
	STALS_WrByte(VD6283_SPARE_0, VD6283_SPARE_0_IN_GPIO2);
	handle_dev.st = DEV_FREE;
}


VD6283::STALS_ErrCode_t VD6283::get_channel_gain(int c,	uint16_t *pAppliedGain)
{
	*pAppliedGain = handle_dev.gains[c];

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::set_channel_gain(int c,	uint16_t Gain)
{
	STALS_ErrCode_t res;
	uint8_t vref;

	for (vref = ARRAY_SIZE(GainRange) - 1; vref > 0; vref--) {
		if (Gain < GainRangeThreshold[vref - 1])
			break;
	}

	res = STALS_WrByte(VD6283_CHANNEL_VREF(c), vref + 1);
	if (res) return res;
	handle_dev.gains[c] = GainRange[vref];

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::get_exposure(uint32_t *pAppliedExpoTimeUs)
{
    /*
    uint8_t temp;

    SafeI2C::readByte8(VD6283_EXPOSURE_M, temp);
    *pAppliedExpoTimeUs = temp & 0x3;
    *pAppliedExpoTimeUs <<= 8;

    SafeI2C::readByte8(VD6283_EXPOSURE_L, temp);
    *pAppliedExpoTimeUs |= temp;
    *pAppliedExpoTimeUs *= VD6283_EXPOSURE_STEP_SIZE_US;
    */

    uint16_t temp;
    if (SafeI2C::readWord8(VD6283_EXPOSURE_M, temp)) return STALS_ERROR_READ;    
    
    *pAppliedExpoTimeUs = (temp & 0x3FF) * VD6283_EXPOSURE_STEP_SIZE_US; 
    
    return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::set_exposure(uint32_t ExpoTimeInUs)
{
	uint16_t exposure;
	uint64_t exposure_acc;
	STALS_ErrCode_t res;

	/* avoid integer overflow using intermediate 64 bits arithmetics */
	exposure_acc = ExpoTimeInUs + (VD6283_EXPOSURE_STEP_SIZE_US / 2); // rounding = VD6283_EXPOSURE_STEP_SIZE_US / 2
	exposure = div64_u64(MIN(exposure_acc, 0xffffffffULL), VD6283_EXPOSURE_STEP_SIZE_US);
	exposure = MAX(exposure, 1);
	exposure = MIN(exposure, 0x3ff);
	
    /*
    res = STALS_WrByte(VD6283_EXPOSURE_L, exposure & 0xff);
	if (res) return res;
	
    res = STALS_WrByte(VD6283_EXPOSURE_M, (exposure >> 8) & 0x3);
	if (res) return res;
    
	handle_dev.exposure = exposure * VD6283_EXPOSURE_STEP_SIZE_US;
    */

    if (SafeI2C::writeWord8(VD6283_EXPOSURE_M, exposure)) return STALS_ERROR_WRITE;
	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::dev_enable_channels_for_mode(STALS_Mode_t mode, uint8_t channels)
{
	uint8_t active_chan = handle_dev.als.chan | handle_dev.flk.chan;
	int is_flk = mode == STALS_MODE_FLICKER;
	uint8_t dc_chan_en;
	STALS_ErrCode_t res;

	dc_chan_en = (channels & VD6283_DC_CHANNELS_MASK) | handle_dev.dc_chan_en;
	if (channels & active_chan)
		return STALS_ERROR_INVALID_PARAMS;

	if (dc_chan_en ^ handle_dev.dc_chan_en) {
		res = STALS_WrByte(VD6283_DC_EN, dc_chan_en);
		if (res)
			return res;
	}
	if ((channels & VD6283_AC_CHANNELS_MASK) &&
	    !handle_dev.ac_chan_en) {
		res = STALS_WrByte(VD6283_AC_EN, 1);
		if (res) {
			STALS_WrByte(VD6283_DC_EN,
				handle_dev.dc_chan_en);
			return res;
		}
		handle_dev.ac_chan_en = 1;
	}

	handle_dev.dc_chan_en = dc_chan_en;

	if (is_flk)
		handle_dev.flk.chan = channels;
	else
		handle_dev.als.chan = channels;

	return STALS_NO_ERROR;
}

uint8_t VD6283::dev_disable_dc_chan_en_for_mode(STALS_Mode_t mode)
{
	int is_flk = mode == STALS_MODE_FLICKER;
	uint8_t channels = is_flk ? handle_dev.flk.chan : handle_dev.als.chan;
	uint8_t res;

	res = handle_dev.dc_chan_en & ~(channels & VD6283_DC_CHANNELS_MASK);

	return res;
}

VD6283::STALS_ErrCode_t VD6283::dev_disable_channels_for_mode(STALS_Mode_t mode)
{
	int is_flk = mode == STALS_MODE_FLICKER;
	uint8_t channels = is_flk ? handle_dev.flk.chan : handle_dev.als.chan;
	uint8_t dc_chan_en = dev_disable_dc_chan_en_for_mode(mode);
	STALS_ErrCode_t res;

	if (dc_chan_en ^ handle_dev.dc_chan_en) {
		res = STALS_WrByte(VD6283_DC_EN, dc_chan_en);
		if (res)
			return res;
	}

	if (channels & VD6283_AC_CHANNELS_MASK) {
		res = STALS_WrByte(VD6283_AC_EN, 0);
		if (res) {
			STALS_WrByte(VD6283_DC_EN,
				handle_dev.dc_chan_en);
			return res;
		}
		handle_dev.ac_chan_en = 0;
	}
	handle_dev.dc_chan_en = dc_chan_en;

	if (is_flk)
		handle_dev.flk.chan = 0;
	else
		handle_dev.als.chan = 0;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::enable_flicker_output_mode()
{
	STALS_ErrCode_t res;
	uint8_t pdm_select_output = PDM_SELECT_GPIO1;
	uint8_t pdm_select_clk = PDM_SELECT_INTERNAL_CLK;
	uint8_t intr_config = 0x00;
	uint8_t sel_dig = 0x00;
	uint8_t spare0 = VD6283_SPARE_0_IN_GPIO2;

	STALS_FlickerOutputType_t FlickerOutputType = handle_dev.flicker_output_type;

	/* STALS_FLICKER_OUTPUT_ANALOG_CFG_1 : pdm data on GPIO1 pin through
	 *internal resistance and use internal clock for sigma-delta 1 bit dac.
	 * STALS_FLICKER_OUTPUT_DIGITAL_PDM : pdm data on GPIO1 pin and use
	 * external clock feed from GPIO2 pin to drive sigma-delta 1 bit dac.
	 * STALS_FLICKER_OUTPUT_ANALOG_CFG_2 : pdm data on GPIO2 pin and use
	 * internal clock for sigma-delta 1 bit dac.
	 * STALS_FLICKER_OUTPUT_ZC_CFG_1 : zc output on GPIO1 in.
	 * STALS_FLICKER_OUTPUT_ZC_CFG_2 : zc output on GPIO2 pin.
	 */
	switch (FlickerOutputType)
    {
	case STALS_FLICKER_OUTPUT_ANALOG_CFG_1:
		intr_config = 0x02;
		sel_dig = 0x0f;
		break;
	case STALS_FLICKER_OUTPUT_DIGITAL_PDM:
		pdm_select_clk = PDM_SELECT_EXTERNAL_CLK;
		intr_config = 0x01;
		sel_dig = 0x0f;
		break;
	case STALS_FLICKER_OUTPUT_ANALOG_CFG_2:
		pdm_select_output = PDM_SELECT_GPIO2;
		spare0 = VD6283_SPARE_0_OUT_GPIO2;
		break;
	case STALS_FLICKER_OUTPUT_ZC_CFG_1:
		intr_config = 0x01;
		spare0 = VD6283_SPARE_0_IN_GPIO2 | VD6283_SPARE_0_ZC_GPIO1;
		break;
	case STALS_FLICKER_OUTPUT_ZC_CFG_2:
		spare0 = VD6283_SPARE_0_OUT_GPIO2 | VD6283_SPARE_0_ZC_GPIO2;
		break;
	default:
		return STALS_ERROR_INVALID_PARAMS;
	}

	res = ac_mode_update(PDM_SELECT_OUTPUT | PDM_SELECT_CLK, pdm_select_output | pdm_select_clk);
	if (res) return res;

	/* interrupt pin output stage configuration */
	res = STALS_WrByte(VD6283_INTR_CFG, intr_config);
	if (res) return res;

	/* interrupt output selection */
	res = STALS_WrByte(VD6283_DTEST_SELECTION, sel_dig);
	if (res) return res;

	res = STALS_WrByte(VD6283_SPARE_0, spare0);
	if (res) return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::disable_flicker_output_mode()
{
	STALS_ErrCode_t res;

	/* restore spare0 */
	res = STALS_WrByte(VD6283_SPARE_0, VD6283_SPARE_0_IN_GPIO2);
	if (res) return res;

	/* restore ac_mode bit 4 and bit 5 */
	res = ac_mode_update(PDM_SELECT_OUTPUT | PDM_SELECT_CLK, PDM_SELECT_GPIO1 | PDM_SELECT_INTERNAL_CLK);
	if (res) return res;

	/* restore OD output stage configuration */
	res = STALS_WrByte(VD6283_INTR_CFG, 0x00);
	if (res) return res;

	/* and output als_complete signal */
	res = STALS_WrByte(VD6283_DTEST_SELECTION, 0x00);
	if (res) return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::stop_als(STALS_Mode_t mode)
{
	STALS_ErrCode_t res;

	if (handle_dev.st != DEV_ALS_RUN && handle_dev.st != DEV_BOTH_RUN)
		return STALS_ERROR_NOT_STARTED;

	if (handle_dev.als_started_mode != mode)
		return STALS_ERROR_NOT_STARTED;

	res = STALS_WrByte(VD6283_ALS_CTRL, 0);
	if (res) return res;

	res = dev_disable_channels_for_mode(mode);
	if (res) return res;

	handle_dev.st = handle_dev.st == DEV_ALS_RUN ? DEV_INIT : DEV_FLICKER_RUN;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t  VD6283::start_als(uint8_t channels, STALS_Mode_t mode)
{
	STALS_ErrCode_t res;
	uint32_t cmd = (mode == STALS_MODE_ALS_SYNCHRONOUS) ? VD6283_ALS_START |
		VD6283_ALS_CONTINUOUS | VD6283_ALS_CONTINUOUS_SLAVED :
		VD6283_ALS_START;

	if (handle_dev.st == DEV_ALS_RUN || handle_dev.st == DEV_BOTH_RUN)
		return STALS_ERROR_ALREADY_STARTED;

	//if (!is_als_autogain_valid(dev, channels))
		//return STALS_ERROR_INVALID_PARAMS;

	res = dev_enable_channels_for_mode(mode, channels);
	if (res)
		return res;

	if (handle_dev.st == DEV_FLICKER_RUN) {
		/* stop flicker */
		res = ac_mode_update(AC_EXTRACTOR, AC_EXTRACTOR_DISABLE);
		if (res) goto start_als_error;
	}

	res = STALS_WrByte(VD6283_ALS_CTRL, cmd);
	if (res) goto start_als_error;

	if (handle_dev.st == DEV_FLICKER_RUN) {
		/* restart flicker */
		res = ac_mode_update(AC_EXTRACTOR, AC_EXTRACTOR_ENABLE);
	}

	handle_dev.st = handle_dev.st == DEV_INIT ? DEV_ALS_RUN : DEV_BOTH_RUN;
	handle_dev.als_started_mode = mode;

	return STALS_NO_ERROR;

start_als_error:
	dev_disable_channels_for_mode(mode);
	return res;
}

VD6283::STALS_ErrCode_t VD6283::start_single_shot(uint8_t channels)
{
	return start_als(channels, STALS_MODE_ALS_SINGLE_SHOT);
}

VD6283::STALS_ErrCode_t VD6283::start_synchronous(uint8_t channels)
{
	return start_als(channels, STALS_MODE_ALS_SYNCHRONOUS);
}

VD6283::STALS_ErrCode_t VD6283::start_flicker(uint8_t channels)
{
	STALS_ErrCode_t res;
	uint8_t ac_channel_select;

	/* This check only one channel is selected */
	CHECK_CHANNEL_VALID(channelId_2_index((STALS_Channel_Id_t)channels));

	if (handle_dev.st == DEV_FLICKER_RUN || handle_dev.st == DEV_BOTH_RUN)
		return STALS_ERROR_ALREADY_STARTED;

	res = dev_enable_channels_for_mode(STALS_MODE_FLICKER, channels);
	if (res)
		return res;

	res = enable_flicker_output_mode();
	if (res) {
		dev_disable_channels_for_mode(STALS_MODE_FLICKER);
		return res;
	}

	/* now enable ac and select channel ac channel */
	ac_channel_select = channels == STALS_CHANNEL_6 ? 1 :
		channelId_2_index((STALS_Channel_Id_t)channels) + 2;
	res = ac_mode_update(AC_CHANNEL_SELECT | AC_EXTRACTOR,	(ac_channel_select << 1) | AC_EXTRACTOR_ENABLE);
	if (res) {
		disable_flicker_output_mode();
		dev_disable_channels_for_mode(STALS_MODE_FLICKER);
		return res;
	}

	handle_dev.st = handle_dev.st == DEV_INIT ? DEV_FLICKER_RUN : DEV_BOTH_RUN;

	return res;
}

VD6283::STALS_ErrCode_t VD6283::stop_single_shot()
{
	return stop_als(STALS_MODE_ALS_SINGLE_SHOT);
}

VD6283::STALS_ErrCode_t VD6283::stop_synchronous()
{
	return stop_als(STALS_MODE_ALS_SYNCHRONOUS);
}

VD6283::STALS_ErrCode_t VD6283::stop_flicker()
{
	STALS_ErrCode_t res;

	if (handle_dev.st != DEV_FLICKER_RUN && handle_dev.st != DEV_BOTH_RUN)
		return STALS_ERROR_NOT_STARTED;

	res = ac_mode_update(AC_EXTRACTOR, AC_EXTRACTOR_DISABLE);
	if (res) return res;

	disable_flicker_output_mode();
	dev_disable_channels_for_mode(STALS_MODE_FLICKER);
	handle_dev.st = handle_dev.st == DEV_FLICKER_RUN ? DEV_INIT : DEV_ALS_RUN;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::get_pedestal_enable(uint32_t *value)
{
	STALS_ErrCode_t res;
	uint8_t ac_mode;

	res = STALS_RdByte(VD6283_AC_MODE, &ac_mode);
	*value = (ac_mode & AC_PEDESTAL) ? STALS_CONTROL_DISABLE : STALS_CONTROL_ENABLE;

	return res;
}

VD6283::STALS_ErrCode_t VD6283::get_pedestal_value(uint32_t *value)
{
	STALS_ErrCode_t res;
	uint8_t data;

	res = STALS_RdByte(VD6283_AC_PEDESTAL, &data);
	*value = data & VD6283_PEDESTAL_VALUE_MASK;

	return res;
}

VD6283::STALS_ErrCode_t VD6283::get_otp_usage_enable(uint32_t *value)
{
	*value = handle_dev.is_otp_usage_enable;
	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::get_output_dark_enable(uint32_t *value)
{
	*value = handle_dev.is_output_dark_enable;
	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::get_sda_drive_value(uint32_t *value)
{
	STALS_ErrCode_t res;
	uint8_t data;

	res = STALS_RdByte(VD6283_SDA_DRIVE, &data);
	data &= VD6283_SDA_DRIVE_MASK;
	*value = (data + 1) * 4;

	return res;
}

VD6283::STALS_ErrCode_t VD6283::get_saturation_value(uint32_t *value)
{
	STALS_ErrCode_t res;
	uint8_t sat_m_before;
	uint8_t sat_m_after;
	uint8_t sat_l;

	do
    {
		res = STALS_RdByte(VD6283_AC_SAT_METRIC_M, &sat_m_before);
		res = (STALS_ErrCode_t) (res | STALS_RdByte(VD6283_AC_SAT_METRIC_L, &sat_l));
		res = (STALS_ErrCode_t) (res | STALS_RdByte(VD6283_AC_SAT_METRIC_M, &sat_m_after));
		if (res) return res;
	}
    while(sat_m_before != sat_m_after);
	*value = (sat_m_before << 8) + sat_l;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::get_als_autogain_ctrl(uint32_t *value)
{
	*value = handle_dev.als_autogain_ctrl;
	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::set_pedestal_enable(uint32_t value)
{
	return ac_mode_update(AC_PEDESTAL, value ? AC_PEDESTAL_ENABLE : AC_PEDESTAL_DISABLE);
}

VD6283::STALS_ErrCode_t VD6283::set_pedestal_value(uint32_t value)
{
	return STALS_WrByte(VD6283_AC_PEDESTAL, value & VD6283_PEDESTAL_VALUE_MASK);
}

VD6283::STALS_ErrCode_t VD6283::set_otp_usage_enable(uint32_t value)
{
	STALS_ErrCode_t res;

	handle_dev.is_otp_usage_enable = (STALS_Control_t) value;
	/* oscillator trimming configuration need to be done again */
	res = trim_oscillators();
	if (res) return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::set_output_dark_enable(uint32_t value)
{
	STALS_ErrCode_t res;
	uint8_t pd_value = value ? 0x18 : 0x07;

	res = STALS_WrByte(VD6283_SEL_PD_x(1), pd_value);
	if (res) return res;

	handle_dev.is_output_dark_enable = (STALS_Control_t) value;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::set_sda_drive_value(uint32_t value)
{
	STALS_ErrCode_t res;
	uint8_t data;
	uint8_t sda_drive_reg_value;

	/* valid values are 4, 8, 12, 16, 20 */
	if (value > 20 || value == 0 || value % 4)
		return STALS_ERROR_INVALID_PARAMS;

	sda_drive_reg_value = value / 4 - 1;
	res = STALS_RdByte(VD6283_SDA_DRIVE, &data);
	if (res) return res;

	data = (data & ~VD6283_SDA_DRIVE_MASK) | sda_drive_reg_value;
	return STALS_WrByte(VD6283_SDA_DRIVE, data);
}

VD6283::STALS_ErrCode_t VD6283::set_als_autogain_ctrl(uint32_t value)
{
	return STALS_ERROR_FNCT_DEPRECATED;
}

static uint8_t calfactor( uint8_t gain)
{
    if (gain&BIT(3)) return 0x80|(gain&0x07)<<2;
    else return (0x20-gain)<<2;
}

static uint8_t calfactor1050(uint16_t channelgain, uint8_t gain)
{
    if (channelgain < 0x2154) /* use cal10 */  
    return calfactor((gain&0xF0)>>4);
    else /*use cal50*/ 
    return calfactor(gain&0x0F);
}

static uint32_t cook_values_slope_channel(uint8_t gain, uint32_t raw)
{
 	uint32_t cooked;

 	/* note that no overflow can occur */
 	cooked = (raw * gain) >> 7;

 	/* be sure to not generate values greater that saturation level ... */
 	cooked = MIN(cooked, 0x00ffffff);
 	/* ... but also avoid value below 0x100 except for zero */
 	cooked = cooked < 0x100 ? 0 : cooked;

 	return cooked;
}

void VD6283::cook_values_slope(struct STALS_Als_t *pAlsValue)
{
 	uint8_t otp_gains[STALS_ALS_MAX_CHANNELS];
 	int c;

 	for (c = 0; c < VD6283_CHANNEL_NB; c++)
    {
 		otp_gains[c] = handle_dev.is_otp_usage_enable ? calfactor1050(handle_dev.gains[c], handle_dev.otp.gains[c]) : VD6283_DEFAULT_GAIN;
 	}
 	for (c = 0; c < VD6283_CHANNEL_NB; c++)
    {
 		if (!(handle_dev.als.chan & (1 << c))) continue;
 		pAlsValue->CountValue[c] = cook_values_slope_channel(otp_gains[c], pAlsValue->CountValueRaw[c]);
 	}
}

void VD6283::cook_values(struct STALS_Als_t *pAlsValue)
{
 	cook_values_slope(pAlsValue);
}

//***********************************************************************************

/* public API */
/* STALS_ERROR_INVALID_PARAMS */
VD6283::STALS_ErrCode_t VD6283::STALS_Init()
{
	STALS_ErrCode_t res;
		
	//setup_device(&handle_dev, pClient, dev_7bits_i2c_addr);
	// no otp read yet
	res = check_supported_device();
	if (res) goto check_supported_device_error;
	
    res = dev_sw_reset();
	if (res) goto dev_sw_reset_error;
	
    res = dev_configuration();
	if (res) goto dev_configuration_error;
	
    // now we can check otp version
	if (handle_dev.otp.otp_version != VD6283_DEFAULT_OTP_VERSION)
		goto check_supported_device_error;

	return STALS_NO_ERROR;

dev_configuration_error:
dev_sw_reset_error:
check_supported_device_error:
	put_device();
	return STALS_ERROR_INIT;
}

VD6283::STALS_ErrCode_t VD6283::STALS_Term()
{
	put_device();
	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::STALS_SetExposureTime(uint32_t ExpoTimeInUs, uint32_t *pAppliedExpoTimeUs)
{	
	STALS_ErrCode_t res;

	CHECK_NULL_PTR(pAppliedExpoTimeUs);
	CHECH_DEV_ST_ALS_NOT_STARTED((&handle_dev));

	res = set_exposure(ExpoTimeInUs);
	if (res) return res;

	return get_exposure(pAppliedExpoTimeUs);
}

VD6283::STALS_ErrCode_t VD6283::STALS_GetExposureTime(uint32_t *pAppliedExpoTimeUs)
{
	CHECK_NULL_PTR(pAppliedExpoTimeUs);
	return get_exposure(pAppliedExpoTimeUs);
}

VD6283::STALS_ErrCode_t VD6283::STALS_SetInterMeasurementTime(uint32_t InterMeasurmentInUs, uint32_t *pAppliedInterMeasurmentInUs)
{
	const uint32_t step_size_us = 20500;
	const uint32_t rounding = step_size_us / 2;
	STALS_ErrCode_t res;
	uint64_t value_acc;
	uint8_t value;

	CHECK_NULL_PTR(pAppliedInterMeasurmentInUs);
	CHECH_DEV_ST_ALS_NOT_STARTED((&handle_dev));

	/* avoid integer overflow using intermediate 64 bits arithmetics */
	value_acc = InterMeasurmentInUs + (uint64_t) rounding;
	value_acc = div64_u64(MIN(value_acc, 0xffffffffULL), step_size_us);
	value = MIN(value_acc, 0xff);

	res = STALS_WrByte(VD6283_CONTINUOUS_PERIOD, value);
    if (res) return res;

	return STALS_GetInterMeasurementTime(pAppliedInterMeasurmentInUs);
}

VD6283::STALS_ErrCode_t VD6283::STALS_GetInterMeasurementTime(uint32_t *pAppliedInterMeasurmentInUs)
{
	const uint32_t step_size_us = 20500;
	STALS_ErrCode_t res;
	uint8_t value;

	CHECK_NULL_PTR(pAppliedInterMeasurmentInUs);

	res = STALS_RdByte(VD6283_CONTINUOUS_PERIOD, &value);
    if (res) return res;

	*pAppliedInterMeasurmentInUs = step_size_us * value;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::STALS_SetGain(STALS_Channel_Id_t ChannelId,	uint16_t Gain, uint16_t *pAppliedGain)
{	
    CHECK_SINGLE_CHANNEL(ChannelId);
    CHECK_NULL_PTR(pAppliedGain);
    CHECK_CHANNEL_NOT_IN_USE((&handle_dev), ChannelId); 

	int chan = channelId_2_index(ChannelId);
	STALS_ErrCode_t res;

    CHECK_CHANNEL_VALID(chan);
	
	res = set_channel_gain(chan, Gain);	
    if (res) return res;

	res = get_channel_gain(chan, pAppliedGain);

	return res;
}

VD6283::STALS_ErrCode_t VD6283::STALS_GetGain(STALS_Channel_Id_t ChannelId,	uint16_t *pAppliedGain)
{	
    CHECK_SINGLE_CHANNEL(ChannelId);
	CHECK_NULL_PTR(pAppliedGain);

	int chan = channelId_2_index(ChannelId);
	STALS_ErrCode_t res;

	CHECK_CHANNEL_VALID(chan);

	res = get_channel_gain(chan, pAppliedGain);
	if (res) return res;

	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::STALS_SetFlickerOutputType(STALS_FlickerOutputType_t FlickerOutputType)
{
	STALS_ErrCode_t res = STALS_NO_ERROR;

	CHECH_DEV_ST_FLICKER_NOT_STARTED((&handle_dev));

	switch (FlickerOutputType) {
	case STALS_FLICKER_OUTPUT_ANALOG_CFG_1:
	case STALS_FLICKER_OUTPUT_DIGITAL_PDM:
		handle_dev.flicker_output_type = FlickerOutputType;
		break;
	case STALS_FLICKER_OUTPUT_ANALOG_CFG_2:
		handle_dev.flicker_output_type = FlickerOutputType;
		break;
	case STALS_FLICKER_OUTPUT_ZC_CFG_1:
	case STALS_FLICKER_OUTPUT_ZC_CFG_2:
		handle_dev.flicker_output_type = FlickerOutputType;
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

VD6283::STALS_ErrCode_t VD6283::STALS_Start(STALS_Mode_t Mode,	STALS_Channel_Id_t Channels)
{	
	STALS_ErrCode_t res;
	CHECK_CHANNEL_MASK_VALID(Channels);

	switch (Mode)
    {
	case STALS_MODE_ALS_SINGLE_SHOT:
		res = start_single_shot(Channels);
		break;
	case STALS_MODE_ALS_SYNCHRONOUS:
		res = start_synchronous(Channels);
		break;
	case STALS_MODE_FLICKER:
		res = start_flicker(Channels);
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}
	return res;
}

VD6283::STALS_ErrCode_t VD6283::STALS_Stop(STALS_Mode_t Mode)
{	
	STALS_ErrCode_t res;

	switch (Mode)
    {
	case STALS_MODE_ALS_SINGLE_SHOT:
		res = stop_single_shot();
		break;
	case STALS_MODE_ALS_SYNCHRONOUS:
		res = stop_synchronous();
		break;
	case STALS_MODE_FLICKER:
		res = stop_flicker();
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

VD6283::STALS_ErrCode_t VD6283::STALS_GetAlsValues(STALS_Channel_Id_t Channels, struct STALS_Als_t *pAlsValue, uint8_t *pMeasureValid)
{
	uint8_t Channels_save = Channels;
	STALS_ErrCode_t res;
	uint8_t c;

	CHECK_NULL_PTR(pAlsValue);
	CHECK_NULL_PTR(pMeasureValid);
	CHECK_CHANNEL_MASK_VALID_FOR_ALS(Channels);
	CHECK_DEV_ST_ALS_STARTED((&handle_dev));

	if (handle_dev.st != DEV_ALS_RUN && handle_dev.st != DEV_BOTH_RUN)
		return STALS_ERROR_NOT_STARTED;

	/* do we have ready datas */
	res = is_data_ready(pMeasureValid);
	if (res) return res;
	if (!*pMeasureValid) return STALS_NO_ERROR;

	/* yes so read them */
	/* be sure to read als autogain channels if als autogain is enable */
	if ((handle_dev.als_autogain_ctrl & 1) == STALS_CONTROL_ENABLE)
	{
       	Channels = (STALS_Channel_Id_t)((uint8_t)Channels | ALS_AUTO_VALUE_2_CHANNELS(handle_dev.als_autogain_ctrl));
    }
    
    Channels = (STALS_Channel_Id_t)(Channels & handle_dev.als.chan);
	for (c = 0; c < VD6283_CHANNEL_NB; c++) {
		if (!(Channels & (1 << c)))	continue;
		res = read_channel(c, &pAlsValue->CountValueRaw[c]);
		if (res) return res;
		pAlsValue->Gains[c] = handle_dev.gains[c];
	}
	pAlsValue->Channels = Channels_save;
	cook_values(pAlsValue);

	/* acknowledge irq */
	return acknowledge_irq();
}

VD6283::STALS_ErrCode_t VD6283::STALS_SetControl(enum STALS_Control_Id_t ControlId, uint32_t ControlValue)
{	
	STALS_ErrCode_t res;

	switch (ControlId) {
	case STALS_PEDESTAL_ENABLE:
		CHECH_DEV_ST_FLICKER_NOT_STARTED((&handle_dev));
		res = set_pedestal_enable(ControlValue);
		break;
	case STALS_PEDESTAL_VALUE:
		CHECH_DEV_ST_FLICKER_NOT_STARTED((&handle_dev));
		res = set_pedestal_value(ControlValue);
		break;
	case STALS_OTP_USAGE_ENABLE:
		CHECH_DEV_ST_INIT((&handle_dev));
		res = set_otp_usage_enable(ControlValue);
		break;
	case STALS_OUTPUT_DARK_ENABLE:
		CHECH_DEV_ST_INIT((&handle_dev));
		res = set_output_dark_enable(ControlValue);
		break;
	case STALS_SDA_DRIVE_VALUE_MA:
		CHECH_DEV_ST_INIT((&handle_dev));
		res = set_sda_drive_value(ControlValue);
		break;
	case STALS_WA_STATE:
		res = STALS_ERROR_INVALID_PARAMS;
		break;
	case STALS_ALS_AUTOGAIN_CTRL:
		CHECH_DEV_ST_ALS_NOT_STARTED((&handle_dev));
		res = set_als_autogain_ctrl(ControlValue);
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

VD6283::STALS_ErrCode_t VD6283::STALS_GetControl(enum STALS_Control_Id_t ControlId, uint32_t *pControlValue)
{	
	STALS_ErrCode_t res;

	CHECK_NULL_PTR(pControlValue);

	switch (ControlId) {
	case STALS_PEDESTAL_ENABLE:
		res = get_pedestal_enable(pControlValue);
		break;
	case STALS_PEDESTAL_VALUE:
		res = get_pedestal_value(pControlValue);
		break;
	case STALS_OTP_USAGE_ENABLE:
		res = get_otp_usage_enable(pControlValue);
		break;
	case STALS_OUTPUT_DARK_ENABLE:
		res = get_output_dark_enable(pControlValue);
		break;
	case STALS_SDA_DRIVE_VALUE_MA:
		res = get_sda_drive_value(pControlValue);
		break;
	case STALS_WA_STATE:
		res = STALS_ERROR_INVALID_PARAMS;
		break;
	case STALS_SATURATION_VALUE:
		res = get_saturation_value(pControlValue);
		break;
	case STALS_ALS_AUTOGAIN_CTRL:
		res = get_als_autogain_ctrl(pControlValue);
		break;
	default:
		res = STALS_ERROR_INVALID_PARAMS;
	}

	return res;
}

VD6283::STALS_ErrCode_t VD6283::STALS_GetUid(char **pUid)
{	
	CHECK_NULL_PTR(pUid);
	*pUid = &handle_dev.uid[0];
	return STALS_NO_ERROR;
}

VD6283::STALS_ErrCode_t VD6283::read()
{  
    uint8_t is_valid;
    STALS_ErrCode_t err;

    // START READ
    err = STALS_Start(STALS_MODE_ALS_SINGLE_SHOT, STALS_CHANNEL_ALL);
    if (err) return err;

    // WAIT WHEN DATA READY
    do 
    {
        err = STALS_GetAlsValues(STALS_CHANNEL_ALL, &meas, &is_valid);
        if (err) return err;
        if (!is_valid) ThisThread::yield();            
    }
    while(!is_valid);
    
    // STOP READ
    return STALS_Stop(STALS_MODE_ALS_SINGLE_SHOT); 
}