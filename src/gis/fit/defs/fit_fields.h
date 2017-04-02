/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#ifndef FIT_FIELDS_H
#define FIT_FIELDS_H
// ----------- start generated code -----------
typedef enum {
    eFileIdType = 0,
    eFileIdManufacturer = 1,
    eFileIdProduct = 2,
    eFileIdSerialNumber = 3,
    eFileIdTimeCreated = 4,
    eFileIdNumber = 5,
    eFileIdProductName = 8
} message_file_id_e;

typedef enum {
    eFileCreatorSoftwareVersion = 0,
    eFileCreatorHardwareVersion = 1
} message_file_creator_e;

typedef enum {
    eTimestampCorrelationTimestamp = 253,
    eTimestampCorrelationFractionalTimestamp = 0,
    eTimestampCorrelationSystemTimestamp = 1,
    eTimestampCorrelationFractionalSystemTimestamp = 2,
    eTimestampCorrelationLocalTimestamp = 3,
    eTimestampCorrelationTimestampMs = 4,
    eTimestampCorrelationSystemTimestampMs = 5
} message_timestamp_correlation_e;

typedef enum {
    eSoftwareMessageIndex = 254,
    eSoftwareVersion = 3,
    eSoftwarePartNumber = 5
} message_software_e;

typedef enum {
    eSlaveDeviceManufacturer = 0,
    eSlaveDeviceProduct = 1
} message_slave_device_e;

typedef enum {
    eCapabilitiesLanguages = 0,
    eCapabilitiesSports = 1,
    eCapabilitiesWorkoutsSupported = 21,
    eCapabilitiesConnectivitySupported = 23
} message_capabilities_e;

typedef enum {
    eFileCapabilitiesMessageIndex = 254,
    eFileCapabilitiesType = 0,
    eFileCapabilitiesFlags = 1,
    eFileCapabilitiesDirectory = 2,
    eFileCapabilitiesMaxCount = 3,
    eFileCapabilitiesMaxSize = 4
} message_file_capabilities_e;

typedef enum {
    eMesgCapabilitiesMessageIndex = 254,
    eMesgCapabilitiesFile = 0,
    eMesgCapabilitiesMesgNum = 1,
    eMesgCapabilitiesCountType = 2,
    eMesgCapabilitiesCount = 3
} message_mesg_capabilities_e;

typedef enum {
    eFieldCapabilitiesMessageIndex = 254,
    eFieldCapabilitiesFile = 0,
    eFieldCapabilitiesMesgNum = 1,
    eFieldCapabilitiesFieldNum = 2,
    eFieldCapabilitiesCount = 3
} message_field_capabilities_e;

typedef enum {
    eDeviceSettingsActiveTimeZone = 0,
    eDeviceSettingsUtcOffset = 1,
    eDeviceSettingsTimeOffset = 2,
    eDeviceSettingsTimeMode = 4,
    eDeviceSettingsTimeZoneOffset = 5,
    eDeviceSettingsBacklightMode = 12,
    eDeviceSettingsActivityTrackerEnabled = 36,
    eDeviceSettingsClockTime = 39,
    eDeviceSettingsPagesEnabled = 40,
    eDeviceSettingsMoveAlertEnabled = 46,
    eDeviceSettingsDateMode = 47,
    eDeviceSettingsDisplayOrientation = 55,
    eDeviceSettingsMountingSide = 56,
    eDeviceSettingsDefaultPage = 57,
    eDeviceSettingsAutosyncMinSteps = 58,
    eDeviceSettingsAutosyncMinTime = 59,
    eDeviceSettingsLactateThresholdAutodetectEnabled = 80,
    eDeviceSettingsBleAutoUploadEnabled = 86,
    eDeviceSettingsAutoSyncFrequency = 89,
    eDeviceSettingsAutoActivityDetect = 90,
    eDeviceSettingsNumberOfScreens = 94,
    eDeviceSettingsSmartNotificationDisplayOrientation = 95
} message_device_settings_e;

typedef enum {
    eUserProfileMessageIndex = 254,
    eUserProfileFriendlyName = 0,
    eUserProfileGender = 1,
    eUserProfileAge = 2,
    eUserProfileHeight = 3,
    eUserProfileWeight = 4,
    eUserProfileLanguage = 5,
    eUserProfileElevSetting = 6,
    eUserProfileWeightSetting = 7,
    eUserProfileRestingHeartRate = 8,
    eUserProfileDefaultMaxRunningHeartRate = 9,
    eUserProfileDefaultMaxBikingHeartRate = 10,
    eUserProfileDefaultMaxHeartRate = 11,
    eUserProfileHrSetting = 12,
    eUserProfileSpeedSetting = 13,
    eUserProfileDistSetting = 14,
    eUserProfilePowerSetting = 16,
    eUserProfileActivityClass = 17,
    eUserProfilePositionSetting = 18,
    eUserProfileTemperatureSetting = 21,
    eUserProfileLocalId = 22,
    eUserProfileGlobalId = 23,
    eUserProfileWakeTime = 28,
    eUserProfileSleepTime = 29,
    eUserProfileHeightSetting = 30,
    eUserProfileUserRunningStepLength = 31,
    eUserProfileUserWalkingStepLength = 32
} message_user_profile_e;

typedef enum {
    eHrmProfileMessageIndex = 254,
    eHrmProfileEnabled = 0,
    eHrmProfileHrmAntId = 1,
    eHrmProfileLogHrv = 2,
    eHrmProfileHrmAntIdTransType = 3
} message_hrm_profile_e;

typedef enum {
    eSdmProfileMessageIndex = 254,
    eSdmProfileEnabled = 0,
    eSdmProfileSdmAntId = 1,
    eSdmProfileSdmCalFactor = 2,
    eSdmProfileOdometer = 3,
    eSdmProfileSpeedSource = 4,
    eSdmProfileSdmAntIdTransType = 5,
    eSdmProfileOdometerRollover = 7
} message_sdm_profile_e;

typedef enum {
    eBikeProfileMessageIndex = 254,
    eBikeProfileName = 0,
    eBikeProfileSport = 1,
    eBikeProfileSubSport = 2,
    eBikeProfileOdometer = 3,
    eBikeProfileBikeSpdAntId = 4,
    eBikeProfileBikeCadAntId = 5,
    eBikeProfileBikeSpdcadAntId = 6,
    eBikeProfileBikePowerAntId = 7,
    eBikeProfileCustomWheelsize = 8,
    eBikeProfileAutoWheelsize = 9,
    eBikeProfileBikeWeight = 10,
    eBikeProfilePowerCalFactor = 11,
    eBikeProfileAutoWheelCal = 12,
    eBikeProfileAutoPowerZero = 13,
    eBikeProfileId = 14,
    eBikeProfileSpdEnabled = 15,
    eBikeProfileCadEnabled = 16,
    eBikeProfileSpdcadEnabled = 17,
    eBikeProfilePowerEnabled = 18,
    eBikeProfileCrankLength = 19,
    eBikeProfileEnabled = 20,
    eBikeProfileBikeSpdAntIdTransType = 21,
    eBikeProfileBikeCadAntIdTransType = 22,
    eBikeProfileBikeSpdcadAntIdTransType = 23,
    eBikeProfileBikePowerAntIdTransType = 24,
    eBikeProfileOdometerRollover = 37,
    eBikeProfileFrontGearNum = 38,
    eBikeProfileFrontGear = 39,
    eBikeProfileRearGearNum = 40,
    eBikeProfileRearGear = 41,
    eBikeProfileShimanoDi2Enabled = 44
} message_bike_profile_e;

typedef enum {
    eConnectivityBluetoothEnabled = 0,
    eConnectivityBluetoothLeEnabled = 1,
    eConnectivityAntEnabled = 2,
    eConnectivityName = 3,
    eConnectivityLiveTrackingEnabled = 4,
    eConnectivityWeatherConditionsEnabled = 5,
    eConnectivityWeatherAlertsEnabled = 6,
    eConnectivityAutoActivityUploadEnabled = 7,
    eConnectivityCourseDownloadEnabled = 8,
    eConnectivityWorkoutDownloadEnabled = 9,
    eConnectivityGpsEphemerisDownloadEnabled = 10,
    eConnectivityIncidentDetectionEnabled = 11,
    eConnectivityGrouptrackEnabled = 12
} message_connectivity_e;

typedef enum {
    eWatchfaceSettingsMessageIndex = 254,
    eWatchfaceSettingsMode = 0,
    eWatchfaceSettingsLayout = 1
} message_watchface_settings_e;

typedef enum {
    eOhrSettingsEnabled = 0
} message_ohr_settings_e;

typedef enum {
    eZonesTargetMaxHeartRate = 1,
    eZonesTargetThresholdHeartRate = 2,
    eZonesTargetFunctionalThresholdPower = 3,
    eZonesTargetHrCalcType = 5,
    eZonesTargetPwrCalcType = 7
} message_zones_target_e;

typedef enum {
    eSportSport = 0,
    eSportSubSport = 1,
    eSportName = 3
} message_sport_e;

typedef enum {
    eHrZoneMessageIndex = 254,
    eHrZoneHighBpm = 1,
    eHrZoneName = 2
} message_hr_zone_e;

typedef enum {
    eSpeedZoneMessageIndex = 254,
    eSpeedZoneHighValue = 0,
    eSpeedZoneName = 1
} message_speed_zone_e;

typedef enum {
    eCadenceZoneMessageIndex = 254,
    eCadenceZoneHighValue = 0,
    eCadenceZoneName = 1
} message_cadence_zone_e;

typedef enum {
    ePowerZoneMessageIndex = 254,
    ePowerZoneHighValue = 1,
    ePowerZoneName = 2
} message_power_zone_e;

typedef enum {
    eMetZoneMessageIndex = 254,
    eMetZoneHighBpm = 1,
    eMetZoneCalories = 2,
    eMetZoneFatCalories = 3
} message_met_zone_e;

typedef enum {
    eGoalMessageIndex = 254,
    eGoalSport = 0,
    eGoalSubSport = 1,
    eGoalStartDate = 2,
    eGoalEndDate = 3,
    eGoalType = 4,
    eGoalValue = 5,
    eGoalRepeat = 6,
    eGoalTargetValue = 7,
    eGoalRecurrence = 8,
    eGoalRecurrenceValue = 9,
    eGoalEnabled = 10,
    eGoalSource = 11
} message_goal_e;

typedef enum {
    eActivityTimestamp = 253,
    eActivityTotalTimerTime = 0,
    eActivityNumSessions = 1,
    eActivityType = 2,
    eActivityEvent = 3,
    eActivityEventType = 4,
    eActivityLocalTimestamp = 5,
    eActivityEventGroup = 6
} message_activity_e;

typedef enum {
    eSessionMessageIndex = 254,
    eSessionTimestamp = 253,
    eSessionEvent = 0,
    eSessionEventType = 1,
    eSessionStartTime = 2,
    eSessionStartPositionLat = 3,
    eSessionStartPositionLong = 4,
    eSessionSport = 5,
    eSessionSubSport = 6,
    eSessionTotalElapsedTime = 7,
    eSessionTotalTimerTime = 8,
    eSessionTotalDistance = 9,
    eSessionTotalCycles = 10,
    eSessionTotalCalories = 11,
    eSessionTotalFatCalories = 13,
    eSessionAvgSpeed = 14,
    eSessionMaxSpeed = 15,
    eSessionAvgHeartRate = 16,
    eSessionMaxHeartRate = 17,
    eSessionAvgCadence = 18,
    eSessionMaxCadence = 19,
    eSessionAvgPower = 20,
    eSessionMaxPower = 21,
    eSessionTotalAscent = 22,
    eSessionTotalDescent = 23,
    eSessionTotalTrainingEffect = 24,
    eSessionFirstLapIndex = 25,
    eSessionNumLaps = 26,
    eSessionEventGroup = 27,
    eSessionTrigger = 28,
    eSessionNecLat = 29,
    eSessionNecLong = 30,
    eSessionSwcLat = 31,
    eSessionSwcLong = 32,
    eSessionNormalizedPower = 34,
    eSessionTrainingStressScore = 35,
    eSessionIntensityFactor = 36,
    eSessionLeftRightBalance = 37,
    eSessionAvgStrokeCount = 41,
    eSessionAvgStrokeDistance = 42,
    eSessionSwimStroke = 43,
    eSessionPoolLength = 44,
    eSessionThresholdPower = 45,
    eSessionPoolLengthUnit = 46,
    eSessionNumActiveLengths = 47,
    eSessionTotalWork = 48,
    eSessionAvgAltitude = 49,
    eSessionMaxAltitude = 50,
    eSessionGpsAccuracy = 51,
    eSessionAvgGrade = 52,
    eSessionAvgPosGrade = 53,
    eSessionAvgNegGrade = 54,
    eSessionMaxPosGrade = 55,
    eSessionMaxNegGrade = 56,
    eSessionAvgTemperature = 57,
    eSessionMaxTemperature = 58,
    eSessionTotalMovingTime = 59,
    eSessionAvgPosVerticalSpeed = 60,
    eSessionAvgNegVerticalSpeed = 61,
    eSessionMaxPosVerticalSpeed = 62,
    eSessionMaxNegVerticalSpeed = 63,
    eSessionMinHeartRate = 64,
    eSessionTimeInHrZone = 65,
    eSessionTimeInSpeedZone = 66,
    eSessionTimeInCadenceZone = 67,
    eSessionTimeInPowerZone = 68,
    eSessionAvgLapTime = 69,
    eSessionBestLapIndex = 70,
    eSessionMinAltitude = 71,
    eSessionPlayerScore = 82,
    eSessionOpponentScore = 83,
    eSessionOpponentName = 84,
    eSessionStrokeCount = 85,
    eSessionZoneCount = 86,
    eSessionMaxBallSpeed = 87,
    eSessionAvgBallSpeed = 88,
    eSessionAvgVerticalOscillation = 89,
    eSessionAvgStanceTimePercent = 90,
    eSessionAvgStanceTime = 91,
    eSessionAvgFractionalCadence = 92,
    eSessionMaxFractionalCadence = 93,
    eSessionTotalFractionalCycles = 94,
    eSessionAvgTotalHemoglobinConc = 95,
    eSessionMinTotalHemoglobinConc = 96,
    eSessionMaxTotalHemoglobinConc = 97,
    eSessionAvgSaturatedHemoglobinPercent = 98,
    eSessionMinSaturatedHemoglobinPercent = 99,
    eSessionMaxSaturatedHemoglobinPercent = 100,
    eSessionAvgLeftTorqueEffectiveness = 101,
    eSessionAvgRightTorqueEffectiveness = 102,
    eSessionAvgLeftPedalSmoothness = 103,
    eSessionAvgRightPedalSmoothness = 104,
    eSessionAvgCombinedPedalSmoothness = 105,
    eSessionSportIndex = 111,
    eSessionTimeStanding = 112,
    eSessionStandCount = 113,
    eSessionAvgLeftPco = 114,
    eSessionAvgRightPco = 115,
    eSessionAvgLeftPowerPhase = 116,
    eSessionAvgLeftPowerPhasePeak = 117,
    eSessionAvgRightPowerPhase = 118,
    eSessionAvgRightPowerPhasePeak = 119,
    eSessionAvgPowerPosition = 120,
    eSessionMaxPowerPosition = 121,
    eSessionAvgCadencePosition = 122,
    eSessionMaxCadencePosition = 123,
    eSessionEnhancedAvgSpeed = 124,
    eSessionEnhancedMaxSpeed = 125,
    eSessionEnhancedAvgAltitude = 126,
    eSessionEnhancedMinAltitude = 127,
    eSessionEnhancedMaxAltitude = 128,
    eSessionAvgLevMotorPower = 129,
    eSessionMaxLevMotorPower = 130,
    eSessionLevBatteryConsumption = 131,
    eSessionAvgVerticalRatio = 132,
    eSessionAvgStanceTimeBalance = 133,
    eSessionAvgStepLength = 134,
    eSessionTotalAnaerobicTrainingEffect = 137,
    eSessionAvgVam = 139
} message_session_e;

typedef enum {
    eLapMessageIndex = 254,
    eLapTimestamp = 253,
    eLapEvent = 0,
    eLapEventType = 1,
    eLapStartTime = 2,
    eLapStartPositionLat = 3,
    eLapStartPositionLong = 4,
    eLapEndPositionLat = 5,
    eLapEndPositionLong = 6,
    eLapTotalElapsedTime = 7,
    eLapTotalTimerTime = 8,
    eLapTotalDistance = 9,
    eLapTotalCycles = 10,
    eLapTotalCalories = 11,
    eLapTotalFatCalories = 12,
    eLapAvgSpeed = 13,
    eLapMaxSpeed = 14,
    eLapAvgHeartRate = 15,
    eLapMaxHeartRate = 16,
    eLapAvgCadence = 17,
    eLapMaxCadence = 18,
    eLapAvgPower = 19,
    eLapMaxPower = 20,
    eLapTotalAscent = 21,
    eLapTotalDescent = 22,
    eLapIntensity = 23,
    eLapLapTrigger = 24,
    eLapSport = 25,
    eLapEventGroup = 26,
    eLapNumLengths = 32,
    eLapNormalizedPower = 33,
    eLapLeftRightBalance = 34,
    eLapFirstLengthIndex = 35,
    eLapAvgStrokeDistance = 37,
    eLapSwimStroke = 38,
    eLapSubSport = 39,
    eLapNumActiveLengths = 40,
    eLapTotalWork = 41,
    eLapAvgAltitude = 42,
    eLapMaxAltitude = 43,
    eLapGpsAccuracy = 44,
    eLapAvgGrade = 45,
    eLapAvgPosGrade = 46,
    eLapAvgNegGrade = 47,
    eLapMaxPosGrade = 48,
    eLapMaxNegGrade = 49,
    eLapAvgTemperature = 50,
    eLapMaxTemperature = 51,
    eLapTotalMovingTime = 52,
    eLapAvgPosVerticalSpeed = 53,
    eLapAvgNegVerticalSpeed = 54,
    eLapMaxPosVerticalSpeed = 55,
    eLapMaxNegVerticalSpeed = 56,
    eLapTimeInHrZone = 57,
    eLapTimeInSpeedZone = 58,
    eLapTimeInCadenceZone = 59,
    eLapTimeInPowerZone = 60,
    eLapRepetitionNum = 61,
    eLapMinAltitude = 62,
    eLapMinHeartRate = 63,
    eLapWktStepIndex = 71,
    eLapOpponentScore = 74,
    eLapStrokeCount = 75,
    eLapZoneCount = 76,
    eLapAvgVerticalOscillation = 77,
    eLapAvgStanceTimePercent = 78,
    eLapAvgStanceTime = 79,
    eLapAvgFractionalCadence = 80,
    eLapMaxFractionalCadence = 81,
    eLapTotalFractionalCycles = 82,
    eLapPlayerScore = 83,
    eLapAvgTotalHemoglobinConc = 84,
    eLapMinTotalHemoglobinConc = 85,
    eLapMaxTotalHemoglobinConc = 86,
    eLapAvgSaturatedHemoglobinPercent = 87,
    eLapMinSaturatedHemoglobinPercent = 88,
    eLapMaxSaturatedHemoglobinPercent = 89,
    eLapAvgLeftTorqueEffectiveness = 91,
    eLapAvgRightTorqueEffectiveness = 92,
    eLapAvgLeftPedalSmoothness = 93,
    eLapAvgRightPedalSmoothness = 94,
    eLapAvgCombinedPedalSmoothness = 95,
    eLapTimeStanding = 98,
    eLapStandCount = 99,
    eLapAvgLeftPco = 100,
    eLapAvgRightPco = 101,
    eLapAvgLeftPowerPhase = 102,
    eLapAvgLeftPowerPhasePeak = 103,
    eLapAvgRightPowerPhase = 104,
    eLapAvgRightPowerPhasePeak = 105,
    eLapAvgPowerPosition = 106,
    eLapMaxPowerPosition = 107,
    eLapAvgCadencePosition = 108,
    eLapMaxCadencePosition = 109,
    eLapEnhancedAvgSpeed = 110,
    eLapEnhancedMaxSpeed = 111,
    eLapEnhancedAvgAltitude = 112,
    eLapEnhancedMinAltitude = 113,
    eLapEnhancedMaxAltitude = 114,
    eLapAvgLevMotorPower = 115,
    eLapMaxLevMotorPower = 116,
    eLapLevBatteryConsumption = 117,
    eLapAvgVerticalRatio = 118,
    eLapAvgStanceTimeBalance = 119,
    eLapAvgStepLength = 120,
    eLapAvgVam = 121
} message_lap_e;

typedef enum {
    eLengthMessageIndex = 254,
    eLengthTimestamp = 253,
    eLengthEvent = 0,
    eLengthEventType = 1,
    eLengthStartTime = 2,
    eLengthTotalElapsedTime = 3,
    eLengthTotalTimerTime = 4,
    eLengthTotalStrokes = 5,
    eLengthAvgSpeed = 6,
    eLengthSwimStroke = 7,
    eLengthAvgSwimmingCadence = 9,
    eLengthEventGroup = 10,
    eLengthTotalCalories = 11,
    eLengthLengthType = 12,
    eLengthPlayerScore = 18,
    eLengthOpponentScore = 19,
    eLengthStrokeCount = 20,
    eLengthZoneCount = 21
} message_length_e;

typedef enum {
    eRecordTimestamp = 253,
    eRecordPositionLat = 0,
    eRecordPositionLong = 1,
    eRecordAltitude = 2,
    eRecordHeartRate = 3,
    eRecordCadence = 4,
    eRecordDistance = 5,
    eRecordSpeed = 6,
    eRecordPower = 7,
    eRecordCompressedSpeedDistance = 8,
    eRecordGrade = 9,
    eRecordResistance = 10,
    eRecordTimeFromCourse = 11,
    eRecordCycleLength = 12,
    eRecordTemperature = 13,
    eRecordSpeed1S = 17,
    eRecordCycles = 18,
    eRecordTotalCycles = 19,
    eRecordCompressedAccumulatedPower = 28,
    eRecordAccumulatedPower = 29,
    eRecordLeftRightBalance = 30,
    eRecordGpsAccuracy = 31,
    eRecordVerticalSpeed = 32,
    eRecordCalories = 33,
    eRecordVerticalOscillation = 39,
    eRecordStanceTimePercent = 40,
    eRecordStanceTime = 41,
    eRecordActivityType = 42,
    eRecordLeftTorqueEffectiveness = 43,
    eRecordRightTorqueEffectiveness = 44,
    eRecordLeftPedalSmoothness = 45,
    eRecordRightPedalSmoothness = 46,
    eRecordCombinedPedalSmoothness = 47,
    eRecordTime128 = 48,
    eRecordStrokeType = 49,
    eRecordZone = 50,
    eRecordBallSpeed = 51,
    eRecordCadence256 = 52,
    eRecordFractionalCadence = 53,
    eRecordTotalHemoglobinConc = 54,
    eRecordTotalHemoglobinConcMin = 55,
    eRecordTotalHemoglobinConcMax = 56,
    eRecordSaturatedHemoglobinPercent = 57,
    eRecordSaturatedHemoglobinPercentMin = 58,
    eRecordSaturatedHemoglobinPercentMax = 59,
    eRecordDeviceIndex = 62,
    eRecordLeftPco = 67,
    eRecordRightPco = 68,
    eRecordLeftPowerPhase = 69,
    eRecordLeftPowerPhasePeak = 70,
    eRecordRightPowerPhase = 71,
    eRecordRightPowerPhasePeak = 72,
    eRecordEnhancedSpeed = 73,
    eRecordEnhancedAltitude = 78,
    eRecordBatterySoc = 81,
    eRecordMotorPower = 82,
    eRecordVerticalRatio = 83,
    eRecordStanceTimeBalance = 84,
    eRecordStepLength = 85
} message_record_e;

typedef enum {
    eEventTimestamp = 253,
    eEventEvent = 0,
    eEventEventType = 1,
    eEventData16 = 2,
    eEventData = 3,
    eEventEventGroup = 4,
    eEventScore = 7,
    eEventOpponentScore = 8,
    eEventFrontGearNum = 9,
    eEventFrontGear = 10,
    eEventRearGearNum = 11,
    eEventRearGear = 12,
    eEventDeviceIndex = 13
} message_event_e;

typedef enum {
    eDeviceInfoTimestamp = 253,
    eDeviceInfoDeviceIndex = 0,
    eDeviceInfoDeviceType = 1,
    eDeviceInfoManufacturer = 2,
    eDeviceInfoSerialNumber = 3,
    eDeviceInfoProduct = 4,
    eDeviceInfoSoftwareVersion = 5,
    eDeviceInfoHardwareVersion = 6,
    eDeviceInfoCumOperatingTime = 7,
    eDeviceInfoBatteryVoltage = 10,
    eDeviceInfoBatteryStatus = 11,
    eDeviceInfoSensorPosition = 18,
    eDeviceInfoDescriptor = 19,
    eDeviceInfoAntTransmissionType = 20,
    eDeviceInfoAntDeviceNumber = 21,
    eDeviceInfoAntNetwork = 22,
    eDeviceInfoSourceType = 25,
    eDeviceInfoProductName = 27
} message_device_info_e;

typedef enum {
    eTrainingFileTimestamp = 253,
    eTrainingFileType = 0,
    eTrainingFileManufacturer = 1,
    eTrainingFileProduct = 2,
    eTrainingFileSerialNumber = 3,
    eTrainingFileTimeCreated = 4
} message_training_file_e;

typedef enum {
    eHrvTime = 0
} message_hrv_e;

typedef enum {
    eWeatherConditionsTimestamp = 253,
    eWeatherConditionsWeatherReport = 0,
    eWeatherConditionsTemperature = 1,
    eWeatherConditionsCondition = 2,
    eWeatherConditionsWindDirection = 3,
    eWeatherConditionsWindSpeed = 4,
    eWeatherConditionsPrecipitationProbability = 5,
    eWeatherConditionsTemperatureFeelsLike = 6,
    eWeatherConditionsRelativeHumidity = 7,
    eWeatherConditionsLocation = 8,
    eWeatherConditionsObservedAtTime = 9,
    eWeatherConditionsObservedLocationLat = 10,
    eWeatherConditionsObservedLocationLong = 11,
    eWeatherConditionsDayOfWeek = 12,
    eWeatherConditionsHighTemperature = 13,
    eWeatherConditionsLowTemperature = 14
} message_weather_conditions_e;

typedef enum {
    eWeatherAlertTimestamp = 253,
    eWeatherAlertReportId = 0,
    eWeatherAlertIssueTime = 1,
    eWeatherAlertExpireTime = 2,
    eWeatherAlertSeverity = 3,
    eWeatherAlertType = 4
} message_weather_alert_e;

typedef enum {
    eGpsMetadataTimestamp = 253,
    eGpsMetadataTimestampMs = 0,
    eGpsMetadataPositionLat = 1,
    eGpsMetadataPositionLong = 2,
    eGpsMetadataEnhancedAltitude = 3,
    eGpsMetadataEnhancedSpeed = 4,
    eGpsMetadataHeading = 5,
    eGpsMetadataUtcTimestamp = 6,
    eGpsMetadataVelocity = 7
} message_gps_metadata_e;

typedef enum {
    eCameraEventTimestamp = 253,
    eCameraEventTimestampMs = 0,
    eCameraEventCameraEventType = 1,
    eCameraEventCameraFileUuid = 2,
    eCameraEventCameraOrientation = 3
} message_camera_event_e;

typedef enum {
    eGyroscopeDataTimestamp = 253,
    eGyroscopeDataTimestampMs = 0,
    eGyroscopeDataSampleTimeOffset = 1,
    eGyroscopeDataGyroX = 2,
    eGyroscopeDataGyroY = 3,
    eGyroscopeDataGyroZ = 4,
    eGyroscopeDataCalibratedGyroX = 5,
    eGyroscopeDataCalibratedGyroY = 6,
    eGyroscopeDataCalibratedGyroZ = 7
} message_gyroscope_data_e;

typedef enum {
    eAccelerometerDataTimestamp = 253,
    eAccelerometerDataTimestampMs = 0,
    eAccelerometerDataSampleTimeOffset = 1,
    eAccelerometerDataAccelX = 2,
    eAccelerometerDataAccelY = 3,
    eAccelerometerDataAccelZ = 4,
    eAccelerometerDataCalibratedAccelX = 5,
    eAccelerometerDataCalibratedAccelY = 6,
    eAccelerometerDataCalibratedAccelZ = 7,
    eAccelerometerDataCompressedCalibratedAccelX = 8,
    eAccelerometerDataCompressedCalibratedAccelY = 9,
    eAccelerometerDataCompressedCalibratedAccelZ = 10
} message_accelerometer_data_e;

typedef enum {
    eMagnetometerDataTimestamp = 253,
    eMagnetometerDataTimestampMs = 0,
    eMagnetometerDataSampleTimeOffset = 1,
    eMagnetometerDataMagX = 2,
    eMagnetometerDataMagY = 3,
    eMagnetometerDataMagZ = 4,
    eMagnetometerDataCalibratedMagX = 5,
    eMagnetometerDataCalibratedMagY = 6,
    eMagnetometerDataCalibratedMagZ = 7
} message_magnetometer_data_e;

typedef enum {
    eThreeDSensorCalibrationTimestamp = 253,
    eThreeDSensorCalibrationSensorType = 0,
    eThreeDSensorCalibrationCalibrationFactor = 1,
    eThreeDSensorCalibrationCalibrationDivisor = 2,
    eThreeDSensorCalibrationLevelShift = 3,
    eThreeDSensorCalibrationOffsetCal = 4,
    eThreeDSensorCalibrationOrientationMatrix = 5
} message_three_d_sensor_calibration_e;

typedef enum {
    eVideoFrameTimestamp = 253,
    eVideoFrameTimestampMs = 0,
    eVideoFrameFrameNumber = 1
} message_video_frame_e;

typedef enum {
    eObdiiDataTimestamp = 253,
    eObdiiDataTimestampMs = 0,
    eObdiiDataTimeOffset = 1,
    eObdiiDataPid = 2,
    eObdiiDataRawData = 3,
    eObdiiDataPidDataSize = 4,
    eObdiiDataSystemTime = 5,
    eObdiiDataStartTimestamp = 6,
    eObdiiDataStartTimestampMs = 7
} message_obdii_data_e;

typedef enum {
    eNmeaSentenceTimestamp = 253,
    eNmeaSentenceTimestampMs = 0,
    eNmeaSentenceSentence = 1
} message_nmea_sentence_e;

typedef enum {
    eAviationAttitudeTimestamp = 253,
    eAviationAttitudeTimestampMs = 0,
    eAviationAttitudeSystemTime = 1,
    eAviationAttitudePitch = 2,
    eAviationAttitudeRoll = 3,
    eAviationAttitudeAccelLateral = 4,
    eAviationAttitudeAccelNormal = 5,
    eAviationAttitudeTurnRate = 6,
    eAviationAttitudeStage = 7,
    eAviationAttitudeAttitudeStageComplete = 8,
    eAviationAttitudeTrack = 9,
    eAviationAttitudeValidity = 10
} message_aviation_attitude_e;

typedef enum {
    eVideoUrl = 0,
    eVideoHostingProvider = 1,
    eVideoDuration = 2
} message_video_e;

typedef enum {
    eVideoTitleMessageIndex = 254,
    eVideoTitleMessageCount = 0,
    eVideoTitleText = 1
} message_video_title_e;

typedef enum {
    eVideoDescriptionMessageIndex = 254,
    eVideoDescriptionMessageCount = 0,
    eVideoDescriptionText = 1
} message_video_description_e;

typedef enum {
    eVideoClipClipNumber = 0,
    eVideoClipStartTimestamp = 1,
    eVideoClipStartTimestampMs = 2,
    eVideoClipEndTimestamp = 3,
    eVideoClipEndTimestampMs = 4,
    eVideoClipClipStart = 6,
    eVideoClipClipEnd = 7
} message_video_clip_e;

typedef enum {
    eCourseSport = 4,
    eCourseName = 5,
    eCourseCapabilities = 6,
    eCourseSubSport = 7
} message_course_e;

typedef enum {
    eCoursePointMessageIndex = 254,
    eCoursePointTimestamp = 1,
    eCoursePointPositionLat = 2,
    eCoursePointPositionLong = 3,
    eCoursePointDistance = 4,
    eCoursePointType = 5,
    eCoursePointName = 6,
    eCoursePointFavorite = 8
} message_course_point_e;

typedef enum {
    eSegmentIdName = 0,
    eSegmentIdUuid = 1,
    eSegmentIdSport = 2,
    eSegmentIdEnabled = 3,
    eSegmentIdUserProfilePrimaryKey = 4,
    eSegmentIdDeviceId = 5,
    eSegmentIdDefaultRaceLeader = 6,
    eSegmentIdDeleteStatus = 7,
    eSegmentIdSelectionType = 8
} message_segment_id_e;

typedef enum {
    eSegmentLeaderboardEntryMessageIndex = 254,
    eSegmentLeaderboardEntryName = 0,
    eSegmentLeaderboardEntryType = 1,
    eSegmentLeaderboardEntryGroupPrimaryKey = 2,
    eSegmentLeaderboardEntryActivityId = 3,
    eSegmentLeaderboardEntrySegmentTime = 4,
    eSegmentLeaderboardEntryActivityIdString = 5
} message_segment_leaderboard_entry_e;

typedef enum {
    eSegmentPointMessageIndex = 254,
    eSegmentPointPositionLat = 1,
    eSegmentPointPositionLong = 2,
    eSegmentPointDistance = 3,
    eSegmentPointAltitude = 4,
    eSegmentPointLeaderTime = 5
} message_segment_point_e;

typedef enum {
    eSegmentLapMessageIndex = 254,
    eSegmentLapTimestamp = 253,
    eSegmentLapEvent = 0,
    eSegmentLapEventType = 1,
    eSegmentLapStartTime = 2,
    eSegmentLapStartPositionLat = 3,
    eSegmentLapStartPositionLong = 4,
    eSegmentLapEndPositionLat = 5,
    eSegmentLapEndPositionLong = 6,
    eSegmentLapTotalElapsedTime = 7,
    eSegmentLapTotalTimerTime = 8,
    eSegmentLapTotalDistance = 9,
    eSegmentLapTotalCycles = 10,
    eSegmentLapTotalCalories = 11,
    eSegmentLapTotalFatCalories = 12,
    eSegmentLapAvgSpeed = 13,
    eSegmentLapMaxSpeed = 14,
    eSegmentLapAvgHeartRate = 15,
    eSegmentLapMaxHeartRate = 16,
    eSegmentLapAvgCadence = 17,
    eSegmentLapMaxCadence = 18,
    eSegmentLapAvgPower = 19,
    eSegmentLapMaxPower = 20,
    eSegmentLapTotalAscent = 21,
    eSegmentLapTotalDescent = 22,
    eSegmentLapSport = 23,
    eSegmentLapEventGroup = 24,
    eSegmentLapNecLat = 25,
    eSegmentLapNecLong = 26,
    eSegmentLapSwcLat = 27,
    eSegmentLapSwcLong = 28,
    eSegmentLapName = 29,
    eSegmentLapNormalizedPower = 30,
    eSegmentLapLeftRightBalance = 31,
    eSegmentLapSubSport = 32,
    eSegmentLapTotalWork = 33,
    eSegmentLapAvgAltitude = 34,
    eSegmentLapMaxAltitude = 35,
    eSegmentLapGpsAccuracy = 36,
    eSegmentLapAvgGrade = 37,
    eSegmentLapAvgPosGrade = 38,
    eSegmentLapAvgNegGrade = 39,
    eSegmentLapMaxPosGrade = 40,
    eSegmentLapMaxNegGrade = 41,
    eSegmentLapAvgTemperature = 42,
    eSegmentLapMaxTemperature = 43,
    eSegmentLapTotalMovingTime = 44,
    eSegmentLapAvgPosVerticalSpeed = 45,
    eSegmentLapAvgNegVerticalSpeed = 46,
    eSegmentLapMaxPosVerticalSpeed = 47,
    eSegmentLapMaxNegVerticalSpeed = 48,
    eSegmentLapTimeInHrZone = 49,
    eSegmentLapTimeInSpeedZone = 50,
    eSegmentLapTimeInCadenceZone = 51,
    eSegmentLapTimeInPowerZone = 52,
    eSegmentLapRepetitionNum = 53,
    eSegmentLapMinAltitude = 54,
    eSegmentLapMinHeartRate = 55,
    eSegmentLapActiveTime = 56,
    eSegmentLapWktStepIndex = 57,
    eSegmentLapSportEvent = 58,
    eSegmentLapAvgLeftTorqueEffectiveness = 59,
    eSegmentLapAvgRightTorqueEffectiveness = 60,
    eSegmentLapAvgLeftPedalSmoothness = 61,
    eSegmentLapAvgRightPedalSmoothness = 62,
    eSegmentLapAvgCombinedPedalSmoothness = 63,
    eSegmentLapStatus = 64,
    eSegmentLapUuid = 65,
    eSegmentLapAvgFractionalCadence = 66,
    eSegmentLapMaxFractionalCadence = 67,
    eSegmentLapTotalFractionalCycles = 68,
    eSegmentLapFrontGearShiftCount = 69,
    eSegmentLapRearGearShiftCount = 70,
    eSegmentLapTimeStanding = 71,
    eSegmentLapStandCount = 72,
    eSegmentLapAvgLeftPco = 73,
    eSegmentLapAvgRightPco = 74,
    eSegmentLapAvgLeftPowerPhase = 75,
    eSegmentLapAvgLeftPowerPhasePeak = 76,
    eSegmentLapAvgRightPowerPhase = 77,
    eSegmentLapAvgRightPowerPhasePeak = 78,
    eSegmentLapAvgPowerPosition = 79,
    eSegmentLapMaxPowerPosition = 80,
    eSegmentLapAvgCadencePosition = 81,
    eSegmentLapMaxCadencePosition = 82,
    eSegmentLapManufacturer = 83
} message_segment_lap_e;

typedef enum {
    eSegmentFileMessageIndex = 254,
    eSegmentFileFileUuid = 1,
    eSegmentFileEnabled = 3,
    eSegmentFileUserProfilePrimaryKey = 4,
    eSegmentFileLeaderType = 7,
    eSegmentFileLeaderGroupPrimaryKey = 8,
    eSegmentFileLeaderActivityId = 9,
    eSegmentFileLeaderActivityIdString = 10,
    eSegmentFileDefaultRaceLeader = 11
} message_segment_file_e;

typedef enum {
    eWorkoutSport = 4,
    eWorkoutCapabilities = 5,
    eWorkoutNumValidSteps = 6,
    eWorkoutWktName = 8
} message_workout_e;

typedef enum {
    eWorkoutStepMessageIndex = 254,
    eWorkoutStepWktStepName = 0,
    eWorkoutStepDurationType = 1,
    eWorkoutStepDurationValue = 2,
    eWorkoutStepTargetType = 3,
    eWorkoutStepTargetValue = 4,
    eWorkoutStepCustomTargetValueLow = 5,
    eWorkoutStepCustomTargetValueHigh = 6,
    eWorkoutStepIntensity = 7,
    eWorkoutStepNotes = 8
} message_workout_step_e;

typedef enum {
    eScheduleManufacturer = 0,
    eScheduleProduct = 1,
    eScheduleSerialNumber = 2,
    eScheduleTimeCreated = 3,
    eScheduleCompleted = 4,
    eScheduleType = 5,
    eScheduleScheduledTime = 6
} message_schedule_e;

typedef enum {
    eTotalsMessageIndex = 254,
    eTotalsTimestamp = 253,
    eTotalsTimerTime = 0,
    eTotalsDistance = 1,
    eTotalsCalories = 2,
    eTotalsSport = 3,
    eTotalsElapsedTime = 4,
    eTotalsSessions = 5,
    eTotalsActiveTime = 6,
    eTotalsSportIndex = 9
} message_totals_e;

typedef enum {
    eWeightScaleTimestamp = 253,
    eWeightScaleWeight = 0,
    eWeightScalePercentFat = 1,
    eWeightScalePercentHydration = 2,
    eWeightScaleVisceralFatMass = 3,
    eWeightScaleBoneMass = 4,
    eWeightScaleMuscleMass = 5,
    eWeightScaleBasalMet = 7,
    eWeightScalePhysiqueRating = 8,
    eWeightScaleActiveMet = 9,
    eWeightScaleMetabolicAge = 10,
    eWeightScaleVisceralFatRating = 11,
    eWeightScaleUserProfileIndex = 12
} message_weight_scale_e;

typedef enum {
    eBloodPressureTimestamp = 253,
    eBloodPressureSystolicPressure = 0,
    eBloodPressureDiastolicPressure = 1,
    eBloodPressureMeanArterialPressure = 2,
    eBloodPressureMap3SampleMean = 3,
    eBloodPressureMapMorningValues = 4,
    eBloodPressureMapEveningValues = 5,
    eBloodPressureHeartRate = 6,
    eBloodPressureHeartRateType = 7,
    eBloodPressureStatus = 8,
    eBloodPressureUserProfileIndex = 9
} message_blood_pressure_e;

typedef enum {
    eMonitoringInfoTimestamp = 253,
    eMonitoringInfoLocalTimestamp = 0,
    eMonitoringInfoActivityType = 1,
    eMonitoringInfoCyclesToDistance = 3,
    eMonitoringInfoCyclesToCalories = 4,
    eMonitoringInfoRestingMetabolicRate = 5
} message_monitoring_info_e;

typedef enum {
    eMonitoringTimestamp = 253,
    eMonitoringDeviceIndex = 0,
    eMonitoringCalories = 1,
    eMonitoringDistance = 2,
    eMonitoringCycles = 3,
    eMonitoringActiveTime = 4,
    eMonitoringActivityType = 5,
    eMonitoringActivitySubtype = 6,
    eMonitoringActivityLevel = 7,
    eMonitoringDistance16 = 8,
    eMonitoringCycles16 = 9,
    eMonitoringActiveTime16 = 10,
    eMonitoringLocalTimestamp = 11,
    eMonitoringTemperature = 12,
    eMonitoringTemperatureMin = 14,
    eMonitoringTemperatureMax = 15,
    eMonitoringActivityTime = 16,
    eMonitoringActiveCalories = 19,
    eMonitoringCurrentActivityTypeIntensity = 24,
    eMonitoringTimestampMin8 = 25,
    eMonitoringTimestamp16 = 26,
    eMonitoringHeartRate = 27,
    eMonitoringIntensity = 28,
    eMonitoringDurationMin = 29,
    eMonitoringDuration = 30,
    eMonitoringAscent = 31,
    eMonitoringDescent = 32,
    eMonitoringModerateActivityMinutes = 33,
    eMonitoringVigorousActivityMinutes = 34
} message_monitoring_e;

typedef enum {
    eHrTimestamp = 253,
    eHrFractionalTimestamp = 0,
    eHrTime256 = 1,
    eHrFilteredBpm = 6,
    eHrEventTimestamp = 9,
    eHrEventTimestamp12 = 10
} message_hr_e;

typedef enum {
    eMemoGlobPartIndex = 250,
    eMemoGlobMemo = 0,
    eMemoGlobMessageNumber = 1,
    eMemoGlobMessageIndex = 2
} message_memo_glob_e;

typedef enum {
    eAntChannelIdChannelNumber = 0,
    eAntChannelIdDeviceType = 1,
    eAntChannelIdDeviceNumber = 2,
    eAntChannelIdTransmissionType = 3,
    eAntChannelIdDeviceIndex = 4
} message_ant_channel_id_e;

typedef enum {
    eAntRxTimestamp = 253,
    eAntRxFractionalTimestamp = 0,
    eAntRxMesgId = 1,
    eAntRxMesgData = 2,
    eAntRxChannelNumber = 3,
    eAntRxData = 4
} message_ant_rx_e;

typedef enum {
    eAntTxTimestamp = 253,
    eAntTxFractionalTimestamp = 0,
    eAntTxMesgId = 1,
    eAntTxMesgData = 2,
    eAntTxChannelNumber = 3,
    eAntTxData = 4
} message_ant_tx_e;

typedef enum {
    eExdScreenConfigurationScreenIndex = 0,
    eExdScreenConfigurationFieldCount = 1,
    eExdScreenConfigurationLayout = 2,
    eExdScreenConfigurationScreenEnabled = 3
} message_exd_screen_configuration_e;

typedef enum {
    eExdDataFieldConfigurationScreenIndex = 0,
    eExdDataFieldConfigurationConceptField = 1,
    eExdDataFieldConfigurationFieldId = 2,
    eExdDataFieldConfigurationConceptCount = 3,
    eExdDataFieldConfigurationDisplayType = 4,
    eExdDataFieldConfigurationTitle = 5
} message_exd_data_field_configuration_e;

typedef enum {
    eExdDataConceptConfigurationScreenIndex = 0,
    eExdDataConceptConfigurationConceptField = 1,
    eExdDataConceptConfigurationFieldId = 2,
    eExdDataConceptConfigurationConceptIndex = 3,
    eExdDataConceptConfigurationDataPage = 4,
    eExdDataConceptConfigurationConceptKey = 5,
    eExdDataConceptConfigurationScaling = 6,
    eExdDataConceptConfigurationDataUnits = 8,
    eExdDataConceptConfigurationQualifier = 9,
    eExdDataConceptConfigurationDescriptor = 10,
    eExdDataConceptConfigurationIsSigned = 11
} message_exd_data_concept_configuration_e;

typedef enum {
    eFieldDescriptionDeveloperDataIndex = 0,
    eFieldDescriptionFieldDefinitionNumber = 1,
    eFieldDescriptionFitBaseTypeId = 2,
    eFieldDescriptionFieldName = 3,
    eFieldDescriptionArray = 4,
    eFieldDescriptionComponents = 5,
    eFieldDescriptionScale = 6,
    eFieldDescriptionOffset = 7,
    eFieldDescriptionUnits = 8,
    eFieldDescriptionBits = 9,
    eFieldDescriptionAccumulate = 10,
    eFieldDescriptionFitBaseUnitId = 13,
    eFieldDescriptionNativeMesgNum = 14,
    eFieldDescriptionNativeFieldNum = 15
} message_field_description_e;

typedef enum {
    eDeveloperDataIdDeveloperId = 0,
    eDeveloperDataIdApplicationId = 1,
    eDeveloperDataIdManufacturerId = 2,
    eDeveloperDataIdDeveloperDataIndex = 3,
    eDeveloperDataIdApplicationVersion = 4
} message_developer_data_id_e;
// ----------- end generated code -----------
#endif // FIT_FIELDS_H
