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

#ifndef FIT_PROFILE_H
#define FIT_PROFILE_H

// ----------- start generated code -----------
typedef enum {
    eFileDevice = 1,
    eFileSettings = 2,
    eFileSport = 3,
    eFileActivity = 4,
    eFileWorkout = 5,
    eFileCourse = 6,
    eFileSchedules = 7,
    eFileWeight = 9,
    eFileTotals = 10,
    eFileGoals = 11,
    eFileBloodPressure = 14,
    eFileMonitoringA = 15,
    eFileActivitySummary = 20,
    eFileMonitoringDaily = 28,
    eFileMonitoringB = 32,
    eFileSegment = 34,
    eFileSegmentList = 35,
    eFileMfgRangeMin = 0xF7,
    eFileMfgRangeMax = 0xFE
} file_e;

typedef enum {
    eMesgNumFileId = 0,
    eMesgNumCapabilities = 1,
    eMesgNumDeviceSettings = 2,
    eMesgNumUserProfile = 3,
    eMesgNumHrmProfile = 4,
    eMesgNumSdmProfile = 5,
    eMesgNumBikeProfile = 6,
    eMesgNumZonesTarget = 7,
    eMesgNumHrZone = 8,
    eMesgNumPowerZone = 9,
    eMesgNumMetZone = 10,
    eMesgNumSport = 12,
    eMesgNumGoal = 15,
    eMesgNumSession = 18,
    eMesgNumLap = 19,
    eMesgNumRecord = 20,
    eMesgNumEvent = 21,
    eMesgNumDeviceInfo = 23,
    eMesgNumWorkout = 26,
    eMesgNumWorkoutStep = 27,
    eMesgNumSchedule = 28,
    eMesgNumWeightScale = 30,
    eMesgNumCourse = 31,
    eMesgNumCoursePoint = 32,
    eMesgNumTotals = 33,
    eMesgNumActivity = 34,
    eMesgNumSoftware = 35,
    eMesgNumFileCapabilities = 37,
    eMesgNumMesgCapabilities = 38,
    eMesgNumFieldCapabilities = 39,
    eMesgNumFileCreator = 49,
    eMesgNumBloodPressure = 51,
    eMesgNumSpeedZone = 53,
    eMesgNumMonitoring = 55,
    eMesgNumTrainingFile = 72,
    eMesgNumHrv = 78,
    eMesgNumLength = 101,
    eMesgNumMonitoringInfo = 103,
    eMesgNumPad = 105,
    eMesgNumSlaveDevice = 106,
    eMesgNumCadenceZone = 131,
    eMesgNumSegmentLap = 142,
    eMesgNumMemoGlob = 145,
    eMesgNumSegmentId = 148,
    eMesgNumSegmentLeaderboardEntry = 149,
    eMesgNumSegmentPoint = 150,
    eMesgNumSegmentFile = 151,
    eMesgNumGpsMetadata = 160,
    eMesgNumCameraEvent = 161,
    eMesgNumTimestampCorrelation = 162,
    eMesgNumGyroscopeData = 164,
    eMesgNumAccelerometerData = 165,
    eMesgNumThreeDSensorCalibration = 167,
    eMesgNumVideoFrame = 169,
    eMesgNumObdiiData = 174,
    eMesgNumNmeaSentence = 177,
    eMesgNumAviationAttitude = 178,
    eMesgNumVideo = 184,
    eMesgNumVideoTitle = 185,
    eMesgNumVideoDescription = 186,
    eMesgNumVideoClip = 187,
    eMesgNumMfgRangeMin = 0xFF00,
    eMesgNumMfgRangeMax = 0xFFFE
} mesg_num_e;

typedef enum {
    eChecksumClear = 0,
    eChecksumOk = 1
} checksum_e;

typedef enum {
    eFileFlagsRead = 0x02,
    eFileFlagsWrite = 0x04,
    eFileFlagsErase = 0x08
} file_flags_e;

typedef enum {
    eMesgCountNumPerFile = 0,
    eMesgCountMaxPerFile = 1,
    eMesgCountMaxPerFileType = 2
} mesg_count_e;

typedef enum {
    eDateTimeMin = 0x10000000
} date_time_e;

typedef enum {
    eLocalDateTimeMin = 0x10000000
} local_date_time_e;

typedef enum {
    eMessageIndexSelected = 0x8000,
    eMessageIndexReserved = 0x7000,
    eMessageIndexMask = 0x0FFF
} message_index_e;

typedef enum {
    eDeviceIndexCreator = 0
} device_index_e;

typedef enum {
    eGenderFemale = 0,
    eGenderMale = 1
} gender_e;

typedef enum {
    eLanguageEnglish = 0,
    eLanguageFrench = 1,
    eLanguageItalian = 2,
    eLanguageGerman = 3,
    eLanguageSpanish = 4,
    eLanguageCroatian = 5,
    eLanguageCzech = 6,
    eLanguageDanish = 7,
    eLanguageDutch = 8,
    eLanguageFinnish = 9,
    eLanguageGreek = 10,
    eLanguageHungarian = 11,
    eLanguageNorwegian = 12,
    eLanguagePolish = 13,
    eLanguagePortuguese = 14,
    eLanguageSlovakian = 15,
    eLanguageSlovenian = 16,
    eLanguageSwedish = 17,
    eLanguageRussian = 18,
    eLanguageTurkish = 19,
    eLanguageLatvian = 20,
    eLanguageUkrainian = 21,
    eLanguageArabic = 22,
    eLanguageFarsi = 23,
    eLanguageBulgarian = 24,
    eLanguageRomanian = 25,
    eLanguageCustom = 254
} language_e;

typedef enum {
    eTimeZoneAlmaty = 0,
    eTimeZoneBangkok = 1,
    eTimeZoneBombay = 2,
    eTimeZoneBrasilia = 3,
    eTimeZoneCairo = 4,
    eTimeZoneCapeVerdeIs = 5,
    eTimeZoneDarwin = 6,
    eTimeZoneEniwetok = 7,
    eTimeZoneFiji = 8,
    eTimeZoneHongKong = 9,
    eTimeZoneIslamabad = 10,
    eTimeZoneKabul = 11,
    eTimeZoneMagadan = 12,
    eTimeZoneMidAtlantic = 13,
    eTimeZoneMoscow = 14,
    eTimeZoneMuscat = 15,
    eTimeZoneNewfoundland = 16,
    eTimeZoneSamoa = 17,
    eTimeZoneSydney = 18,
    eTimeZoneTehran = 19,
    eTimeZoneTokyo = 20,
    eTimeZoneUsAlaska = 21,
    eTimeZoneUsAtlantic = 22,
    eTimeZoneUsCentral = 23,
    eTimeZoneUsEastern = 24,
    eTimeZoneUsHawaii = 25,
    eTimeZoneUsMountain = 26,
    eTimeZoneUsPacific = 27,
    eTimeZoneOther = 28,
    eTimeZoneAuckland = 29,
    eTimeZoneKathmandu = 30,
    eTimeZoneEuropeWesternWet = 31,
    eTimeZoneEuropeCentralCet = 32,
    eTimeZoneEuropeEasternEet = 33,
    eTimeZoneJakarta = 34,
    eTimeZonePerth = 35,
    eTimeZoneAdelaide = 36,
    eTimeZoneBrisbane = 37,
    eTimeZoneTasmania = 38,
    eTimeZoneIceland = 39,
    eTimeZoneAmsterdam = 40,
    eTimeZoneAthens = 41,
    eTimeZoneBarcelona = 42,
    eTimeZoneBerlin = 43,
    eTimeZoneBrussels = 44,
    eTimeZoneBudapest = 45,
    eTimeZoneCopenhagen = 46,
    eTimeZoneDublin = 47,
    eTimeZoneHelsinki = 48,
    eTimeZoneLisbon = 49,
    eTimeZoneLondon = 50,
    eTimeZoneMadrid = 51,
    eTimeZoneMunich = 52,
    eTimeZoneOslo = 53,
    eTimeZoneParis = 54,
    eTimeZonePrague = 55,
    eTimeZoneReykjavik = 56,
    eTimeZoneRome = 57,
    eTimeZoneStockholm = 58,
    eTimeZoneVienna = 59,
    eTimeZoneWarsaw = 60,
    eTimeZoneZurich = 61,
    eTimeZoneQuebec = 62,
    eTimeZoneOntario = 63,
    eTimeZoneManitoba = 64,
    eTimeZoneSaskatchewan = 65,
    eTimeZoneAlberta = 66,
    eTimeZoneBritishColumbia = 67,
    eTimeZoneBoise = 68,
    eTimeZoneBoston = 69,
    eTimeZoneChicago = 70,
    eTimeZoneDallas = 71,
    eTimeZoneDenver = 72,
    eTimeZoneKansasCity = 73,
    eTimeZoneLasVegas = 74,
    eTimeZoneLosAngeles = 75,
    eTimeZoneMiami = 76,
    eTimeZoneMinneapolis = 77,
    eTimeZoneNewYork = 78,
    eTimeZoneNewOrleans = 79,
    eTimeZonePhoenix = 80,
    eTimeZoneSantaFe = 81,
    eTimeZoneSeattle = 82,
    eTimeZoneWashingtonDc = 83,
    eTimeZoneUsArizona = 84,
    eTimeZoneChita = 85,
    eTimeZoneEkaterinburg = 86,
    eTimeZoneIrkutsk = 87,
    eTimeZoneKaliningrad = 88,
    eTimeZoneKrasnoyarsk = 89,
    eTimeZoneNovosibirsk = 90,
    eTimeZonePetropavlovskKamchatskiy = 91,
    eTimeZoneSamara = 92,
    eTimeZoneVladivostok = 93,
    eTimeZoneMexicoCentral = 94,
    eTimeZoneMexicoMountain = 95,
    eTimeZoneMexicoPacific = 96,
    eTimeZoneCapeTown = 97,
    eTimeZoneWinkhoek = 98,
    eTimeZoneLagos = 99,
    eTimeZoneRiyahd = 100,
    eTimeZoneVenezuela = 101,
    eTimeZoneAustraliaLh = 102,
    eTimeZoneSantiago = 103,
    eTimeZoneManual = 253,
    eTimeZoneAutomatic = 254
} time_zone_e;

typedef enum {
    eDisplayMeasureMetric = 0,
    eDisplayMeasureStatute = 1
} display_measure_e;

typedef enum {
    eDisplayHeartBpm = 0,
    eDisplayHeartMax = 1,
    eDisplayHeartReserve = 2
} display_heart_e;

typedef enum {
    eDisplayPowerWatts = 0,
    eDisplayPowerPercentFtp = 1
} display_power_e;

typedef enum {
    eDisplayPositionDegree = 0,
    eDisplayPositionDegreeMinute = 1,
    eDisplayPositionDegreeMinuteSecond = 2,
    eDisplayPositionAustrianGrid = 3,
    eDisplayPositionBritishGrid = 4,
    eDisplayPositionDutchGrid = 5,
    eDisplayPositionHungarianGrid = 6,
    eDisplayPositionFinnishGrid = 7,
    eDisplayPositionGermanGrid = 8,
    eDisplayPositionIcelandicGrid = 9,
    eDisplayPositionIndonesianEquatorial = 10,
    eDisplayPositionIndonesianIrian = 11,
    eDisplayPositionIndonesianSouthern = 12,
    eDisplayPositionIndiaZone0 = 13,
    eDisplayPositionIndiaZoneIA = 14,
    eDisplayPositionIndiaZoneIB = 15,
    eDisplayPositionIndiaZoneIIA = 16,
    eDisplayPositionIndiaZoneIIB = 17,
    eDisplayPositionIndiaZoneIIIA = 18,
    eDisplayPositionIndiaZoneIIIB = 19,
    eDisplayPositionIndiaZoneIVA = 20,
    eDisplayPositionIndiaZoneIVB = 21,
    eDisplayPositionIrishTransverse = 22,
    eDisplayPositionIrishGrid = 23,
    eDisplayPositionLoran = 24,
    eDisplayPositionMaidenheadGrid = 25,
    eDisplayPositionMgrsGrid = 26,
    eDisplayPositionNewZealandGrid = 27,
    eDisplayPositionNewZealandTransverse = 28,
    eDisplayPositionQatarGrid = 29,
    eDisplayPositionModifiedSwedishGrid = 30,
    eDisplayPositionSwedishGrid = 31,
    eDisplayPositionSouthAfricanGrid = 32,
    eDisplayPositionSwissGrid = 33,
    eDisplayPositionTaiwanGrid = 34,
    eDisplayPositionUnitedStatesGrid = 35,
    eDisplayPositionUtmUpsGrid = 36,
    eDisplayPositionWestMalayan = 37,
    eDisplayPositionBorneoRso = 38,
    eDisplayPositionEstonianGrid = 39,
    eDisplayPositionLatvianGrid = 40,
    eDisplayPositionSwedishRef99Grid = 41
} display_position_e;

typedef enum {
    eSportGeneric = 0,
    eSportRunning = 1,
    eSportCycling = 2,
    eSportTransition = 3,
    eSportFitnessEquipment = 4,
    eSportSwimming = 5,
    eSportBasketball = 6,
    eSportSoccer = 7,
    eSportTennis = 8,
    eSportAmericanFootball = 9,
    eSportTraining = 10,
    eSportWalking = 11,
    eSportCrossCountrySkiing = 12,
    eSportAlpineSkiing = 13,
    eSportSnowboarding = 14,
    eSportRowing = 15,
    eSportMountaineering = 16,
    eSportHiking = 17,
    eSportMultisport = 18,
    eSportPaddling = 19,
    eSportFlying = 20,
    eSportEBiking = 21,
    eSportMotorcycling = 22,
    eSportBoating = 23,
    eSportDriving = 24,
    eSportGolf = 25,
    eSportHangGliding = 26,
    eSportHorsebackRiding = 27,
    eSportHunting = 28,
    eSportFishing = 29,
    eSportInlineSkating = 30,
    eSportRockClimbing = 31,
    eSportSailing = 32,
    eSportIceSkating = 33,
    eSportSkyDiving = 34,
    eSportSnowshoeing = 35,
    eSportSnowmobiling = 36,
    eSportStandUpPaddleboarding = 37,
    eSportSurfing = 38,
    eSportWakeboarding = 39,
    eSportWaterSkiing = 40,
    eSportKayaking = 41,
    eSportRafting = 42,
    eSportWindsurfing = 43,
    eSportKitesurfing = 44,
    eSportAll = 254
} sport_e;

typedef enum {
    eSportBits0Generic = 0x01,
    eSportBits0Running = 0x02,
    eSportBits0Cycling = 0x04,
    eSportBits0Transition = 0x08,
    eSportBits0FitnessEquipment = 0x10,
    eSportBits0Swimming = 0x20,
    eSportBits0Basketball = 0x40,
    eSportBits0Soccer = 0x80
} sport_bits_0_e;

typedef enum {
    eSportBits1Tennis = 0x01,
    eSportBits1AmericanFootball = 0x02,
    eSportBits1Training = 0x04,
    eSportBits1Walking = 0x08,
    eSportBits1CrossCountrySkiing = 0x10,
    eSportBits1AlpineSkiing = 0x20,
    eSportBits1Snowboarding = 0x40,
    eSportBits1Rowing = 0x80
} sport_bits_1_e;

typedef enum {
    eSportBits2Mountaineering = 0x01,
    eSportBits2Hiking = 0x02,
    eSportBits2Multisport = 0x04,
    eSportBits2Paddling = 0x08,
    eSportBits2Flying = 0x10,
    eSportBits2EBiking = 0x20,
    eSportBits2Motorcycling = 0x40,
    eSportBits2Boating = 0x80
} sport_bits_2_e;

typedef enum {
    eSportBits3Driving = 0x01,
    eSportBits3Golf = 0x02,
    eSportBits3HangGliding = 0x04,
    eSportBits3HorsebackRiding = 0x08,
    eSportBits3Hunting = 0x10,
    eSportBits3Fishing = 0x20,
    eSportBits3InlineSkating = 0x40,
    eSportBits3RockClimbing = 0x80
} sport_bits_3_e;

typedef enum {
    eSportBits4Sailing = 0x01,
    eSportBits4IceSkating = 0x02,
    eSportBits4SkyDiving = 0x04,
    eSportBits4Snowshoeing = 0x08,
    eSportBits4Snowmobiling = 0x10,
    eSportBits4StandUpPaddleboarding = 0x20,
    eSportBits4Surfing = 0x40,
    eSportBits4Wakeboarding = 0x80
} sport_bits_4_e;

typedef enum {
    eSportBits5WaterSkiing = 0x01,
    eSportBits5Kayaking = 0x02,
    eSportBits5Rafting = 0x04,
    eSportBits5Windsurfing = 0x08,
    eSportBits5Kitesurfing = 0x10
} sport_bits_5_e;

typedef enum {
    eSubSportGeneric = 0,
    eSubSportTreadmill = 1,
    eSubSportStreet = 2,
    eSubSportTrail = 3,
    eSubSportTrack = 4,
    eSubSportSpin = 5,
    eSubSportIndoorCycling = 6,
    eSubSportRoad = 7,
    eSubSportMountain = 8,
    eSubSportDownhill = 9,
    eSubSportRecumbent = 10,
    eSubSportCyclocross = 11,
    eSubSportHandCycling = 12,
    eSubSportTrackCycling = 13,
    eSubSportIndoorRowing = 14,
    eSubSportElliptical = 15,
    eSubSportStairClimbing = 16,
    eSubSportLapSwimming = 17,
    eSubSportOpenWater = 18,
    eSubSportFlexibilityTraining = 19,
    eSubSportStrengthTraining = 20,
    eSubSportWarmUp = 21,
    eSubSportMatch = 22,
    eSubSportExercise = 23,
    eSubSportChallenge = 24,
    eSubSportIndoorSkiing = 25,
    eSubSportCardioTraining = 26,
    eSubSportIndoorWalking = 27,
    eSubSportEBikeFitness = 28,
    eSubSportBmx = 29,
    eSubSportCasualWalking = 30,
    eSubSportSpeedWalking = 31,
    eSubSportBikeToRunTransition = 32,
    eSubSportRunToBikeTransition = 33,
    eSubSportSwimToBikeTransition = 34,
    eSubSportAtv = 35,
    eSubSportMotocross = 36,
    eSubSportBackcountry = 37,
    eSubSportResort = 38,
    eSubSportRcDrone = 39,
    eSubSportWingsuit = 40,
    eSubSportWhitewater = 41,
    eSubSportAll = 254
} sub_sport_e;

typedef enum {
    eSportEventUncategorized = 0,
    eSportEventGeocaching = 1,
    eSportEventFitness = 2,
    eSportEventRecreation = 3,
    eSportEventRace = 4,
    eSportEventSpecialEvent = 5,
    eSportEventTraining = 6,
    eSportEventTransportation = 7,
    eSportEventTouring = 8
} sport_event_e;

typedef enum {
    eActivityManual = 0,
    eActivityAutoMultiSport = 1
} activity_e;

typedef enum {
    eIntensityActive = 0,
    eIntensityRest = 1,
    eIntensityWarmup = 2,
    eIntensityCooldown = 3
} intensity_e;

typedef enum {
    eSessionTriggerActivityEnd = 0,
    eSessionTriggerManual = 1,
    eSessionTriggerAutoMultiSport = 2,
    eSessionTriggerFitnessEquipment = 3
} session_trigger_e;

typedef enum {
    eAutolapTriggerTime = 0,
    eAutolapTriggerDistance = 1,
    eAutolapTriggerPositionStart = 2,
    eAutolapTriggerPositionLap = 3,
    eAutolapTriggerPositionWaypoint = 4,
    eAutolapTriggerPositionMarked = 5,
    eAutolapTriggerOff = 6
} autolap_trigger_e;

typedef enum {
    eLapTriggerManual = 0,
    eLapTriggerTime = 1,
    eLapTriggerDistance = 2,
    eLapTriggerPositionStart = 3,
    eLapTriggerPositionLap = 4,
    eLapTriggerPositionWaypoint = 5,
    eLapTriggerPositionMarked = 6,
    eLapTriggerSessionEnd = 7,
    eLapTriggerFitnessEquipment = 8
} lap_trigger_e;

typedef enum {
    eEventTimer = 0,
    eEventWorkout = 3,
    eEventWorkoutStep = 4,
    eEventPowerDown = 5,
    eEventPowerUp = 6,
    eEventOffCourse = 7,
    eEventSession = 8,
    eEventLap = 9,
    eEventCoursePoint = 10,
    eEventBattery = 11,
    eEventVirtualPartnerPace = 12,
    eEventHrHighAlert = 13,
    eEventHrLowAlert = 14,
    eEventSpeedHighAlert = 15,
    eEventSpeedLowAlert = 16,
    eEventCadHighAlert = 17,
    eEventCadLowAlert = 18,
    eEventPowerHighAlert = 19,
    eEventPowerLowAlert = 20,
    eEventRecoveryHr = 21,
    eEventBatteryLow = 22,
    eEventTimeDurationAlert = 23,
    eEventDistanceDurationAlert = 24,
    eEventCalorieDurationAlert = 25,
    eEventActivity = 26,
    eEventFitnessEquipment = 27,
    eEventLength = 28,
    eEventUserMarker = 32,
    eEventSportPoint = 33,
    eEventCalibration = 36,
    eEventFrontGearChange = 42,
    eEventRearGearChange = 43,
    eEventRiderPositionChange = 44,
    eEventElevHighAlert = 45,
    eEventElevLowAlert = 46,
    eEventCommTimeout = 47
} event_e;

typedef enum {
    eEventTypeStart = 0,
    eEventTypeStop = 1,
    eEventTypeConsecutiveDepreciated = 2,
    eEventTypeMarker = 3,
    eEventTypeStopAll = 4,
    eEventTypeBeginDepreciated = 5,
    eEventTypeEndDepreciated = 6,
    eEventTypeEndAllDepreciated = 7,
    eEventTypeStopDisable = 8,
    eEventTypeStopDisableAll = 9
} event_type_e;

typedef enum {
    eTimerTriggerManual = 0,
    eTimerTriggerAuto = 1,
    eTimerTriggerFitnessEquipment = 2
} timer_trigger_e;

typedef enum {
    eFitnessEquipmentStateReady = 0,
    eFitnessEquipmentStateInUse = 1,
    eFitnessEquipmentStatePaused = 2,
    eFitnessEquipmentStateUnknown = 3
} fitness_equipment_state_e;

typedef enum {
    eActivityClassLevel = 0x7F,
    eActivityClassLevelMax = 100,
    eActivityClassAthlete = 0x80
} activity_class_e;

typedef enum {
    eHrZoneCalcCustom = 0,
    eHrZoneCalcPercentMaxHr = 1,
    eHrZoneCalcPercentHrr = 2
} hr_zone_calc_e;

typedef enum {
    ePwrZoneCalcCustom = 0,
    ePwrZoneCalcPercentFtp = 1
} pwr_zone_calc_e;

typedef enum {
    eWktStepDurationTime = 0,
    eWktStepDurationDistance = 1,
    eWktStepDurationHrLessThan = 2,
    eWktStepDurationHrGreaterThan = 3,
    eWktStepDurationCalories = 4,
    eWktStepDurationOpen = 5,
    eWktStepDurationRepeatUntilStepsCmplt = 6,
    eWktStepDurationRepeatUntilTime = 7,
    eWktStepDurationRepeatUntilDistance = 8,
    eWktStepDurationRepeatUntilCalories = 9,
    eWktStepDurationRepeatUntilHrLessThan = 10,
    eWktStepDurationRepeatUntilHrGreaterThan = 11,
    eWktStepDurationRepeatUntilPowerLessThan = 12,
    eWktStepDurationRepeatUntilPowerGreaterThan = 13,
    eWktStepDurationPowerLessThan = 14,
    eWktStepDurationPowerGreaterThan = 15,
    eWktStepDurationRepetitionTime = 28
} wkt_step_duration_e;

typedef enum {
    eWktStepTargetSpeed = 0,
    eWktStepTargetHeartRate = 1,
    eWktStepTargetOpen = 2,
    eWktStepTargetCadence = 3,
    eWktStepTargetPower = 4,
    eWktStepTargetGrade = 5,
    eWktStepTargetResistance = 6
} wkt_step_target_e;

typedef enum {
    eGoalTime = 0,
    eGoalDistance = 1,
    eGoalCalories = 2,
    eGoalFrequency = 3,
    eGoalSteps = 4
} goal_e;

typedef enum {
    eGoalRecurrenceOff = 0,
    eGoalRecurrenceDaily = 1,
    eGoalRecurrenceWeekly = 2,
    eGoalRecurrenceMonthly = 3,
    eGoalRecurrenceYearly = 4,
    eGoalRecurrenceCustom = 5
} goal_recurrence_e;

typedef enum {
    eScheduleWorkout = 0,
    eScheduleCourse = 1
} schedule_e;

typedef enum {
    eCoursePointGeneric = 0,
    eCoursePointSummit = 1,
    eCoursePointValley = 2,
    eCoursePointWater = 3,
    eCoursePointFood = 4,
    eCoursePointDanger = 5,
    eCoursePointLeft = 6,
    eCoursePointRight = 7,
    eCoursePointStraight = 8,
    eCoursePointFirstAid = 9,
    eCoursePointFourthCategory = 10,
    eCoursePointThirdCategory = 11,
    eCoursePointSecondCategory = 12,
    eCoursePointFirstCategory = 13,
    eCoursePointHorsCategory = 14,
    eCoursePointSprint = 15,
    eCoursePointLeftFork = 16,
    eCoursePointRightFork = 17,
    eCoursePointMiddleFork = 18,
    eCoursePointSlightLeft = 19,
    eCoursePointSharpLeft = 20,
    eCoursePointSlightRight = 21,
    eCoursePointSharpRight = 22,
    eCoursePointUTurn = 23
} course_point_e;

typedef enum {
    eManufacturerGarmin = 1,
    eManufacturerGarminFr405Antfs = 2,
    eManufacturerZephyr = 3,
    eManufacturerDayton = 4,
    eManufacturerIdt = 5,
    eManufacturerSrm = 6,
    eManufacturerQuarq = 7,
    eManufacturerIbike = 8,
    eManufacturerSaris = 9,
    eManufacturerSparkHk = 10,
    eManufacturerTanita = 11,
    eManufacturerEchowell = 12,
    eManufacturerDynastreamOem = 13,
    eManufacturerNautilus = 14,
    eManufacturerDynastream = 15,
    eManufacturerTimex = 16,
    eManufacturerMetrigear = 17,
    eManufacturerXelic = 18,
    eManufacturerBeurer = 19,
    eManufacturerCardiosport = 20,
    eManufacturerAAndD = 21,
    eManufacturerHmm = 22,
    eManufacturerSuunto = 23,
    eManufacturerThitaElektronik = 24,
    eManufacturerGpulse = 25,
    eManufacturerCleanMobile = 26,
    eManufacturerPedalBrain = 27,
    eManufacturerPeaksware = 28,
    eManufacturerSaxonar = 29,
    eManufacturerLemondFitness = 30,
    eManufacturerDexcom = 31,
    eManufacturerWahooFitness = 32,
    eManufacturerOctaneFitness = 33,
    eManufacturerArchinoetics = 34,
    eManufacturerTheHurtBox = 35,
    eManufacturerCitizenSystems = 36,
    eManufacturerMagellan = 37,
    eManufacturerOsynce = 38,
    eManufacturerHolux = 39,
    eManufacturerConcept2 = 40,
    eManufacturerOneGiantLeap = 42,
    eManufacturerAceSensor = 43,
    eManufacturerBrimBrothers = 44,
    eManufacturerXplova = 45,
    eManufacturerPerceptionDigital = 46,
    eManufacturerBf1systems = 47,
    eManufacturerPioneer = 48,
    eManufacturerSpantec = 49,
    eManufacturerMetalogics = 50,
    eManufacturer4iiiis = 51,
    eManufacturerSeikoEpson = 52,
    eManufacturerSeikoEpsonOem = 53,
    eManufacturerIforPowell = 54,
    eManufacturerMaxwellGuider = 55,
    eManufacturerStarTrac = 56,
    eManufacturerBreakaway = 57,
    eManufacturerAlatechTechnologyLtd = 58,
    eManufacturerMioTechnologyEurope = 59,
    eManufacturerRotor = 60,
    eManufacturerGeonaute = 61,
    eManufacturerIdBike = 62,
    eManufacturerSpecialized = 63,
    eManufacturerWtek = 64,
    eManufacturerPhysicalEnterprises = 65,
    eManufacturerNorthPoleEngineering = 66,
    eManufacturerBkool = 67,
    eManufacturerCateye = 68,
    eManufacturerStagesCycling = 69,
    eManufacturerSigmasport = 70,
    eManufacturerTomtom = 71,
    eManufacturerPeripedal = 72,
    eManufacturerWattbike = 73,
    eManufacturerMoxy = 76,
    eManufacturerCiclosport = 77,
    eManufacturerPowerbahn = 78,
    eManufacturerAcornProjectsAps = 79,
    eManufacturerLifebeam = 80,
    eManufacturerBontrager = 81,
    eManufacturerWellgo = 82,
    eManufacturerScosche = 83,
    eManufacturerMagura = 84,
    eManufacturerWoodway = 85,
    eManufacturerElite = 86,
    eManufacturerNielsenKellerman = 87,
    eManufacturerDkCity = 88,
    eManufacturerTacx = 89,
    eManufacturerDirectionTechnology = 90,
    eManufacturerMagtonic = 91,
    eManufacturer1partcarbon = 92,
    eManufacturerInsideRideTechnologies = 93,
    eManufacturerSoundOfMotion = 94,
    eManufacturerStryd = 95,
    eManufacturerIcg = 96,
    eManufacturerMiPulse = 97,
    eManufacturerBsxAthletics = 98,
    eManufacturerLook = 99,
    eManufacturerDevelopment = 255,
    eManufacturerHealthandlife = 257,
    eManufacturerLezyne = 258,
    eManufacturerScribeLabs = 259,
    eManufacturerZwift = 260,
    eManufacturerWatteam = 261,
    eManufacturerRecon = 262,
    eManufacturerFaveroElectronics = 263,
    eManufacturerDynovelo = 264,
    eManufacturerStrava = 265,
    eManufacturerActigraphcorp = 5759
} manufacturer_e;

typedef enum {
    eGarminProductHrm1 = 1,
    eGarminProductAxh01 = 2,
    eGarminProductAxb01 = 3,
    eGarminProductAxb02 = 4,
    eGarminProductHrm2ss = 5,
    eGarminProductDsiAlf02 = 6,
    eGarminProductHrm3ss = 7,
    eGarminProductHrmRunSingleByteProductId = 8,
    eGarminProductBsm = 9,
    eGarminProductBcm = 10,
    eGarminProductAxs01 = 11,
    eGarminProductHrmTriSingleByteProductId = 12,
    eGarminProductFr225SingleByteProductId = 14,
    eGarminProductFr301China = 473,
    eGarminProductFr301Japan = 474,
    eGarminProductFr301Korea = 475,
    eGarminProductFr301Taiwan = 494,
    eGarminProductFr405 = 717,
    eGarminProductFr50 = 782,
    eGarminProductFr405Japan = 987,
    eGarminProductFr60 = 988,
    eGarminProductDsiAlf01 = 1011,
    eGarminProductFr310xt = 1018,
    eGarminProductEdge500 = 1036,
    eGarminProductFr110 = 1124,
    eGarminProductEdge800 = 1169,
    eGarminProductEdge500Taiwan = 1199,
    eGarminProductEdge500Japan = 1213,
    eGarminProductChirp = 1253,
    eGarminProductFr110Japan = 1274,
    eGarminProductEdge200 = 1325,
    eGarminProductFr910xt = 1328,
    eGarminProductEdge800Taiwan = 1333,
    eGarminProductEdge800Japan = 1334,
    eGarminProductAlf04 = 1341,
    eGarminProductFr610 = 1345,
    eGarminProductFr210Japan = 1360,
    eGarminProductVectorSs = 1380,
    eGarminProductVectorCp = 1381,
    eGarminProductEdge800China = 1386,
    eGarminProductEdge500China = 1387,
    eGarminProductFr610Japan = 1410,
    eGarminProductEdge500Korea = 1422,
    eGarminProductFr70 = 1436,
    eGarminProductFr310xt4t = 1446,
    eGarminProductAmx = 1461,
    eGarminProductFr10 = 1482,
    eGarminProductEdge800Korea = 1497,
    eGarminProductSwim = 1499,
    eGarminProductFr910xtChina = 1537,
    eGarminProductFenix = 1551,
    eGarminProductEdge200Taiwan = 1555,
    eGarminProductEdge510 = 1561,
    eGarminProductEdge810 = 1567,
    eGarminProductTempe = 1570,
    eGarminProductFr910xtJapan = 1600,
    eGarminProductFr620 = 1623,
    eGarminProductFr220 = 1632,
    eGarminProductFr910xtKorea = 1664,
    eGarminProductFr10Japan = 1688,
    eGarminProductEdge810Japan = 1721,
    eGarminProductVirbElite = 1735,
    eGarminProductEdgeTouring = 1736,
    eGarminProductEdge510Japan = 1742,
    eGarminProductHrmTri = 1743,
    eGarminProductHrmRun = 1752,
    eGarminProductFr920xt = 1765,
    eGarminProductEdge510Asia = 1821,
    eGarminProductEdge810China = 1822,
    eGarminProductEdge810Taiwan = 1823,
    eGarminProductEdge1000 = 1836,
    eGarminProductVivoFit = 1837,
    eGarminProductVirbRemote = 1853,
    eGarminProductVivoKi = 1885,
    eGarminProductFr15 = 1903,
    eGarminProductVivoActive = 1907,
    eGarminProductEdge510Korea = 1918,
    eGarminProductFr620Japan = 1928,
    eGarminProductFr620China = 1929,
    eGarminProductFr220Japan = 1930,
    eGarminProductFr220China = 1931,
    eGarminProductApproachS6 = 1936,
    eGarminProductVivoSmart = 1956,
    eGarminProductFenix2 = 1967,
    eGarminProductEpix = 1988,
    eGarminProductFenix3 = 2050,
    eGarminProductEdge1000Taiwan = 2052,
    eGarminProductEdge1000Japan = 2053,
    eGarminProductFr15Japan = 2061,
    eGarminProductEdge520 = 2067,
    eGarminProductEdge1000China = 2070,
    eGarminProductFr620Russia = 2072,
    eGarminProductFr220Russia = 2073,
    eGarminProductVectorS = 2079,
    eGarminProductEdge1000Korea = 2100,
    eGarminProductFr920xtTaiwan = 2130,
    eGarminProductFr920xtChina = 2131,
    eGarminProductFr920xtJapan = 2132,
    eGarminProductVirbx = 2134,
    eGarminProductVivoSmartApac = 2135,
    eGarminProductEtrexTouch = 2140,
    eGarminProductEdge25 = 2147,
    eGarminProductVivoFit2 = 2150,
    eGarminProductFr225 = 2153,
    eGarminProductVivoActiveApac = 2160,
    eGarminProductVector2 = 2161,
    eGarminProductVector2s = 2162,
    eGarminProductVirbxe = 2172,
    eGarminProductFr620Taiwan = 2173,
    eGarminProductFr220Taiwan = 2174,
    eGarminProductFenix3China = 2188,
    eGarminProductFenix3Twn = 2189,
    eGarminProductVariaHeadlight = 2192,
    eGarminProductVariaTaillightOld = 2193,
    eGarminProductFr225Asia = 2219,
    eGarminProductVariaRadarTaillight = 2225,
    eGarminProductVariaRadarDisplay = 2226,
    eGarminProductEdge20 = 2238,
    eGarminProductD2Bravo = 2262,
    eGarminProductVariaRemote = 2276,
    eGarminProductSdm4 = 10007,
    eGarminProductEdgeRemote = 10014,
    eGarminProductTrainingCenter = 20119,
    eGarminProductAndroidAntplusPlugin = 65532,
    eGarminProductConnect = 65534
} garmin_product_e;

typedef enum {
    eAntplusDeviceTypeAntfs = 1,
    eAntplusDeviceTypeBikePower = 11,
    eAntplusDeviceTypeEnvironmentSensorLegacy = 12,
    eAntplusDeviceTypeMultiSportSpeedDistance = 15,
    eAntplusDeviceTypeControl = 16,
    eAntplusDeviceTypeFitnessEquipment = 17,
    eAntplusDeviceTypeBloodPressure = 18,
    eAntplusDeviceTypeGeocacheNode = 19,
    eAntplusDeviceTypeLightElectricVehicle = 20,
    eAntplusDeviceTypeEnvSensor = 25,
    eAntplusDeviceTypeRacquet = 26,
    eAntplusDeviceTypeWeightScale = 119,
    eAntplusDeviceTypeHeartRate = 120,
    eAntplusDeviceTypeBikeSpeedCadence = 121,
    eAntplusDeviceTypeBikeCadence = 122,
    eAntplusDeviceTypeBikeSpeed = 123,
    eAntplusDeviceTypeStrideSpeedDistance = 124
} antplus_device_type_e;

typedef enum {
    eAntNetworkPublic = 0,
    eAntNetworkAntplus = 1,
    eAntNetworkAntfs = 2,
    eAntNetworkPrivate = 3
} ant_network_e;

typedef enum {
    eWorkoutCapabilitiesInterval = 0x00000001,
    eWorkoutCapabilitiesCustom = 0x00000002,
    eWorkoutCapabilitiesFitnessEquipment = 0x00000004,
    eWorkoutCapabilitiesFirstbeat = 0x00000008,
    eWorkoutCapabilitiesNewLeaf = 0x00000010,
    eWorkoutCapabilitiesTcx = 0x00000020,
    eWorkoutCapabilitiesSpeed = 0x00000080,
    eWorkoutCapabilitiesHeartRate = 0x00000100,
    eWorkoutCapabilitiesDistance = 0x00000200,
    eWorkoutCapabilitiesCadence = 0x00000400,
    eWorkoutCapabilitiesPower = 0x00000800,
    eWorkoutCapabilitiesGrade = 0x00001000,
    eWorkoutCapabilitiesResistance = 0x00002000,
    eWorkoutCapabilitiesProtected = 0x00004000
} workout_capabilities_e;

typedef enum {
    eBatteryStatusNew = 1,
    eBatteryStatusGood = 2,
    eBatteryStatusOk = 3,
    eBatteryStatusLow = 4,
    eBatteryStatusCritical = 5,
    eBatteryStatusUnknown = 7
} battery_status_e;

typedef enum {
    eHrTypeNormal = 0,
    eHrTypeIrregular = 1
} hr_type_e;

typedef enum {
    eCourseCapabilitiesProcessed = 0x00000001,
    eCourseCapabilitiesValid = 0x00000002,
    eCourseCapabilitiesTime = 0x00000004,
    eCourseCapabilitiesDistance = 0x00000008,
    eCourseCapabilitiesPosition = 0x00000010,
    eCourseCapabilitiesHeartRate = 0x00000020,
    eCourseCapabilitiesPower = 0x00000040,
    eCourseCapabilitiesCadence = 0x00000080,
    eCourseCapabilitiesTraining = 0x00000100,
    eCourseCapabilitiesNavigation = 0x00000200,
    eCourseCapabilitiesBikeway = 0x00000400
} course_capabilities_e;

typedef enum {
    eWeightCalculating = 0xFFFE
} weight_e;

typedef enum {
    eWorkoutHrBpmOffset = 100
} workout_hr_e;

typedef enum {
    eWorkoutPowerWattsOffset = 1000
} workout_power_e;

typedef enum {
    eBpStatusNoError = 0,
    eBpStatusErrorIncompleteData = 1,
    eBpStatusErrorNoMeasurement = 2,
    eBpStatusErrorDataOutOfRange = 3,
    eBpStatusErrorIrregularHeartRate = 4
} bp_status_e;

typedef enum {
    eUserLocalIdLocalMin = 0x0000,
    eUserLocalIdLocalMax = 0x000F,
    eUserLocalIdStationaryMin = 0x0010,
    eUserLocalIdStationaryMax = 0x00FF,
    eUserLocalIdPortableMin = 0x0100,
    eUserLocalIdPortableMax = 0xFFFE
} user_local_id_e;

typedef enum {
    eSwimStrokeFreestyle = 0,
    eSwimStrokeBackstroke = 1,
    eSwimStrokeBreaststroke = 2,
    eSwimStrokeButterfly = 3,
    eSwimStrokeDrill = 4,
    eSwimStrokeMixed = 5,
    eSwimStrokeIm = 6
} swim_stroke_e;

typedef enum {
    eActivityTypeGeneric = 0,
    eActivityTypeRunning = 1,
    eActivityTypeCycling = 2,
    eActivityTypeTransition = 3,
    eActivityTypeFitnessEquipment = 4,
    eActivityTypeSwimming = 5,
    eActivityTypeWalking = 6,
    eActivityTypeAll = 254
} activity_type_e;

typedef enum {
    eActivitySubtypeGeneric = 0,
    eActivitySubtypeTreadmill = 1,
    eActivitySubtypeStreet = 2,
    eActivitySubtypeTrail = 3,
    eActivitySubtypeTrack = 4,
    eActivitySubtypeSpin = 5,
    eActivitySubtypeIndoorCycling = 6,
    eActivitySubtypeRoad = 7,
    eActivitySubtypeMountain = 8,
    eActivitySubtypeDownhill = 9,
    eActivitySubtypeRecumbent = 10,
    eActivitySubtypeCyclocross = 11,
    eActivitySubtypeHandCycling = 12,
    eActivitySubtypeTrackCycling = 13,
    eActivitySubtypeIndoorRowing = 14,
    eActivitySubtypeElliptical = 15,
    eActivitySubtypeStairClimbing = 16,
    eActivitySubtypeLapSwimming = 17,
    eActivitySubtypeOpenWater = 18,
    eActivitySubtypeAll = 254
} activity_subtype_e;

typedef enum {
    eActivityLevelLow = 0,
    eActivityLevelMedium = 1,
    eActivityLevelHigh = 2
} activity_level_e;

typedef enum {
    eLeftRightBalanceMask = 0x7F,
    eLeftRightBalanceRight = 0x80
} left_right_balance_e;

typedef enum {
    eLeftRightBalance100Mask = 0x3FFF,
    eLeftRightBalance100Right = 0x8000
} left_right_balance_100_e;

typedef enum {
    eLengthTypeIdle = 0,
    eLengthTypeActive = 1
} length_type_e;

typedef enum {
    eConnectivityCapabilitiesBluetooth = 0x00000001,
    eConnectivityCapabilitiesBluetoothLe = 0x00000002,
    eConnectivityCapabilitiesAnt = 0x00000004,
    eConnectivityCapabilitiesActivityUpload = 0x00000008,
    eConnectivityCapabilitiesCourseDownload = 0x00000010,
    eConnectivityCapabilitiesWorkoutDownload = 0x00000020,
    eConnectivityCapabilitiesLiveTrack = 0x00000040,
    eConnectivityCapabilitiesWeatherConditions = 0x00000080,
    eConnectivityCapabilitiesWeatherAlerts = 0x00000100,
    eConnectivityCapabilitiesGpsEphemerisDownload = 0x00000200,
    eConnectivityCapabilitiesExplicitArchive = 0x00000400,
    eConnectivityCapabilitiesSetupIncomplete = 0x00000800,
    eConnectivityCapabilitiesContinueSyncAfterSoftwareUpdate = 0x00001000,
    eConnectivityCapabilitiesConnectIqAppDownload = 0x00002000
} connectivity_capabilities_e;

typedef enum {
    eStrokeTypeNoEvent = 0,
    eStrokeTypeOther = 1,
    eStrokeTypeServe = 2,
    eStrokeTypeForehand = 3,
    eStrokeTypeBackhand = 4,
    eStrokeTypeSmash = 5
} stroke_type_e;

typedef enum {
    eBodyLocationLeftLeg = 0,
    eBodyLocationLeftCalf = 1,
    eBodyLocationLeftShin = 2,
    eBodyLocationLeftHamstring = 3,
    eBodyLocationLeftQuad = 4,
    eBodyLocationLeftGlute = 5,
    eBodyLocationRightLeg = 6,
    eBodyLocationRightCalf = 7,
    eBodyLocationRightShin = 8,
    eBodyLocationRightHamstring = 9,
    eBodyLocationRightQuad = 10,
    eBodyLocationRightGlute = 11,
    eBodyLocationTorsoBack = 12,
    eBodyLocationLeftLowerBack = 13,
    eBodyLocationLeftUpperBack = 14,
    eBodyLocationRightLowerBack = 15,
    eBodyLocationRightUpperBack = 16,
    eBodyLocationTorsoFront = 17,
    eBodyLocationLeftAbdomen = 18,
    eBodyLocationLeftChest = 19,
    eBodyLocationRightAbdomen = 20,
    eBodyLocationRightChest = 21,
    eBodyLocationLeftArm = 22,
    eBodyLocationLeftShoulder = 23,
    eBodyLocationLeftBicep = 24,
    eBodyLocationLeftTricep = 25,
    eBodyLocationLeftBrachioradialis = 26,
    eBodyLocationLeftForearmExtensors = 27,
    eBodyLocationRightArm = 28,
    eBodyLocationRightShoulder = 29,
    eBodyLocationRightBicep = 30,
    eBodyLocationRightTricep = 31,
    eBodyLocationRightBrachioradialis = 32,
    eBodyLocationRightForearmExtensors = 33,
    eBodyLocationNeck = 34,
    eBodyLocationThroat = 35
} body_location_e;

typedef enum {
    eSegmentLapStatusEnd = 0,
    eSegmentLapStatusFail = 1
} segment_lap_status_e;

typedef enum {
    eSegmentLeaderboardTypeOverall = 0,
    eSegmentLeaderboardTypePersonalBest = 1,
    eSegmentLeaderboardTypeConnections = 2,
    eSegmentLeaderboardTypeGroup = 3,
    eSegmentLeaderboardTypeChallenger = 4,
    eSegmentLeaderboardTypeKom = 5,
    eSegmentLeaderboardTypeQom = 6,
    eSegmentLeaderboardTypePr = 7,
    eSegmentLeaderboardTypeGoal = 8,
    eSegmentLeaderboardTypeRival = 9,
    eSegmentLeaderboardTypeClubLeader = 10
} segment_leaderboard_type_e;

typedef enum {
    eSegmentDeleteStatusDoNotDelete = 0,
    eSegmentDeleteStatusDeleteOne = 1,
    eSegmentDeleteStatusDeleteAll = 2
} segment_delete_status_e;

typedef enum {
    eSegmentSelectionTypeStarred = 0,
    eSegmentSelectionTypeSuggested = 1
} segment_selection_type_e;

typedef enum {
    eSourceTypeAnt = 0,
    eSourceTypeAntplus = 1,
    eSourceTypeBluetooth = 2,
    eSourceTypeBluetoothLowEnergy = 3,
    eSourceTypeWifi = 4,
    eSourceTypeLocal = 5
} source_type_e;

typedef enum {
    eRiderPositionTypeSeated = 0,
    eRiderPositionTypeStanding = 1
} rider_position_type_e;

typedef enum {
    ePowerPhaseTypePowerPhaseStartAngle = 0,
    ePowerPhaseTypePowerPhaseEndAngle = 1,
    ePowerPhaseTypePowerPhaseArcLength = 2,
    ePowerPhaseTypePowerPhaseCenter = 3
} power_phase_type_e;

typedef enum {
    eCameraEventTypeVideoStart = 0,
    eCameraEventTypeVideoSplit = 1,
    eCameraEventTypeVideoEnd = 2,
    eCameraEventTypePhotoTaken = 3,
    eCameraEventTypeVideoSecondStreamStart = 4,
    eCameraEventTypeVideoSecondStreamSplit = 5,
    eCameraEventTypeVideoSecondStreamEnd = 6,
    eCameraEventTypeVideoSplitStart = 7,
    eCameraEventTypeVideoSecondStreamSplitStart = 8
} camera_event_type_e;

typedef enum {
    eSensorTypeAccelerometer = 0,
    eSensorTypeGyroscope = 1,
    eSensorTypeCompass = 2
} sensor_type_e;

typedef enum {
    eCommTimeoutTypeWildcardPairingTimeout = 0,
    eCommTimeoutTypePairingTimeout = 1,
    eCommTimeoutTypeConnectionLost = 2,
} comm_timeout_type_e;

    typedef enum {
        eCameraOrientationTypeCameraOrientation0 = 0,
        eCameraOrientationTypeCameraOrientation90 = 1,
        eCameraOrientationTypeCameraOrientation180 = 2,
        eCameraOrientationTypeCameraOrientation270 = 3
    } camera_orientation_type_e;

    typedef enum {
        eAttitudeStageFailed = 0,
        eAttitudeStageAligning = 1,
        eAttitudeStageDegraded = 2,
        eAttitudeStageValid = 3
    } attitude_stage_e;

    typedef enum {
        eAttitudeValidityTrackAngleHeadingValid = 0x0001,
        eAttitudeValidityPitchValid = 0x0002,
        eAttitudeValidityRollValid = 0x0004,
        eAttitudeValidityLateralBodyAccelValid = 0x0008,
        eAttitudeValidityNormalBodyAccelValid = 0x0010,
        eAttitudeValidityTurnRateValid = 0x0020,
        eAttitudeValidityHwFail = 0x0040,
        eAttitudeValidityMagInvalid = 0x0080,
        eAttitudeValidityNoGps = 0x0100,
        eAttitudeValidityGpsInvalid = 0x0200,
        eAttitudeValiditySolutionCoasting = 0x0400,
        eAttitudeValidityTrueTrackAngle = 0x0800,
        eAttitudeValidityMagneticHeading = 0x1000
    } attitude_validity_e;

// ----------- end generated code -----------

#endif // FIT_PROFILE_H
