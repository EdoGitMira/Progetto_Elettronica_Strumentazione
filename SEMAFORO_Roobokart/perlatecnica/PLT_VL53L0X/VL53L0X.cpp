#include "VL53L0X.h"
#include <cstdio>

VL53L0X::VL53L0X(PinName sda, PinName scl, const PinName & cs, const uint8_t & def_address, const int & frequency) 
: SafeI2C(sda, scl, def_address, frequency), enable_pin(cs), external_enable(nullptr)
{
    
}

VL53L0X::VL53L0X(PinName sda, PinName scl, PLTDigitalOutBase * cs, const uint8_t & def_address, const int & frequency) 
: SafeI2C(sda, scl, def_address, frequency), enable_pin(NC), external_enable(cs)
{
    
}

int VL53L0X::enable()
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
    return 0;
}

int VL53L0X::disable() 
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
    return 0;
}

int VL53L0X::init(uint8_t newaddress)
{
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;
    VL53L0X_Version_t version;
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    disable();
    enable();

    //Status = VL53L0X_GetVersion(&version);
    //if (Status) return VL53L0X_ERROR_CONTROL_INTERFACE;

    Status = VL53L0X_SetDeviceAddress(&MyDevice, newaddress); // Data initialization
    if (Status) return Status;
    
    Status = VL53L0X_DataInit(&MyDevice); // Data initialization
    if (Status) return Status;    
    
    Status = VL53L0X_StaticInit(&MyDevice); // Device Initialization
    if (Status) return Status;

    Status = VL53L0X_PerformRefCalibration(&MyDevice, &VhvSettings, &PhaseCal); // Device Initialization
    if (Status) return Status;

    Status = VL53L0X_PerformRefSpadManagement(&MyDevice, &refSpadCount, &isApertureSpads); // Device Initialization
    if (Status) return Status;

    Status = VL53L0X_SetDeviceMode(&MyDevice, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING); // Setup in single ranging mode
    if (Status) return Status;

    Status = VL53L0X_StartMeasurement(&MyDevice);
    if (Status) return Status;

    Status = setRangeMode(Range::Normal);

    return Status;
}

int VL53L0X::read(uint32_t & distance)
{
    VL53L0X_RangingMeasurementData_t RangingMeasurementData;
    //FixPoint1616_t LimitCheckCurrent;
    int Status = VL53L0X_ERROR_NONE;
              
    //Status = VL53L0X_PerformSingleRangingMeasurement(&MyDevice, &RangingMeasurementData);

    //******************************************************

    Status = VL53L0X_GetRangingMeasurementData(&MyDevice, &RangingMeasurementData);
    
    distance = RangingMeasurementData.RangeMilliMeter;
    //printf("In loop measurement %d: %d\n", measurement, pRangingMeasurementData->RangeMilliMeter);

    // Clear the interrupt
    VL53L0X_ClearInterruptMask(&MyDevice, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
    //ThisThread::sleep_for(1);

    //******************************************************
    
    //VL53L0X_GetLimitCheckCurrent(&MyDevice, VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, &LimitCheckCurrent);

    //printf("RANGE IGNORE THRESHOLD: %f ", (float)LimitCheckCurrent/65536.0);

    if (Status) return Status;

    //printf("distance(%d): %i\n\n", RangingMeasurementData.RangeStatus, RangingMeasurementData.RangeMilliMeter);
    
    //distance = RangingMeasurementData.RangeMilliMeter;
    return RangingMeasurementData.RangeStatus;
}

int VL53L0X::setRangeMode(const Range & range)
{
    int status;

    status = VL53L0X_SetLimitCheckEnable(&MyDevice, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
    if (status) return status;

    status = VL53L0X_SetLimitCheckEnable(&MyDevice, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
    if (status) return status;

    return 0;  
    
    //VL53L0X_SetLimitCheckEnable(&MyDevice, VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 1);
    //VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, (FixPoint1616_t)(1.5 * 0.023 * 65536));
    
    switch (range)
    {        
        case Range::Normal:
            VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.25*65536));
            VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(18*65536));	

            VL53L0X_SetVcselPulsePeriod(&MyDevice, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 14);
            VL53L0X_SetVcselPulsePeriod(&MyDevice, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 10);

            VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&MyDevice, 33823);		
            break;

        case Range::Long:
            //    Status = VL53L0X_SetSequenceStepEnable(pMyDevice,VL53L0X_SEQUENCESTEP_DSS, 1);	
            VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1*65536));
            VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60*65536));

            VL53L0X_SetVcselPulsePeriod(&MyDevice, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
            VL53L0X_SetVcselPulsePeriod(&MyDevice, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);
            
            VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&MyDevice, 33823);
            break;

        case Range::HighAccurancy:
            VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.25*65536));
            VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(18*65536));	

            VL53L0X_SetVcselPulsePeriod(&MyDevice, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 14);
            VL53L0X_SetVcselPulsePeriod(&MyDevice, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 10);

            VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&MyDevice, 200000);                
            break;

        case Range::HighSpeed:    
            VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.25*65536));
            VL53L0X_SetLimitCheckValue(&MyDevice, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(32*65536));

            VL53L0X_SetVcselPulsePeriod(&MyDevice, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 14);
            VL53L0X_SetVcselPulsePeriod(&MyDevice, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 10);

            VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&MyDevice, 30000);                
            break;
    }    

    return status;
}