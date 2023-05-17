#include "VL53L0X.h"

// the ranging_sensor_comms.dll will take care of the page selection
VL53L0X_Error VL53L0X::VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count)
{
    if (VL53L0X::writeMulti8(index, pdata, count)) return VL53L0X_ERROR_CONTROL_INTERFACE;
    else return VL53L0X_ERROR_NONE;
}

// the ranging_sensor_comms.dll will take care of the page selection
VL53L0X_Error VL53L0X::VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count)
{

    if (VL53L0X::readMulti8(index, pdata, count)) return VL53L0X_ERROR_CONTROL_INTERFACE;
    else return VL53L0X_ERROR_NONE;
}


VL53L0X_Error VL53L0X::VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data)
{
    if (VL53L0X::writeMulti8(index, &data, 1)) return VL53L0X_ERROR_CONTROL_INTERFACE;
    else return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X::VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data)
{
    if (VL53L0X::writeWord8(index, data)) return VL53L0X_ERROR_CONTROL_INTERFACE;
    else return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X::VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data)
{
    if (VL53L0X::writeDoubleWord8(index, data)) return VL53L0X_ERROR_CONTROL_INTERFACE;
    else return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X::VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData){
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    int32_t status_int;
    uint8_t data;

    status_int = VL53L0X_RdByte(Dev, index, &data);

    if (status_int != 0)
        Status = VL53L0X_ERROR_CONTROL_INTERFACE;

    if (Status == VL53L0X_ERROR_NONE) {
        data = (data & AndData) | OrData;
        status_int = VL53L0X_WrByte(Dev, index, data);

        if (status_int != 0)
            Status = VL53L0X_ERROR_CONTROL_INTERFACE;
    }

    return Status;
}

VL53L0X_Error VL53L0X::VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data){
    if (VL53L0X::readMulti8(index, data, 1)) return VL53L0X_ERROR_CONTROL_INTERFACE;
    else return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X::VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data)
{
    if (VL53L0X::readWord8(index, *data)) return VL53L0X_ERROR_CONTROL_INTERFACE;        
    else return VL53L0X_ERROR_NONE;
}

VL53L0X_Error  VL53L0X::VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data)
{
    if (VL53L0X::readDoubleWord8(index, *data)) return VL53L0X_ERROR_CONTROL_INTERFACE;    
    else return VL53L0X_ERROR_NONE;
}