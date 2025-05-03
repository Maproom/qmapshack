/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023 Garmin International, Inc.
// Licensed under the Flexible and Interoperable Data Transfer (FIT) Protocol License; you
// may not use this file except in compliance with the Flexible and Interoperable Data
// Transfer (FIT) Protocol License.
/////////////////////////////////////////////////////////////////////////////////////////////


#if !defined(FIT_THREE_D_SENSOR_ADJUSTMENT_PLUGIN_HPP)
#define FIT_THREE_D_SENSOR_ADJUSTMENT_PLUGIN_HPP

#include "fit.hpp"
#include "fit_three_d_sensor_calibration_mesg.hpp"
#include "fit_mesg_broadcast_plugin.hpp"
#include <vector>

namespace fit
{

class ThreeDSensorAdjustmentPlugin : public fit::MesgBroadcastPlugin
{
    public:
        void OnBroadcast(std::vector<Mesg>& mesgs);
        void OnIncomingMesg(const Mesg& mesg);
        ThreeDSensorAdjustmentPlugin();
        // Plugins are not designed to be copied
        ThreeDSensorAdjustmentPlugin & operator=(const ThreeDSensorAdjustmentPlugin&) = delete;
        ThreeDSensorAdjustmentPlugin(const ThreeDSensorAdjustmentPlugin&) = delete;

    private:
        class CalibrationParameters
        {
            public:
                FIT_UINT32 calFactor;
                FIT_UINT32 calDivisor;
                FIT_UINT32 channelOffset[3];
                FIT_UINT32 levelShift;
                FIT_FLOAT32 rotationMatrix[3][3];

                void LoadParams(const fit::ThreeDSensorCalibrationMesg& calMessage);
        };

        void adjustSensorData(FIT_FLOAT32 (&rawData)[3], CalibrationParameters calParams);
        void ProcessCalibrationFactor(fit::Mesg& mesg, const FIT_UINT8* fieldsXYZ, const FIT_FLOAT32 calibratedXYZ[]);

        FIT_BOOL haveAccelCal;
        FIT_BOOL haveGyroCal;
        FIT_BOOL haveMagCal;

        CalibrationParameters accelCalParams;
        CalibrationParameters gyroCalParams;
        CalibrationParameters magCalParams;

        const FIT_UINT8 X_AXIS_OFFSET = 0;
        const FIT_UINT8 Y_AXIS_OFFSET = 1;
        const FIT_UINT8 Z_AXIS_OFFSET = 2;

        const FIT_UINT8 X_AXIS_FIELD_NUM = 5;
        const FIT_UINT8 Y_AXIS_FIELD_NUM = 6;
        const FIT_UINT8 Z_AXIS_FIELD_NUM = 7;
};

} // namespace fit

#endif // defined(FIT_THREE_D_SENSOR_ADJUSTMENT_PLUGIN_HPP)
