////////////////////////////////////////////////////////////////////////////////
// The following FIT Protocol software provided may be used with FIT protocol
// devices only and remains the copyrighted property of Garmin Canada Inc.
// The software is being provided on an "as-is" basis and as an accommodation,
// and therefore all warranties, representations, or guarantees of any kind
// (whether express, implied or statutory) including, without limitation,
// warranties of merchantability, non-infringement, or fitness for a particular
// purpose, are specifically disclaimed.
//
// Copyright 2016 Garmin Canada Inc.
////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>

#include "fit_decode.hpp"
#include "fit_buffered_mesg_broadcaster.hpp"
#include "fit_three_d_sensor_adjustment_plugin.hpp"

class Listener : public fit::FileIdMesgListener, public fit::AccelerometerDataMesgListener, public fit::GyroscopeDataMesgListener, public fit::ThreeDSensorCalibrationMesgListener
{
public :
    void OnMesg(fit::FileIdMesg& mesg)
    {
        fprintf(fileDescriptor, "File ID:,");
        if (mesg.GetType() != FIT_FILE_INVALID)
            fprintf(fileDescriptor, "Type: %d,", mesg.GetType());
        if (mesg.GetManufacturer() != FIT_MANUFACTURER_INVALID)
            fprintf(fileDescriptor, "Manufacturer: %d,", mesg.GetManufacturer());
        if (mesg.GetProduct() != FIT_UINT16_INVALID)
            fprintf(fileDescriptor, "Product: %d,", mesg.GetProduct());
        if (mesg.GetSerialNumber() != FIT_UINT32Z_INVALID)
            fprintf(fileDescriptor, "Serial Number: %u,", mesg.GetSerialNumber());
        if (mesg.GetNumber() != FIT_UINT16_INVALID)
            fprintf(fileDescriptor, "Number: %d,", mesg.GetNumber());
        fprintf(fileDescriptor, "\n");
    }

    void OnMesg(fit::AccelerometerDataMesg& mesg)
    {
        fprintf(fileDescriptor, "Accelerometer data:,");
        FIT_UINT8 numX, numY, numZ;

        numX = mesg.GetNumCalibratedAccelX();
        numY = mesg.GetNumCalibratedAccelY();
        numZ = mesg.GetNumCalibratedAccelZ();

        if (numX != FIT_UINT8_INVALID)
        {
            fprintf(fileDescriptor, "Calibrated Accel X,");
            for (FIT_UINT8 i = 0; i < numX; i++)
            {
                FIT_FLOAT32 calibratedAccelX = mesg.GetCalibratedAccelX(i);
                if (memcmp(&calibratedAccelX, &FIT_FLOAT32_INVALID, sizeof(FIT_FLOAT32)) != 0)
                {
                    fprintf(fileDescriptor, "[%f]", calibratedAccelX);
                }
            }
            fprintf(fileDescriptor, ",");
        }
        if (numY != FIT_UINT8_INVALID)
        {
            fprintf(fileDescriptor, "Calibrated Accel Y,");
            for (FIT_UINT8 i = 0; i < numY; i++)
            {
                FIT_FLOAT32 calibratedAccelY = mesg.GetCalibratedAccelY(i);
                if (memcmp(&calibratedAccelY, &FIT_FLOAT32_INVALID, sizeof(FIT_FLOAT32)) != 0)
                {
                fprintf(fileDescriptor, "[%f]", calibratedAccelY);
                }
            }
            fprintf(fileDescriptor, ",");
        }
        if (numZ != FIT_UINT8_INVALID)
        {
            fprintf(fileDescriptor, "Calibrated Accel Z,");
            for (FIT_UINT8 i = 0; i < numZ; i++)
            {
                FIT_FLOAT32 calibratedAccelZ = mesg.GetCalibratedAccelZ(i);
                if (memcmp(&calibratedAccelZ, &FIT_FLOAT32_INVALID, sizeof(FIT_FLOAT32)) != 0)
                {
                    fprintf(fileDescriptor, "[%f]", calibratedAccelZ);
                }
            }
            fprintf(fileDescriptor, ",");
        }
        fprintf(fileDescriptor, "\n");
    }

    void OnMesg(fit::GyroscopeDataMesg& mesg)
    {
        fprintf(fileDescriptor, "Gyroscope data:,");
        FIT_UINT8 numX, numY, numZ;

        numX = mesg.GetNumCalibratedGyroX();
        numY = mesg.GetNumCalibratedGyroY();
        numZ = mesg.GetNumCalibratedGyroZ();

        if (numX != FIT_UINT8_INVALID)
        {
            fprintf(fileDescriptor, "Calibrated Gyro X,");
            for (FIT_UINT8 i = 0; i < numX; i++)
            {
                FIT_FLOAT32 calibratedGyroX = mesg.GetCalibratedGyroX(i);
                if (memcmp(&calibratedGyroX, &FIT_FLOAT32_INVALID, sizeof(FIT_FLOAT32)) != 0)
                {
                    fprintf(fileDescriptor, "[%f]", calibratedGyroX);
                }
            }
            fprintf(fileDescriptor, ",");
        }
        if (numY != FIT_UINT8_INVALID)
        {
            fprintf(fileDescriptor, "Calibrated Gyro Y,");
            for (FIT_UINT8 i = 0; i < numY; i++)
            {
                FIT_FLOAT32 calibratedGyroY = mesg.GetCalibratedGyroY(i);
                if (memcmp(&calibratedGyroY, &FIT_FLOAT32_INVALID, sizeof(FIT_FLOAT32)) != 0)
                {
                    fprintf(fileDescriptor, "[%f]", calibratedGyroY);
                }
            }
            fprintf(fileDescriptor, ",");
        }
        if (numZ != FIT_UINT8_INVALID)
        {
            fprintf(fileDescriptor, "Calibrated Gyro Z,");
            for (FIT_UINT8 i = 0; i < numZ; i++)
            {
                FIT_FLOAT32 calibratedGyroZ = mesg.GetCalibratedGyroZ(i);
                if (memcmp(&calibratedGyroZ, &FIT_FLOAT32_INVALID, sizeof(FIT_FLOAT32)) != 0)
                {
                    fprintf(fileDescriptor, "[%f]", calibratedGyroZ);
                }
            }
            fprintf(fileDescriptor, ",");
        }
        fprintf(fileDescriptor, "\n");
    }

    void OnMesg(fit::ThreeDSensorCalibrationMesg& mesg)
    {
        fprintf(fileDescriptor, "Calibration message,");
        if (mesg.GetCalibrationFactor() != FIT_UINT32_INVALID)
        {
            fprintf(fileDescriptor, "Calibration Factor,%d,", mesg.GetCalibrationFactor());
        }
        if (mesg.GetAccelCalFactor() != FIT_UINT32_INVALID)
        {
            fprintf(fileDescriptor, "Accel Calibration Factor,%d,", mesg.GetAccelCalFactor());
        }
        if (mesg.GetGyroCalFactor() != FIT_UINT32_INVALID)
        {
            fprintf(fileDescriptor, "Gyro Calibration Factor,%d,", mesg.GetGyroCalFactor());
        }
        if (mesg.GetCalibrationDivisor() != FIT_UINT32_INVALID)
        {
            fprintf(fileDescriptor, "Calibration Divisor,%d,", mesg.GetCalibrationDivisor());
        }
        if (mesg.GetLevelShift() != FIT_UINT32_INVALID)
        {
            fprintf(fileDescriptor, "Level Shift,%d,", mesg.GetLevelShift());
        }
        fprintf(fileDescriptor, "\n");
    }

    void SetOutputFileName( std::string file )
    {
        outputfile = file;
    }

    void OpenFile()
    {
        if (fileDescriptor == NULL)
        {
#pragma warning ( suppress : 4996 ) // warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead.
            fileDescriptor = fopen(outputfile.c_str(), "w");
            //Write the csv header out
            fprintf(fileDescriptor, "Type,Local Number,Message,Field 1,Value 1,Units 1,Field 2,Value 2,Units 2,Field 3,Value 3,Units 3,Field 4,Value 4,Units 4,Field 5,Value 5,Units 5,Field 6,Value 6,Units 6\n");
        }
    }

    void CloseFile()
    {
        fclose(fileDescriptor);
        fileDescriptor = NULL;
    }

    Listener()
    {
        fileDescriptor = NULL;
    }

private:
    std::string outputfile;
    FILE *fileDescriptor;
};

int main(int argc, char* argv[])
{
    fit::Decode decode;
    fit::ThreeDSensorAdjustmentPlugin adjust;
    fit::BufferedMesgBroadcaster mesgBroadcaster;
    Listener listener;
    std::fstream file;
    std::string outputFile;

    printf("FIT Decode Example Application\n");

    if (argc != 2)
    {
        printf("Usage: ThreeDSensorAdjustmentExample.exe <input file>.fit\n");
        return -1;
    }

    file.open(argv[1], std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        printf("Error opening file %s\n", argv[1]);
        return -1;
    }

    std::string fitFile = argv[1];
    int index = fitFile.find_last_of(".");
    outputFile = fitFile.substr(0, index) + ".csv";

    //Set the output file for the listener
    listener.SetOutputFileName(outputFile);
    listener.OpenFile();

    if (!decode.CheckIntegrity(file))
    {
        printf("FIT file integrity failed.\nAttempting to decode...\n");
    }
    mesgBroadcaster.RegisterMesgBroadcastPlugin(&adjust);
    mesgBroadcaster.AddListener((fit::FileIdMesgListener &)listener);
    mesgBroadcaster.AddListener((fit::AccelerometerDataMesgListener &)listener);
    mesgBroadcaster.AddListener((fit::GyroscopeDataMesgListener &)listener);
    mesgBroadcaster.AddListener((fit::ThreeDSensorCalibrationMesgListener &)listener);
    //mesgBroadcaster.AddListener((fit::MesgListener &)listener);

    try
    {
        mesgBroadcaster.Run(file);
        mesgBroadcaster.Broadcast();
    }
    catch (const fit::RuntimeException& e)
    {
        listener.CloseFile();
        printf("Exception decoding file: %s\n", e.what());
        return -1;
    }

    listener.CloseFile();
    printf("Adjusted 3D sensor data from %s.\n", argv[1]);

    return 0;
}