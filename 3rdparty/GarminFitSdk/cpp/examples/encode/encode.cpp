////////////////////////////////////////////////////////////////////////////////
// The following FIT Protocol software provided may be used with FIT protocol
// devices only and remains the copyrighted property of Garmin Canada Inc.
// The software is being provided on an "as-is" basis and as an accommodation,
// and therefore all warranties, representations, or guarantees of any kind
// (whether express, implied or statutory) including, without limitation,
// warranties of merchantability, non-infringement, or fitness for a particular
// purpose, are specifically disclaimed.
//
// Copyright 2008-2016 Garmin Canada Inc.
////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <cstdlib>
#include <cmath>
#include <time.h>

#include "fit_encode.hpp"
#include "fit_mesg_broadcaster.hpp"
#include "fit_file_id_mesg.hpp"
#include "fit_date_time.hpp"

// 2 * PI (3.14159265)
#define TWOPI 6.2831853

// Number of semicircles per meter at the equator
#define SC_PER_M 107.173

int EncodeActivityFile()
{
	try
	{
		// Open the file
		std::fstream file;
		file.open("ExampleActivity.fit", std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

		if (!file.is_open())
		{
			printf("Error opening file ExampleActivity.fit\n");
			return -1;
		}

		// Create a FIT Encode object
		fit::Encode encode(fit::ProtocolVersion::V20);

		// Write the FIT header to the output stream
		encode.Open(file);

		// The starting timestamp for the activity
		fit::DateTime startTime(std::time(0));

		// Every FIT file MUST contain a File ID message
		fit::FileIdMesg fileIdMesg;
		fileIdMesg.SetType(FIT_FILE_ACTIVITY);
		fileIdMesg.SetManufacturer(FIT_MANUFACTURER_DEVELOPMENT);
		fileIdMesg.SetProduct(1);
		fileIdMesg.SetTimeCreated(startTime.GetTimeStamp());
		// You should create a serial number unique to your platform
		srand((unsigned int)time(NULL));
		fileIdMesg.SetSerialNumber(rand() % 10000 + 1);
		encode.Write(fileIdMesg);

		// Create the Developer Id message for the developer data fields.
		fit::DeveloperDataIdMesg developerIdMesg;
		// It is a BEST PRACTICE to use the same Guid for all FIT files created by your platform
		// 00010203-0405-0607-0809-0A0B0C0D0E0F
		FIT_UINT8 appId[] = { 0x03,0x02,0x01,0x00,0x05,0x04,0x07,0x06,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };
		for (FIT_UINT8 i = 0; i < 16; i++)
		{
			developerIdMesg.SetApplicationId(i, appId[i]);
		}
		developerIdMesg.SetDeveloperDataIndex(0);
		developerIdMesg.SetApplicationVersion(110);
		encode.Write(developerIdMesg);

		// Create the Developer Data Field Descriptions
		fit::FieldDescriptionMesg doughnutsFieldDescMesg;
		doughnutsFieldDescMesg.SetDeveloperDataIndex(0);
		doughnutsFieldDescMesg.SetFieldDefinitionNumber(0);
		doughnutsFieldDescMesg.SetFitBaseTypeId(FIT_BASE_TYPE_FLOAT32);
		doughnutsFieldDescMesg.SetFieldName(0, L"Doughnuts Earned");
		doughnutsFieldDescMesg.SetUnits(0, L"Doughnuts");
		doughnutsFieldDescMesg.SetNativeMesgNum(FIT_MESG_NUM_SESSION);
		encode.Write(doughnutsFieldDescMesg);

		fit::FieldDescriptionMesg hrFieldDescMesg;
		hrFieldDescMesg.SetDeveloperDataIndex(0);
		hrFieldDescMesg.SetFieldDefinitionNumber(1);
		hrFieldDescMesg.SetFitBaseTypeId(FIT_BASE_TYPE_UINT8);
		hrFieldDescMesg.SetFieldName(0, L"Heart Rate");
		hrFieldDescMesg.SetUnits(0, L"bpm");
		hrFieldDescMesg.SetNativeFieldNum(fit::RecordMesg::FieldDefNum::HeartRate);
		hrFieldDescMesg.SetNativeMesgNum(FIT_MESG_NUM_RECORD);
		encode.Write(hrFieldDescMesg);

		// Timer Events are a BEST PRACTICE for FIT ACTIVITY files
		fit::EventMesg eventMesgStart;
		eventMesgStart.SetTimestamp(startTime.GetTimeStamp());
		eventMesgStart.SetEvent(FIT_EVENT_TIMER);
		eventMesgStart.SetEventType(FIT_EVENT_TYPE_START);
		encode.Write(eventMesgStart);

		// Every FIT ACTIVITY file MUST contain Record messages
		fit::DateTime timestamp(startTime);

		// Create one hour (3600 seconds) of Record data
		for (FIT_UINT16 i = 0; i < 3600; i++)
		{
			// Create a new Record message and set the timestamp
			fit::RecordMesg recordMesg;
			recordMesg.SetTimestamp(timestamp.GetTimeStamp());

			// Fake Record Data of Various Signal Patterns
			recordMesg.SetDistance(i); // Ramp
			recordMesg.SetSpeed(1); // Flatline
			recordMesg.SetHeartRate((FIT_UINT8)((sin(TWOPI * (0.01 * i + 10)) + 1.0) * 127.0)); // Sine
			recordMesg.SetCadence((FIT_UINT8)(i % 255)); // Sawtooth
			recordMesg.SetPower((FIT_UINT16)((i % 255) < 127 ? 150 : 250)); // Square
			recordMesg.SetAltitude((float)std::abs(((float)(i % 255)) - 127.0f)); // Triangle
			recordMesg.SetPositionLat(0);
			recordMesg.SetPositionLong((FIT_SINT32)(i * SC_PER_M));

			// Add a Developer Field to the Record Message
			fit::DeveloperField developerHrField(hrFieldDescMesg, developerIdMesg);
			developerHrField.AddValue((FIT_UINT8)((sin(TWOPI * (0.01 * i + 10)) + 1.0) * 127.0)); // Sine
			recordMesg.AddDeveloperField(developerHrField);

			// Write the Rercord message to the output stream
			encode.Write(recordMesg);

			// Increment the timestamp by one second
			timestamp.add(fit::DateTime((FIT_DATE_TIME)1));
		}

		// Timer Events are a BEST PRACTICE for FIT ACTIVITY files
		fit::EventMesg eventMesgStop;
		eventMesgStop.SetTimestamp(timestamp.GetTimeStamp());
		eventMesgStop.SetEvent(FIT_EVENT_TIMER);
		eventMesgStop.SetEventType(FIT_EVENT_TYPE_STOP);
		encode.Write(eventMesgStop);

		// Every FIT ACTIVITY file MUST contain at least one Lap message
		fit::LapMesg lapMesg;
		lapMesg.SetTimestamp(timestamp.GetTimeStamp());
		lapMesg.SetStartTime(startTime.GetTimeStamp());
		lapMesg.SetTotalElapsedTime((FIT_FLOAT32)(timestamp.GetTimeStamp() - startTime.GetTimeStamp()));
		lapMesg.SetTotalTimerTime((FIT_FLOAT32)(timestamp.GetTimeStamp() - startTime.GetTimeStamp()));
		encode.Write(lapMesg);

		// Every FIT ACTIVITY file MUST contain at least one Session message
		fit::SessionMesg sessionMesg;
		sessionMesg.SetTimestamp(timestamp.GetTimeStamp());
		sessionMesg.SetStartTime(startTime.GetTimeStamp());
		sessionMesg.SetTotalElapsedTime((FIT_FLOAT32)(timestamp.GetTimeStamp() - startTime.GetTimeStamp()));
		sessionMesg.SetTotalTimerTime((FIT_FLOAT32)(timestamp.GetTimeStamp() - startTime.GetTimeStamp()));
		sessionMesg.SetSport(FIT_SPORT_STAND_UP_PADDLEBOARDING);
		sessionMesg.SetSubSport(FIT_SUB_SPORT_GENERIC);
		sessionMesg.SetFirstLapIndex(0);
		sessionMesg.SetNumLaps(1);

		// Add a Developer Field to the Session message
		fit::DeveloperField doughnutsEarnedDevField(doughnutsFieldDescMesg, developerIdMesg);
		doughnutsEarnedDevField.AddValue(sessionMesg.GetTotalElapsedTime() / 1200.0f);
		sessionMesg.AddDeveloperField(doughnutsEarnedDevField);
		encode.Write(sessionMesg);

		// Every FIT ACTIVITY file MUST contain EXACTLY one Activity message
		fit::ActivityMesg activityMesg;
		activityMesg.SetTimestamp(timestamp.GetTimeStamp());
		activityMesg.SetNumSessions(1);
		int timezoneOffset = -7 * 3600;
		activityMesg.SetLocalTimestamp((FIT_LOCAL_DATE_TIME)((int)timestamp.GetTimeStamp() + timezoneOffset));
		encode.Write(activityMesg);

		// Update the data size in the header and calculate the CRC
		if (!encode.Close())
		{
			printf("Error closing encode.\n");
			return -1;
		}

		// Close the file
		file.close();

		printf("Encoded FIT file ExampleActivity.fit.\n");
		return 0;
	}
	catch (...)
	{
		throw std::exception("Exception encoding activity file");
	}
}
int EncodeSettingsFile()
{
	try
	{
		fit::Encode encode(fit::ProtocolVersion::V10);
		std::fstream file;

		file.open("ExampleSettings.fit", std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

		if (!file.is_open())
		{
			printf("Error opening file ExampleSettings.fit\n");
			return -1;
		}

		fit::FileIdMesg fileIdMesg; // Every FIT file requires a File ID message
		fileIdMesg.SetType(FIT_FILE_SETTINGS);
		fileIdMesg.SetManufacturer(FIT_MANUFACTURER_DEVELOPMENT);
		fileIdMesg.SetProduct(1);
		fileIdMesg.SetSerialNumber(12345);

		fit::UserProfileMesg userProfileMesg;
		userProfileMesg.SetGender(FIT_GENDER_FEMALE);
		userProfileMesg.SetWeight((FIT_FLOAT32)63.1);
		userProfileMesg.SetAge(99);
		std::wstring wstring_name(L"TestUser");
		userProfileMesg.SetFriendlyName(wstring_name);

		encode.Open(file);
		encode.Write(fileIdMesg);
		encode.Write(userProfileMesg);

		if (!encode.Close())
		{
			printf("Error closing encode.\n");
			return -1;
		}
		file.close();

		printf("Encoded FIT file ExampleSettings.fit.\n");
		return 0;
	}
	catch (...)
	{
		throw std::exception("Exception encoding settings file.");
	}
}

int EncodeMonitoringFile()
{
	try
	{
		fit::Encode encode(fit::ProtocolVersion::V10);
		std::fstream file;

		time_t current_time_unix = time(0);
		fit::DateTime initTime(current_time_unix);

		file.open("ExampleMonitoringFile.fit", std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

		if (!file.is_open())
		{
			printf("Error opening file ExampleMonitoringFile.fit\n");
			return -1;
		}

		encode.Open(file);

		fit::FileIdMesg fileIdMesg; // Every FIT file requires a File ID message
		fileIdMesg.SetType(FIT_FILE_MONITORING_B);
		fileIdMesg.SetManufacturer(FIT_MANUFACTURER_DEVELOPMENT);
		fileIdMesg.SetProduct(1);
		fileIdMesg.SetSerialNumber(12345);

		encode.Write(fileIdMesg);

		fit::DeviceInfoMesg deviceInfoMesg;
		deviceInfoMesg.SetTimestamp(initTime.GetTimeStamp()); // Convert to FIT time and write timestamp.
		deviceInfoMesg.SetBatteryStatus(FIT_BATTERY_STATUS_GOOD);

		encode.Write(deviceInfoMesg);

		fit::MonitoringMesg monitoringMesg;

		// By default, each time a new message is written the Local Message Type 0 will be redefined to match the new message.
		// In this case,to avoid having a definition message each time there is a DeviceInfoMesg, we can manually set the Local Message Type of the MonitoringMessage to '1'.
		// By doing this we avoid an additional 7 definition messages in our FIT file.
		monitoringMesg.SetLocalNum(1);

		monitoringMesg.SetTimestamp(initTime.GetTimeStamp()); // Initialise Timestamp to now
		monitoringMesg.SetCycles(0); // Initialise Cycles to 0
		for (int i = 0; i < 4; i++) // This loop represents 1/6 of a day
		{
			for (int j = 0; j < 4; j++) // Each one of these loops represent 1 hour
			{
				fit::DateTime walkingTime(current_time_unix);
				monitoringMesg.SetTimestamp(walkingTime.GetTimeStamp());
				monitoringMesg.SetActivityType(FIT_ACTIVITY_TYPE_WALKING); // By setting this to WALKING, the Cycles field will be interpretted as Steps
				monitoringMesg.SetCycles(monitoringMesg.GetCycles() + (rand() % 1000 + 1)); // Cycles are accumulated (i.e. must be increasing)
				encode.Write(monitoringMesg);
				current_time_unix += (time_t)(3600); //Add an hour to our contrieved timestamp
			}
			fit::DateTime statusTime(current_time_unix);
			deviceInfoMesg.SetTimestamp(statusTime.GetTimeStamp());
			deviceInfoMesg.SetBatteryStatus(FIT_BATTERY_STATUS_GOOD);
			encode.Write(deviceInfoMesg);

		}

		if (!encode.Close())
		{
			printf("Error closing encode.\n");
			return -1;
		}
		file.close();

		printf("Encoded FIT file ExampleMonitoringFile.fit.\n");
		return 0;
	}
	catch (...)
	{
		throw std::exception("Exception encoding monitoring file.");
	}
}

int main()
{
    printf("FIT Encode Example Application\n");

    int returnValue = 0;

	try
	{
		returnValue += EncodeSettingsFile();
		returnValue += EncodeMonitoringFile();
		returnValue += EncodeActivityFile();
	}
	catch (const std::exception &e)
	{
		printf("Exception occurred while encoding example files: %s", e.what());
		return -1;
	}

    return returnValue;
}
