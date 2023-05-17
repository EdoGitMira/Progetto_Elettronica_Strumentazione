#include "VL53L1X.h"
#include "VL53L1X_api.h"

#define DEVIDE_ID 0xEACC

VL53L1X::VL53L1X(PinName sda, PinName scl, const PinName & cs, const uint8_t & boot_address, const int & frequency)
: SafeI2C(sda, scl, boot_address, frequency), enable_pin(cs), external_enable(nullptr) { }

VL53L1X::VL53L1X(PinName sda, PinName scl, PLTDigitalOutBase * cs, const uint8_t & boot_address, const int & frequency)
: SafeI2C(sda, scl, boot_address, frequency), enable_pin(NC), external_enable(cs) { }

int VL53L1X::init(const uint8_t & address)
{
    int status;

    status = private_init();
    RETURN_IF_ERROR_STATUS();

    status = VL53L1X_StartRanging();
    RETURN_IF_ERROR_STATUS();
    
    return VL53L1X_SetI2CAddress(address);
}

int VL53L1X::read(uint32_t & distance)
{
    VL53L1X_ERROR status;
    VL53L1X_Result_t result;
    uint8_t range_sataus;

    /*status = VL53L1X_StartRanging();
    RETURN_IF_ERROR_STATUS();*/

    status = VL53L1X_GetRangeStatus(&range_sataus);
    RETURN_IF_ERROR_STATUS();

    status = VL53L1X_GetDistance(&distance);
    RETURN_IF_ERROR_STATUS();

    /*status = VL53L1X_StopRanging();
    RETURN_IF_ERROR_STATUS();*/

    if (range_sataus <= 14) return (Exception) range_sataus;
    return Exception::RANGESTATUS_NONE;    
}

int VL53L1X::setRangeMode(const Range & mode)
{
    return VL53L1X_SetDistanceMode((uint16_t)mode);
}

int VL53L1X::enable()
{
    if (enable_pin != NC)
    {
        DigitalOut(enable_pin, 1);
    }
    if (external_enable)
    {
        external_enable->write(1);
    }
    ThisThread::sleep_for(10);
    return Exception::NO_ERROR;
}

int VL53L1X::disable() 
{
    if (enable_pin != NC) 
    {
        DigitalOut(enable_pin, 0);
    }
    if (external_enable)
    {
        external_enable->write(0);
    }
    ThisThread::sleep_for(10);
    return Exception::NO_ERROR;
}

bool VL53L1X::isPresent()
{
    uint16_t id;
    Exception error;
    error = VL53L1X_GetSensorId(&id);
    if (error) return false;
    return id == DEVIDE_ID;
}

/*******************************************************************/

int VL53L1X::private_init()
{
    uint16_t sensorID;
    int status;

    status = disable();
    RETURN_IF_ERROR_STATUS();
    
    status = enable();
    RETURN_IF_ERROR_STATUS();

    status = VL53L1X_GetSensorId(&sensorID);
    RETURN_IF_ERROR_STATUS();

    if (sensorID != DEVIDE_ID) return Exception::ERROR_DEVICE_NOT_COMPATIBLE;
    
    return VL53L1X_SensorInit();
}