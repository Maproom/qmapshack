/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023 Garmin International, Inc.
// Licensed under the Flexible and Interoperable Data Transfer (FIT) Protocol License; you
// may not use this file except in compliance with the Flexible and Interoperable Data
// Transfer (FIT) Protocol License.
/////////////////////////////////////////////////////////////////////////////////////////////


#include "fit_three_d_sensor_adjustment_plugin.hpp"
#include "fit_three_d_sensor_calibration_mesg.hpp"
#include "fit_accelerometer_data_mesg.hpp"
#include "fit_gyroscope_data_mesg.hpp"
#include "fit_magnetometer_data_mesg.hpp"



namespace fit
{
    ThreeDSensorAdjustmentPlugin::ThreeDSensorAdjustmentPlugin()
    {
    }

    // peeks at incoming messages as they are being added to the buffer
    void ThreeDSensorAdjustmentPlugin::OnIncomingMesg(const Mesg& mesg)
    {
        // peek at incoming message
        switch (mesg.GetNum())
        {
            // capture the incoming calibration parameters
            case FIT_MESG_NUM_THREE_D_SENSOR_CALIBRATION:
            {
                ThreeDSensorCalibrationMesg calMesg(mesg);

                switch( calMesg.GetSensorType() )
                {
                    case FIT_SENSOR_TYPE_ACCELEROMETER:
                        haveAccelCal = FIT_TRUE;
                        accelCalParams.LoadParams(calMesg);
                        break;
                    case FIT_SENSOR_TYPE_GYROSCOPE:
                        haveGyroCal = FIT_TRUE;
                        gyroCalParams.LoadParams(calMesg);
                        break;
                    case FIT_SENSOR_TYPE_COMPASS:
                        haveMagCal = FIT_TRUE;
                        magCalParams.LoadParams(calMesg);
                        break;
                    default:
                        break;
                }
                break;
            }

            default:
                break;
        } // switch
    }

    // detects 3D sensor messages and adjusts the raw data if calibration data is
    // available. New fields are added to contain the adjusted sensor data
    void ThreeDSensorAdjustmentPlugin::OnBroadcast(std::vector<Mesg>& mesgs)
    {
        FIT_FLOAT32 XYZvalues[3];
        // iterate through all messages in vector
        for(size_t i = 0; i < mesgs.size(); i++)
        {
            switch( mesgs[i].GetNum() )
            {
                case FIT_MESG_NUM_ACCELEROMETER_DATA:
                    if( haveAccelCal )
                    {
                        AccelerometerDataMesg accelData(mesgs[i]);

                        for(FIT_UINT8 j = 0; j < accelData.GetNumAccelX(); j++)
                        {
                            // extract the uncalibrated accel data from incoming message
                            XYZvalues[X_AXIS_OFFSET] = (FIT_FLOAT32)accelData.GetAccelX(j);
                            XYZvalues[Y_AXIS_OFFSET] = (FIT_FLOAT32)accelData.GetAccelY(j);
                            XYZvalues[Z_AXIS_OFFSET] = (FIT_FLOAT32)accelData.GetAccelZ(j);

                            // apply calibration
                            adjustSensorData(XYZvalues, accelCalParams);

                            FIT_UINT8 fieldNums[3] = { X_AXIS_FIELD_NUM, Y_AXIS_FIELD_NUM, Z_AXIS_FIELD_NUM };
                            ProcessCalibrationFactor(mesgs[i], (const FIT_UINT8*)fieldNums, (const FIT_FLOAT32*)&XYZvalues);
                        }
                    }
                    break;
                case FIT_MESG_NUM_GYROSCOPE_DATA:
                    if( haveGyroCal )
                    {
                        GyroscopeDataMesg gyroData(mesgs[i]);

                        for (FIT_UINT8 j = 0; j < gyroData.GetNumGyroX(); j++)
                        {
                            // extract the uncalibrated accel data from incoming message
                            XYZvalues[X_AXIS_OFFSET] = (FIT_FLOAT32)gyroData.GetGyroX(j);
                            XYZvalues[Y_AXIS_OFFSET] = (FIT_FLOAT32)gyroData.GetGyroY(j);
                            XYZvalues[Z_AXIS_OFFSET] = (FIT_FLOAT32)gyroData.GetGyroZ(j);

                            // apply calibration
                            adjustSensorData(XYZvalues, gyroCalParams);

                            FIT_UINT8 fieldNums[3] = { X_AXIS_FIELD_NUM, Y_AXIS_FIELD_NUM, Z_AXIS_FIELD_NUM };
                            ProcessCalibrationFactor(mesgs[i], (const FIT_UINT8*)fieldNums, (const FIT_FLOAT32*)&XYZvalues);
                        }
                    }
                    break;
                case FIT_MESG_NUM_MAGNETOMETER_DATA:
                    if( haveMagCal )
                    {
                        MagnetometerDataMesg magData(mesgs[i]);

                        for (FIT_UINT8 j = 0; j < magData.GetNumMagX(); j++)
                        {
                            // extract the uncalibrated accel data from incoming message
                            XYZvalues[X_AXIS_OFFSET] = (FIT_FLOAT32)magData.GetMagX(j);
                            XYZvalues[Y_AXIS_OFFSET] = (FIT_FLOAT32)magData.GetMagY(j);
                            XYZvalues[Z_AXIS_OFFSET] = (FIT_FLOAT32)magData.GetMagZ(j);

                            // apply calibration
                            adjustSensorData(XYZvalues, magCalParams);

                            FIT_UINT8 fieldNums[3] = { X_AXIS_FIELD_NUM, Y_AXIS_FIELD_NUM, Z_AXIS_FIELD_NUM };
                            ProcessCalibrationFactor(mesgs[i], (const FIT_UINT8*)fieldNums, (const FIT_FLOAT32*)&XYZvalues);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    void ThreeDSensorAdjustmentPlugin::ProcessCalibrationFactor(fit::Mesg& mesg, const FIT_UINT8* fieldsXYZ, const FIT_FLOAT32 calibratedXYZ[])
    {
        // add calibrated data fields to existing mesg if they are not already present
        if(mesg.GetField(fieldsXYZ[X_AXIS_OFFSET]) == NULL)
        {
            mesg.AddField(X_AXIS_FIELD_NUM);
        }
        if(mesg.GetField(fieldsXYZ[Y_AXIS_OFFSET]) == NULL)
        {
            mesg.AddField(Y_AXIS_FIELD_NUM);
        }
        if(mesg.GetField(fieldsXYZ[Z_AXIS_OFFSET]) == NULL)
        {
            mesg.AddField(Z_AXIS_FIELD_NUM);
        }

        FIT_UINT8 index = mesg.GetField(fieldsXYZ[X_AXIS_OFFSET])->GetNumValues();

        // add values to new fields
        mesg.GetField(fieldsXYZ[X_AXIS_OFFSET])->SetFLOAT32Value(calibratedXYZ[X_AXIS_OFFSET], index);
        mesg.GetField(fieldsXYZ[Y_AXIS_OFFSET])->SetFLOAT32Value(calibratedXYZ[Y_AXIS_OFFSET], index);
        mesg.GetField(fieldsXYZ[Z_AXIS_OFFSET])->SetFLOAT32Value(calibratedXYZ[Z_AXIS_OFFSET], index);
    }

    // load the calibration parameters from the incoming calibration message
    void ThreeDSensorAdjustmentPlugin::CalibrationParameters::LoadParams(const fit::ThreeDSensorCalibrationMesg& calMesg)
    {
        this->calFactor = calMesg.GetCalibrationFactor();
        this->calDivisor = calMesg.GetCalibrationDivisor();
        this->levelShift = calMesg.GetLevelShift();

        this->channelOffset[0] = calMesg.GetOffsetCal(0);
        this->channelOffset[1] = calMesg.GetOffsetCal(1);
        this->channelOffset[2] = calMesg.GetOffsetCal(2);

        FIT_UINT8 numRows = 3;
        FIT_UINT8 numColumns = 3;

        // populate rotationMatrix
        // rotationMatrix is row major
        for (FIT_UINT8 i = 0; i < numRows; i++)
            for(FIT_UINT8 j = 0; j < numColumns; j++)
                this->rotationMatrix[i][j] = calMesg.GetOrientationMatrix(i*numColumns + j);
    }

    void ThreeDSensorAdjustmentPlugin::adjustSensorData(FIT_FLOAT32 (&rawData)[3], CalibrationParameters calParams)
    {
        FIT_FLOAT32 tempData[3];

        // apply calibration parameters
        for(int i = 0; i < 3; i++)
        {
            tempData[i] = rawData[i];
            tempData[i] -= calParams.levelShift;
            tempData[i] -= calParams.channelOffset[i];
            tempData[i] *= calParams.calFactor;
            tempData[i] /= calParams.calDivisor;
        }

        // apply rotation matrix
        // [Rotation] * [XYZ]
        rawData[0] = calParams.rotationMatrix[0][0]*tempData[0] + calParams.rotationMatrix[0][1]*tempData[1] + calParams.rotationMatrix[0][2]*tempData[2];
        rawData[1] = calParams.rotationMatrix[1][0]*tempData[0] + calParams.rotationMatrix[1][1]*tempData[1] + calParams.rotationMatrix[1][2]*tempData[2];
        rawData[2] = calParams.rotationMatrix[2][0]*tempData[0] + calParams.rotationMatrix[2][1]*tempData[1] + calParams.rotationMatrix[2][2]*tempData[2];
    }
} // namespace fit
