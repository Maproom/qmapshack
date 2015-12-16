/**********************************************************************************************
 Copyright (C) 2014 Oliver Eichler oliver.eichler@gmx.de

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
 FileIdType = 0,
 FileIdManufacturer = 1,
 FileIdProduct = 2,
 FileIdSerialNumber = 3,
 FileIdTimeCreated = 4,
 FileIdNumber = 5,
 FileIdProductName = 8
} FileId;

typedef enum {
 FileCreatorSoftwareVersion = 0,
 FileCreatorHardwareVersion = 1
} FileCreator;

typedef enum {
 TimestampCorrelationTimestamp = 253,
 TimestampCorrelationFractionalTimestamp = 0,
 TimestampCorrelationSystemTimestamp = 1,
 TimestampCorrelationFractionalSystemTimestamp = 2,
 TimestampCorrelationLocalTimestamp = 3,
 TimestampCorrelationTimestampMs = 4,
 TimestampCorrelationSystemTimestampMs = 5
} TimestampCorrelation;

typedef enum {
 SoftwareMessageIndex = 254,
 SoftwareVersion = 3,
 SoftwarePartNumber = 5
} Software;

typedef enum {
 SlaveDeviceManufacturer = 0,
 SlaveDeviceProduct = 1
} SlaveDevice;

typedef enum {
 CapabilitiesLanguages = 0,
 CapabilitiesSports = 1,
 CapabilitiesWorkoutsSupported = 21,
 CapabilitiesConnectivitySupported = 23
} Capabilities;

typedef enum {
 FileCapabilitiesMessageIndex = 254,
 FileCapabilitiesType = 0,
 FileCapabilitiesFlags = 1,
 FileCapabilitiesDirectory = 2,
 FileCapabilitiesMaxCount = 3,
 FileCapabilitiesMaxSize = 4
} FileCapabilities;

typedef enum {
 MesgCapabilitiesMessageIndex = 254,
 MesgCapabilitiesFile = 0,
 MesgCapabilitiesMesgNum = 1,
 MesgCapabilitiesCountType = 2,
 MesgCapabilitiesCount = 3
} MesgCapabilities;

typedef enum {
 FieldCapabilitiesMessageIndex = 254,
 FieldCapabilitiesFile = 0,
 FieldCapabilitiesMesgNum = 1,
 FieldCapabilitiesFieldNum = 2,
 FieldCapabilitiesCount = 3
} FieldCapabilities;

typedef enum {
 DeviceSettingsActiveTimeZone = 0,
 DeviceSettingsUtcOffset = 1,
 DeviceSettingsTimeZoneOffset = 5
} DeviceSettings;

typedef enum {
 UserProfileMessageIndex = 254,
 UserProfileFriendlyName = 0,
 UserProfileGender = 1,
 UserProfileAge = 2,
 UserProfileHeight = 3,
 UserProfileWeight = 4,
 UserProfileLanguage = 5,
 UserProfileElevSetting = 6,
 UserProfileWeightSetting = 7,
 UserProfileRestingHeartRate = 8,
 UserProfileDefaultMaxRunningHeartRate = 9,
 UserProfileDefaultMaxBikingHeartRate = 10,
 UserProfileDefaultMaxHeartRate = 11,
 UserProfileHrSetting = 12,
 UserProfileSpeedSetting = 13,
 UserProfileDistSetting = 14,
 UserProfilePowerSetting = 16,
 UserProfileActivityClass = 17,
 UserProfilePositionSetting = 18,
 UserProfileTemperatureSetting = 21,
 UserProfileLocalId = 22,
 UserProfileGlobalId = 23,
 UserProfileHeightSetting = 30
} UserProfile;

typedef enum {
 HrmProfileMessageIndex = 254,
 HrmProfileEnabled = 0,
 HrmProfileHrmAntId = 1,
 HrmProfileLogHrv = 2,
 HrmProfileHrmAntIdTransType = 3
} HrmProfile;

typedef enum {
 SdmProfileMessageIndex = 254,
 SdmProfileEnabled = 0,
 SdmProfileSdmAntId = 1,
 SdmProfileSdmCalFactor = 2,
 SdmProfileOdometer = 3,
 SdmProfileSpeedSource = 4,
 SdmProfileSdmAntIdTransType = 5,
 SdmProfileOdometerRollover = 7
} SdmProfile;

typedef enum {
 BikeProfileMessageIndex = 254,
 BikeProfileName = 0,
 BikeProfileSport = 1,
 BikeProfileSubSport = 2,
 BikeProfileOdometer = 3,
 BikeProfileBikeSpdAntId = 4,
 BikeProfileBikeCadAntId = 5,
 BikeProfileBikeSpdcadAntId = 6,
 BikeProfileBikePowerAntId = 7,
 BikeProfileCustomWheelsize = 8,
 BikeProfileAutoWheelsize = 9,
 BikeProfileBikeWeight = 10,
 BikeProfilePowerCalFactor = 11,
 BikeProfileAutoWheelCal = 12,
 BikeProfileAutoPowerZero = 13,
 BikeProfileId = 14,
 BikeProfileSpdEnabled = 15,
 BikeProfileCadEnabled = 16,
 BikeProfileSpdcadEnabled = 17,
 BikeProfilePowerEnabled = 18,
 BikeProfileCrankLength = 19,
 BikeProfileEnabled = 20,
 BikeProfileBikeSpdAntIdTransType = 21,
 BikeProfileBikeCadAntIdTransType = 22,
 BikeProfileBikeSpdcadAntIdTransType = 23,
 BikeProfileBikePowerAntIdTransType = 24,
 BikeProfileOdometerRollover = 37,
 BikeProfileFrontGearNum = 38,
 BikeProfileFrontGear = 39,
 BikeProfileRearGearNum = 40,
 BikeProfileRearGear = 41,
 BikeProfileShimanoDi2Enabled = 44
} BikeProfile;

typedef enum {
 ZonesTargetMaxHeartRate = 1,
 ZonesTargetThresholdHeartRate = 2,
 ZonesTargetFunctionalThresholdPower = 3,
 ZonesTargetHrCalcType = 5,
 ZonesTargetPwrCalcType = 7
} ZonesTarget;

typedef enum {
 SportSport = 0,
 SportSubSport = 1,
 SportName = 3
} Sport;

typedef enum {
 HrZoneMessageIndex = 254,
 HrZoneHighBpm = 1,
 HrZoneName = 2
} HrZone;

typedef enum {
 SpeedZoneMessageIndex = 254,
 SpeedZoneHighValue = 0,
 SpeedZoneName = 1
} SpeedZone;

typedef enum {
 CadenceZoneMessageIndex = 254,
 CadenceZoneHighValue = 0,
 CadenceZoneName = 1
} CadenceZone;

typedef enum {
 PowerZoneMessageIndex = 254,
 PowerZoneHighValue = 1,
 PowerZoneName = 2
} PowerZone;

typedef enum {
 MetZoneMessageIndex = 254,
 MetZoneHighBpm = 1,
 MetZoneCalories = 2,
 MetZoneFatCalories = 3
} MetZone;

typedef enum {
 GoalMessageIndex = 254,
 GoalSport = 0,
 GoalSubSport = 1,
 GoalStartDate = 2,
 GoalEndDate = 3,
 GoalType = 4,
 GoalValue = 5,
 GoalRepeat = 6,
 GoalTargetValue = 7,
 GoalRecurrence = 8,
 GoalRecurrenceValue = 9,
 GoalEnabled = 10
} Goal;

typedef enum {
 ActivityTimestamp = 253,
 ActivityTotalTimerTime = 0,
 ActivityNumSessions = 1,
 ActivityType = 2,
 ActivityEvent = 3,
 ActivityEventType = 4,
 ActivityLocalTimestamp = 5,
 ActivityEventGroup = 6
} Activity;

typedef enum {
 SessionMessageIndex = 254,
 SessionTimestamp = 253,
 SessionEvent = 0,
 SessionEventType = 1,
 SessionStartTime = 2,
 SessionStartPositionLat = 3,
 SessionStartPositionLong = 4,
 SessionSport = 5,
 SessionSubSport = 6,
 SessionTotalElapsedTime = 7,
 SessionTotalTimerTime = 8,
 SessionTotalDistance = 9,
 SessionTotalCycles = 10,
 SessionTotalCalories = 11,
 SessionTotalFatCalories = 13,
 SessionAvgSpeed = 14,
 SessionMaxSpeed = 15,
 SessionAvgHeartRate = 16,
 SessionMaxHeartRate = 17,
 SessionAvgCadence = 18,
 SessionMaxCadence = 19,
 SessionAvgPower = 20,
 SessionMaxPower = 21,
 SessionTotalAscent = 22,
 SessionTotalDescent = 23,
 SessionTotalTrainingEffect = 24,
 SessionFirstLapIndex = 25,
 SessionNumLaps = 26,
 SessionEventGroup = 27,
 SessionTrigger = 28,
 SessionNecLat = 29,
 SessionNecLong = 30,
 SessionSwcLat = 31,
 SessionSwcLong = 32,
 SessionNormalizedPower = 34,
 SessionTrainingStressScore = 35,
 SessionIntensityFactor = 36,
 SessionLeftRightBalance = 37,
 SessionAvgStrokeCount = 41,
 SessionAvgStrokeDistance = 42,
 SessionSwimStroke = 43,
 SessionPoolLength = 44,
 SessionThresholdPower = 45,
 SessionPoolLengthUnit = 46,
 SessionNumActiveLengths = 47,
 SessionTotalWork = 48,
 SessionAvgAltitude = 49,
 SessionMaxAltitude = 50,
 SessionGpsAccuracy = 51,
 SessionAvgGrade = 52,
 SessionAvgPosGrade = 53,
 SessionAvgNegGrade = 54,
 SessionMaxPosGrade = 55,
 SessionMaxNegGrade = 56,
 SessionAvgTemperature = 57,
 SessionMaxTemperature = 58,
 SessionTotalMovingTime = 59,
 SessionAvgPosVerticalSpeed = 60,
 SessionAvgNegVerticalSpeed = 61,
 SessionMaxPosVerticalSpeed = 62,
 SessionMaxNegVerticalSpeed = 63,
 SessionMinHeartRate = 64,
 SessionTimeInHrZone = 65,
 SessionTimeInSpeedZone = 66,
 SessionTimeInCadenceZone = 67,
 SessionTimeInPowerZone = 68,
 SessionAvgLapTime = 69,
 SessionBestLapIndex = 70,
 SessionMinAltitude = 71,
 SessionPlayerScore = 82,
 SessionOpponentScore = 83,
 SessionOpponentName = 84,
 SessionStrokeCount = 85,
 SessionZoneCount = 86,
 SessionMaxBallSpeed = 87,
 SessionAvgBallSpeed = 88,
 SessionAvgVerticalOscillation = 89,
 SessionAvgStanceTimePercent = 90,
 SessionAvgStanceTime = 91,
 SessionAvgFractionalCadence = 92,
 SessionMaxFractionalCadence = 93,
 SessionTotalFractionalCycles = 94,
 SessionAvgTotalHemoglobinConc = 95,
 SessionMinTotalHemoglobinConc = 96,
 SessionMaxTotalHemoglobinConc = 97,
 SessionAvgSaturatedHemoglobinPercent = 98,
 SessionMinSaturatedHemoglobinPercent = 99,
 SessionMaxSaturatedHemoglobinPercent = 100,
 SessionAvgLeftTorqueEffectiveness = 101,
 SessionAvgRightTorqueEffectiveness = 102,
 SessionAvgLeftPedalSmoothness = 103,
 SessionAvgRightPedalSmoothness = 104,
 SessionAvgCombinedPedalSmoothness = 105,
 SessionSportIndex = 111,
 SessionTimeStanding = 112,
 SessionStandCount = 113,
 SessionAvgLeftPco = 114,
 SessionAvgRightPco = 115,
 SessionAvgLeftPowerPhase = 116,
 SessionAvgLeftPowerPhasePeak = 117,
 SessionAvgRightPowerPhase = 118,
 SessionAvgRightPowerPhasePeak = 119,
 SessionAvgPowerPosition = 120,
 SessionMaxPowerPosition = 121,
 SessionAvgCadencePosition = 122,
 SessionMaxCadencePosition = 123,
 SessionEnhancedAvgSpeed = 124,
 SessionEnhancedMaxSpeed = 125,
 SessionEnhancedAvgAltitude = 126,
 SessionEnhancedMinAltitude = 127,
 SessionEnhancedMaxAltitude = 128,
 SessionAvgLevMotorPower = 129,
 SessionMaxLevMotorPower = 130,
 SessionLevBatteryConsumption = 131
} Session;

typedef enum {
 LapMessageIndex = 254,
 LapTimestamp = 253,
 LapEvent = 0,
 LapEventType = 1,
 LapStartTime = 2,
 LapStartPositionLat = 3,
 LapStartPositionLong = 4,
 LapEndPositionLat = 5,
 LapEndPositionLong = 6,
 LapTotalElapsedTime = 7,
 LapTotalTimerTime = 8,
 LapTotalDistance = 9,
 LapTotalCycles = 10,
 LapTotalCalories = 11,
 LapTotalFatCalories = 12,
 LapAvgSpeed = 13,
 LapMaxSpeed = 14,
 LapAvgHeartRate = 15,
 LapMaxHeartRate = 16,
 LapAvgCadence = 17,
 LapMaxCadence = 18,
 LapAvgPower = 19,
 LapMaxPower = 20,
 LapTotalAscent = 21,
 LapTotalDescent = 22,
 LapIntensity = 23,
 LapLapTrigger = 24,
 LapSport = 25,
 LapEventGroup = 26,
 LapNumLengths = 32,
 LapNormalizedPower = 33,
 LapLeftRightBalance = 34,
 LapFirstLengthIndex = 35,
 LapAvgStrokeDistance = 37,
 LapSwimStroke = 38,
 LapSubSport = 39,
 LapNumActiveLengths = 40,
 LapTotalWork = 41,
 LapAvgAltitude = 42,
 LapMaxAltitude = 43,
 LapGpsAccuracy = 44,
 LapAvgGrade = 45,
 LapAvgPosGrade = 46,
 LapAvgNegGrade = 47,
 LapMaxPosGrade = 48,
 LapMaxNegGrade = 49,
 LapAvgTemperature = 50,
 LapMaxTemperature = 51,
 LapTotalMovingTime = 52,
 LapAvgPosVerticalSpeed = 53,
 LapAvgNegVerticalSpeed = 54,
 LapMaxPosVerticalSpeed = 55,
 LapMaxNegVerticalSpeed = 56,
 LapTimeInHrZone = 57,
 LapTimeInSpeedZone = 58,
 LapTimeInCadenceZone = 59,
 LapTimeInPowerZone = 60,
 LapRepetitionNum = 61,
 LapMinAltitude = 62,
 LapMinHeartRate = 63,
 LapWktStepIndex = 71,
 LapOpponentScore = 74,
 LapStrokeCount = 75,
 LapZoneCount = 76,
 LapAvgVerticalOscillation = 77,
 LapAvgStanceTimePercent = 78,
 LapAvgStanceTime = 79,
 LapAvgFractionalCadence = 80,
 LapMaxFractionalCadence = 81,
 LapTotalFractionalCycles = 82,
 LapPlayerScore = 83,
 LapAvgTotalHemoglobinConc = 84,
 LapMinTotalHemoglobinConc = 85,
 LapMaxTotalHemoglobinConc = 86,
 LapAvgSaturatedHemoglobinPercent = 87,
 LapMinSaturatedHemoglobinPercent = 88,
 LapMaxSaturatedHemoglobinPercent = 89,
 LapAvgLeftTorqueEffectiveness = 91,
 LapAvgRightTorqueEffectiveness = 92,
 LapAvgLeftPedalSmoothness = 93,
 LapAvgRightPedalSmoothness = 94,
 LapAvgCombinedPedalSmoothness = 95,
 LapTimeStanding = 98,
 LapStandCount = 99,
 LapAvgLeftPco = 100,
 LapAvgRightPco = 101,
 LapAvgLeftPowerPhase = 102,
 LapAvgLeftPowerPhasePeak = 103,
 LapAvgRightPowerPhase = 104,
 LapAvgRightPowerPhasePeak = 105,
 LapAvgPowerPosition = 106,
 LapMaxPowerPosition = 107,
 LapAvgCadencePosition = 108,
 LapMaxCadencePosition = 109,
 LapEnhancedAvgSpeed = 110,
 LapEnhancedMaxSpeed = 111,
 LapEnhancedAvgAltitude = 112,
 LapEnhancedMinAltitude = 113,
 LapEnhancedMaxAltitude = 114,
 LapAvgLevMotorPower = 115,
 LapMaxLevMotorPower = 116,
 LapLevBatteryConsumption = 117
} Lap;

typedef enum {
 LengthMessageIndex = 254,
 LengthTimestamp = 253,
 LengthEvent = 0,
 LengthEventType = 1,
 LengthStartTime = 2,
 LengthTotalElapsedTime = 3,
 LengthTotalTimerTime = 4,
 LengthTotalStrokes = 5,
 LengthAvgSpeed = 6,
 LengthSwimStroke = 7,
 LengthAvgSwimmingCadence = 9,
 LengthEventGroup = 10,
 LengthTotalCalories = 11,
 LengthLengthType = 12,
 LengthPlayerScore = 18,
 LengthOpponentScore = 19,
 LengthStrokeCount = 20,
 LengthZoneCount = 21
} Length;

typedef enum {
 RecordTimestamp = 253,
 RecordPositionLat = 0,
 RecordPositionLong = 1,
 RecordAltitude = 2,
 RecordHeartRate = 3,
 RecordCadence = 4,
 RecordDistance = 5,
 RecordSpeed = 6,
 RecordPower = 7,
 RecordCompressedSpeedDistance = 8,
 RecordGrade = 9,
 RecordResistance = 10,
 RecordTimeFromCourse = 11,
 RecordCycleLength = 12,
 RecordTemperature = 13,
 RecordSpeed1s = 17,
 RecordCycles = 18,
 RecordTotalCycles = 19,
 RecordCompressedAccumulatedPower = 28,
 RecordAccumulatedPower = 29,
 RecordLeftRightBalance = 30,
 RecordGpsAccuracy = 31,
 RecordVerticalSpeed = 32,
 RecordCalories = 33,
 RecordVerticalOscillation = 39,
 RecordStanceTimePercent = 40,
 RecordStanceTime = 41,
 RecordActivityType = 42,
 RecordLeftTorqueEffectiveness = 43,
 RecordRightTorqueEffectiveness = 44,
 RecordLeftPedalSmoothness = 45,
 RecordRightPedalSmoothness = 46,
 RecordCombinedPedalSmoothness = 47,
 RecordTime128 = 48,
 RecordStrokeType = 49,
 RecordZone = 50,
 RecordBallSpeed = 51,
 RecordCadence256 = 52,
 RecordFractionalCadence = 53,
 RecordTotalHemoglobinConc = 54,
 RecordTotalHemoglobinConcMin = 55,
 RecordTotalHemoglobinConcMax = 56,
 RecordSaturatedHemoglobinPercent = 57,
 RecordSaturatedHemoglobinPercentMin = 58,
 RecordSaturatedHemoglobinPercentMax = 59,
 RecordDeviceIndex = 62,
 RecordLeftPco = 67,
 RecordRightPco = 68,
 RecordLeftPowerPhase = 69,
 RecordLeftPowerPhasePeak = 70,
 RecordRightPowerPhase = 71,
 RecordRightPowerPhasePeak = 72,
 RecordEnhancedSpeed = 73,
 RecordEnhancedAltitude = 78,
 RecordBatterySoc = 81,
 RecordMotorPower = 82
} Record;

typedef enum {
 EventTimestamp = 253,
 EventEvent = 0,
 EventEventType = 1,
 EventData16 = 2,
 EventData = 3,
 EventEventGroup = 4,
 EventScore = 7,
 EventOpponentScore = 8,
 EventFrontGearNum = 9,
 EventFrontGear = 10,
 EventRearGearNum = 11,
 EventRearGear = 12,
 EventDeviceIndex = 13
} Event;

typedef enum {
 DeviceInfoTimestamp = 253,
 DeviceInfoDeviceIndex = 0,
 DeviceInfoDeviceType = 1,
 DeviceInfoManufacturer = 2,
 DeviceInfoSerialNumber = 3,
 DeviceInfoProduct = 4,
 DeviceInfoSoftwareVersion = 5,
 DeviceInfoHardwareVersion = 6,
 DeviceInfoCumOperatingTime = 7,
 DeviceInfoBatteryVoltage = 10,
 DeviceInfoBatteryStatus = 11,
 DeviceInfoSensorPosition = 18,
 DeviceInfoDescriptor = 19,
 DeviceInfoAntTransmissionType = 20,
 DeviceInfoAntDeviceNumber = 21,
 DeviceInfoAntNetwork = 22,
 DeviceInfoSourceType = 25,
 DeviceInfoProductName = 27
} DeviceInfo;

typedef enum {
 TrainingFileTimestamp = 253,
 TrainingFileType = 0,
 TrainingFileManufacturer = 1,
 TrainingFileProduct = 2,
 TrainingFileSerialNumber = 3,
 TrainingFileTimeCreated = 4
} TrainingFile;

typedef enum {
 HrvTime = 0
} Hrv;

typedef enum {
 CameraEventTimestamp = 253,
 CameraEventTimestampMs = 0,
 CameraEventCameraEventType = 1,
 CameraEventCameraFileUuid = 2,
 CameraEventCameraOrientation = 3
} CameraEvent;

typedef enum {
 GyroscopeDataTimestamp = 253,
 GyroscopeDataTimestampMs = 0,
 GyroscopeDataSampleTimeOffset = 1,
 GyroscopeDataGyroX = 2,
 GyroscopeDataGyroY = 3,
 GyroscopeDataGyroZ = 4,
 GyroscopeDataCalibratedGyroX = 5,
 GyroscopeDataCalibratedGyroY = 6,
 GyroscopeDataCalibratedGyroZ = 7
} GyroscopeData;

typedef enum {
 AccelerometerDataTimestamp = 253,
 AccelerometerDataTimestampMs = 0,
 AccelerometerDataSampleTimeOffset = 1,
 AccelerometerDataAccelX = 2,
 AccelerometerDataAccelY = 3,
 AccelerometerDataAccelZ = 4,
 AccelerometerDataCalibratedAccelX = 5,
 AccelerometerDataCalibratedAccelY = 6,
 AccelerometerDataCalibratedAccelZ = 7
} AccelerometerData;

typedef enum {
 ThreeDSensorCalibrationTimestamp = 253,
 ThreeDSensorCalibrationSensorType = 0,
 ThreeDSensorCalibrationCalibrationFactor = 1,
 ThreeDSensorCalibrationCalibrationDivisor = 2,
 ThreeDSensorCalibrationLevelShift = 3,
 ThreeDSensorCalibrationOffsetCal = 4,
 ThreeDSensorCalibrationOrientationMatrix = 5
} ThreeDSensorCalibration;

typedef enum {
 VideoFrameTimestamp = 253,
 VideoFrameTimestampMs = 0,
 VideoFrameFrameNumber = 1
} VideoFrame;

typedef enum {
 ObdiiDataTimestamp = 253,
 ObdiiDataTimestampMs = 0,
 ObdiiDataTimeOffset = 1,
 ObdiiDataPid = 2,
 ObdiiDataRawData = 3,
 ObdiiDataPidDataSize = 4,
 ObdiiDataSystemTime = 5,
 ObdiiDataStartTimestamp = 6,
 ObdiiDataStartTimestampMs = 7
} ObdiiData;

typedef enum {
 NmeaSentenceTimestamp = 253,
 NmeaSentenceTimestampMs = 0,
 NmeaSentenceSentence = 1
} NmeaSentence;

typedef enum {
 AviationAttitudeTimestamp = 253,
 AviationAttitudeTimestampMs = 0,
 AviationAttitudeSystemTime = 1,
 AviationAttitudePitch = 2,
 AviationAttitudeRoll = 3,
 AviationAttitudeAccelLateral = 4,
 AviationAttitudeAccelNormal = 5,
 AviationAttitudeTurnRate = 6,
 AviationAttitudeStage = 7,
 AviationAttitudeAttitudeStageComplete = 8,
 AviationAttitudeTrack = 9,
 AviationAttitudeValidity = 10
} AviationAttitude;

typedef enum {
 VideoUrl = 0,
 VideoHostingProvider = 1,
 VideoDuration = 2
} Video;

typedef enum {
 VideoTitleMessageIndex = 254,
 VideoTitleMessageCount = 0,
 VideoTitleText = 1
} VideoTitle;

typedef enum {
 VideoDescriptionMessageIndex = 254,
 VideoDescriptionMessageCount = 0,
 VideoDescriptionText = 1
} VideoDescription;

typedef enum {
 VideoClipClipNumber = 0,
 VideoClipStartTimestamp = 1,
 VideoClipStartTimestampMs = 2,
 VideoClipEndTimestamp = 3,
 VideoClipEndTimestampMs = 4,
 VideoClipClipStart = 6,
 VideoClipClipEnd = 7
} VideoClip;

typedef enum {
 CourseSport = 4,
 CourseName = 5,
 CourseCapabilities = 6
} Course;

typedef enum {
 CoursePointMessageIndex = 254,
 CoursePointTimestamp = 1,
 CoursePointPositionLat = 2,
 CoursePointPositionLong = 3,
 CoursePointDistance = 4,
 CoursePointType = 5,
 CoursePointName = 6,
 CoursePointFavorite = 8
} CoursePoint;

typedef enum {
 SegmentIdName = 0,
 SegmentIdUuid = 1,
 SegmentIdSport = 2,
 SegmentIdEnabled = 3,
 SegmentIdUserProfilePrimaryKey = 4,
 SegmentIdDeviceId = 5,
 SegmentIdDefaultRaceLeader = 6,
 SegmentIdDeleteStatus = 7,
 SegmentIdSelectionType = 8
} SegmentId;

typedef enum {
 SegmentLeaderboardEntryMessageIndex = 254,
 SegmentLeaderboardEntryName = 0,
 SegmentLeaderboardEntryType = 1,
 SegmentLeaderboardEntryGroupPrimaryKey = 2,
 SegmentLeaderboardEntryActivityId = 3,
 SegmentLeaderboardEntrySegmentTime = 4,
 SegmentLeaderboardEntryActivityIdString = 5
} SegmentLeaderboardEntry;

typedef enum {
 SegmentPointMessageIndex = 254,
 SegmentPointPositionLat = 1,
 SegmentPointPositionLong = 2,
 SegmentPointDistance = 3,
 SegmentPointAltitude = 4,
 SegmentPointLeaderTime = 5
} SegmentPoint;

typedef enum {
 SegmentLapMessageIndex = 254,
 SegmentLapTimestamp = 253,
 SegmentLapEvent = 0,
 SegmentLapEventType = 1,
 SegmentLapStartTime = 2,
 SegmentLapStartPositionLat = 3,
 SegmentLapStartPositionLong = 4,
 SegmentLapEndPositionLat = 5,
 SegmentLapEndPositionLong = 6,
 SegmentLapTotalElapsedTime = 7,
 SegmentLapTotalTimerTime = 8,
 SegmentLapTotalDistance = 9,
 SegmentLapTotalCycles = 10,
 SegmentLapTotalCalories = 11,
 SegmentLapTotalFatCalories = 12,
 SegmentLapAvgSpeed = 13,
 SegmentLapMaxSpeed = 14,
 SegmentLapAvgHeartRate = 15,
 SegmentLapMaxHeartRate = 16,
 SegmentLapAvgCadence = 17,
 SegmentLapMaxCadence = 18,
 SegmentLapAvgPower = 19,
 SegmentLapMaxPower = 20,
 SegmentLapTotalAscent = 21,
 SegmentLapTotalDescent = 22,
 SegmentLapSport = 23,
 SegmentLapEventGroup = 24,
 SegmentLapNecLat = 25,
 SegmentLapNecLong = 26,
 SegmentLapSwcLat = 27,
 SegmentLapSwcLong = 28,
 SegmentLapName = 29,
 SegmentLapNormalizedPower = 30,
 SegmentLapLeftRightBalance = 31,
 SegmentLapSubSport = 32,
 SegmentLapTotalWork = 33,
 SegmentLapAvgAltitude = 34,
 SegmentLapMaxAltitude = 35,
 SegmentLapGpsAccuracy = 36,
 SegmentLapAvgGrade = 37,
 SegmentLapAvgPosGrade = 38,
 SegmentLapAvgNegGrade = 39,
 SegmentLapMaxPosGrade = 40,
 SegmentLapMaxNegGrade = 41,
 SegmentLapAvgTemperature = 42,
 SegmentLapMaxTemperature = 43,
 SegmentLapTotalMovingTime = 44,
 SegmentLapAvgPosVerticalSpeed = 45,
 SegmentLapAvgNegVerticalSpeed = 46,
 SegmentLapMaxPosVerticalSpeed = 47,
 SegmentLapMaxNegVerticalSpeed = 48,
 SegmentLapTimeInHrZone = 49,
 SegmentLapTimeInSpeedZone = 50,
 SegmentLapTimeInCadenceZone = 51,
 SegmentLapTimeInPowerZone = 52,
 SegmentLapRepetitionNum = 53,
 SegmentLapMinAltitude = 54,
 SegmentLapMinHeartRate = 55,
 SegmentLapActiveTime = 56,
 SegmentLapWktStepIndex = 57,
 SegmentLapSportEvent = 58,
 SegmentLapAvgLeftTorqueEffectiveness = 59,
 SegmentLapAvgRightTorqueEffectiveness = 60,
 SegmentLapAvgLeftPedalSmoothness = 61,
 SegmentLapAvgRightPedalSmoothness = 62,
 SegmentLapAvgCombinedPedalSmoothness = 63,
 SegmentLapStatus = 64,
 SegmentLapUuid = 65,
 SegmentLapAvgFractionalCadence = 66,
 SegmentLapMaxFractionalCadence = 67,
 SegmentLapTotalFractionalCycles = 68,
 SegmentLapFrontGearShiftCount = 69,
 SegmentLapRearGearShiftCount = 70,
 SegmentLapTimeStanding = 71,
 SegmentLapStandCount = 72,
 SegmentLapAvgLeftPco = 73,
 SegmentLapAvgRightPco = 74,
 SegmentLapAvgLeftPowerPhase = 75,
 SegmentLapAvgLeftPowerPhasePeak = 76,
 SegmentLapAvgRightPowerPhase = 77,
 SegmentLapAvgRightPowerPhasePeak = 78,
 SegmentLapAvgPowerPosition = 79,
 SegmentLapMaxPowerPosition = 80,
 SegmentLapAvgCadencePosition = 81,
 SegmentLapMaxCadencePosition = 82
} SegmentLap;

typedef enum {
 SegmentFileMessageIndex = 254,
 SegmentFileFileUuid = 1,
 SegmentFileEnabled = 3,
 SegmentFileUserProfilePrimaryKey = 4,
 SegmentFileLeaderType = 7,
 SegmentFileLeaderGroupPrimaryKey = 8,
 SegmentFileLeaderActivityId = 9,
 SegmentFileLeaderActivityIdString = 10
} SegmentFile;

typedef enum {
 WorkoutSport = 4,
 WorkoutCapabilities = 5,
 WorkoutNumValidSteps = 6,
 WorkoutWktName = 8
} Workout;

typedef enum {
 WorkoutStepMessageIndex = 254,
 WorkoutStepWktStepName = 0,
 WorkoutStepDurationType = 1,
 WorkoutStepDurationValue = 2,
 WorkoutStepTargetType = 3,
 WorkoutStepTargetValue = 4,
 WorkoutStepCustomTargetValueLow = 5,
 WorkoutStepCustomTargetValueHigh = 6,
 WorkoutStepIntensity = 7
} WorkoutStep;

typedef enum {
 ScheduleManufacturer = 0,
 ScheduleProduct = 1,
 ScheduleSerialNumber = 2,
 ScheduleTimeCreated = 3,
 ScheduleCompleted = 4,
 ScheduleType = 5,
 ScheduleScheduledTime = 6
} Schedule;

typedef enum {
 TotalsMessageIndex = 254,
 TotalsTimestamp = 253,
 TotalsTimerTime = 0,
 TotalsDistance = 1,
 TotalsCalories = 2,
 TotalsSport = 3,
 TotalsElapsedTime = 4,
 TotalsSessions = 5,
 TotalsActiveTime = 6,
 TotalsSportIndex = 9
} Totals;

typedef enum {
 WeightScaleTimestamp = 253,
 WeightScaleWeight = 0,
 WeightScalePercentFat = 1,
 WeightScalePercentHydration = 2,
 WeightScaleVisceralFatMass = 3,
 WeightScaleBoneMass = 4,
 WeightScaleMuscleMass = 5,
 WeightScaleBasalMet = 7,
 WeightScalePhysiqueRating = 8,
 WeightScaleActiveMet = 9,
 WeightScaleMetabolicAge = 10,
 WeightScaleVisceralFatRating = 11,
 WeightScaleUserProfileIndex = 12
} WeightScale;

typedef enum {
 BloodPressureTimestamp = 253,
 BloodPressureSystolicPressure = 0,
 BloodPressureDiastolicPressure = 1,
 BloodPressureMeanArterialPressure = 2,
 BloodPressureMap3SampleMean = 3,
 BloodPressureMapMorningValues = 4,
 BloodPressureMapEveningValues = 5,
 BloodPressureHeartRate = 6,
 BloodPressureHeartRateType = 7,
 BloodPressureStatus = 8,
 BloodPressureUserProfileIndex = 9
} BloodPressure;

typedef enum {
 MonitoringInfoTimestamp = 253,
 MonitoringInfoLocalTimestamp = 0,
 MonitoringInfoActivityType = 1,
 MonitoringInfoCyclesToDistance = 3,
 MonitoringInfoCyclesToCalories = 4,
 MonitoringInfoRestingMetabolicRate = 5
} MonitoringInfo;

typedef enum {
 MonitoringTimestamp = 253,
 MonitoringDeviceIndex = 0,
 MonitoringCalories = 1,
 MonitoringDistance = 2,
 MonitoringCycles = 3,
 MonitoringActiveTime = 4,
 MonitoringActivityType = 5,
 MonitoringActivitySubtype = 6,
 MonitoringActivityLevel = 7,
 MonitoringDistance16 = 8,
 MonitoringCycles16 = 9,
 MonitoringActiveTime16 = 10,
 MonitoringLocalTimestamp = 11,
 MonitoringTemperature = 12,
 MonitoringTemperatureMin = 14,
 MonitoringTemperatureMax = 15,
 MonitoringActivityTime = 16,
 MonitoringActiveCalories = 19,
 MonitoringCurrentActivityTypeIntensity = 24,
 MonitoringTimestampMin8 = 25,
 MonitoringTimestamp16 = 26,
 MonitoringHeartRate = 27,
 MonitoringIntensity = 28,
 MonitoringDurationMin = 29,
 MonitoringDuration = 30
} Monitoring;

typedef enum {
 MemoGlobPartIndex = 250,
 MemoGlobMemo = 0,
 MemoGlobMessageNumber = 1,
 MemoGlobMessageIndex = 2
} MemoGlob;

// ----------- end generated code -----------
#endif // FIT_FIELDS_H
