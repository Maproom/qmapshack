/**********************************************************************************************
   Copyright (C) 2015 Ivo Kronenberg <>

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

typedef enum
{
    eLocationName = 0,
    eLocationPositionLat = 1,
    eLocationPositionLong = 2,
    eLocationSymbol = 3,
    eLocationAltitude = 4,
    eLocation5 = 5,
    eLocationComment = 6,
    eLocationTimestamp = 253,
    eLocationMessageIndex = 254
} message_location_e;

typedef enum
{
    eMesgNumLocation = 29
} ext_mesg_num_e;

typedef enum
{
    eFileLocation = 8
} ext_file_e;

// ----------- start generated code -----------
typedef enum
{
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
    eFileExdConfiguration = 40,
    eFileMfgRangeMin = 0xF7,
    eFileMfgRangeMax = 0xFE
} file_e;

typedef enum
{
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
    eMesgNumAntRx = 80,
    eMesgNumAntTx = 81,
    eMesgNumAntChannelId = 82,
    eMesgNumLength = 101,
    eMesgNumMonitoringInfo = 103,
    eMesgNumPad = 105,
    eMesgNumSlaveDevice = 106,
    eMesgNumConnectivity = 127,
    eMesgNumWeatherConditions = 128,
    eMesgNumWeatherAlert = 129,
    eMesgNumCadenceZone = 131,
    eMesgNumHr = 132,
    eMesgNumSegmentLap = 142,
    eMesgNumMemoGlob = 145,
    eMesgNumSegmentId = 148,
    eMesgNumSegmentLeaderboardEntry = 149,
    eMesgNumSegmentPoint = 150,
    eMesgNumSegmentFile = 151,
    eMesgNumWatchfaceSettings = 159,
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
    eMesgNumOhrSettings = 188,
    eMesgNumExdScreenConfiguration = 200,
    eMesgNumExdDataFieldConfiguration = 201,
    eMesgNumExdDataConceptConfiguration = 202,
    eMesgNumFieldDescription = 206,
    eMesgNumDeveloperDataId = 207,
    eMesgNumMagnetometerData = 208,
    eMesgNumMfgRangeMin = 0xFF00,
    eMesgNumMfgRangeMax = 0xFFFE
} mesg_num_e;

typedef enum
{
    eChecksumClear = 0,
    eChecksumOk = 1
} checksum_e;

typedef enum
{
    eFileFlagsRead = 0x02,
    eFileFlagsWrite = 0x04,
    eFileFlagsErase = 0x08
} file_flags_e;

typedef enum
{
    eMesgCountNumPerFile = 0,
    eMesgCountMaxPerFile = 1,
    eMesgCountMaxPerFileType = 2
} mesg_count_e;

typedef enum
{
    eDateTimeMin = 0x10000000
} date_time_e;

typedef enum
{
    eLocalDateTimeMin = 0x10000000
} local_date_time_e;

typedef enum
{
    eMessageIndexSelected = 0x8000,
    eMessageIndexReserved = 0x7000,
    eMessageIndexMask = 0x0FFF
} message_index_e;

typedef enum
{
    eDeviceIndexCreator = 0
} device_index_e;

typedef enum
{
    eGenderFemale = 0,
    eGenderMale = 1
} gender_e;

typedef enum
{
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
    eLanguageChinese = 26,
    eLanguageJapanese = 27,
    eLanguageKorean = 28,
    eLanguageTaiwanese = 29,
    eLanguageThai = 30,
    eLanguageHebrew = 31,
    eLanguageBrazilianPortuguese = 32,
    eLanguageIndonesian = 33,
    eLanguageMalaysian = 34,
    eLanguageVietnamese = 35,
    eLanguageBurmese = 36,
    eLanguageMongolian = 37,
    eLanguageCustom = 254
} language_e;

typedef enum
{
    eLanguageBits0English = 0x01,
    eLanguageBits0French = 0x02,
    eLanguageBits0Italian = 0x04,
    eLanguageBits0German = 0x08,
    eLanguageBits0Spanish = 0x10,
    eLanguageBits0Croatian = 0x20,
    eLanguageBits0Czech = 0x40,
    eLanguageBits0Danish = 0x80
} language_bits_0_e;

typedef enum
{
    eLanguageBits1Dutch = 0x01,
    eLanguageBits1Finnish = 0x02,
    eLanguageBits1Greek = 0x04,
    eLanguageBits1Hungarian = 0x08,
    eLanguageBits1Norwegian = 0x10,
    eLanguageBits1Polish = 0x20,
    eLanguageBits1Portuguese = 0x40,
    eLanguageBits1Slovakian = 0x80
} language_bits_1_e;

typedef enum
{
    eLanguageBits2Slovenian = 0x01,
    eLanguageBits2Swedish = 0x02,
    eLanguageBits2Russian = 0x04,
    eLanguageBits2Turkish = 0x08,
    eLanguageBits2Latvian = 0x10,
    eLanguageBits2Ukrainian = 0x20,
    eLanguageBits2Arabic = 0x40,
    eLanguageBits2Farsi = 0x80
} language_bits_2_e;

typedef enum
{
    eLanguageBits3Bulgarian = 0x01,
    eLanguageBits3Romanian = 0x02,
    eLanguageBits3Chinese = 0x04,
    eLanguageBits3Japanese = 0x08,
    eLanguageBits3Korean = 0x10,
    eLanguageBits3Taiwanese = 0x20,
    eLanguageBits3Thai = 0x40,
    eLanguageBits3Hebrew = 0x80
} language_bits_3_e;

typedef enum
{
    eLanguageBits4BrazilianPortuguese = 0x01,
    eLanguageBits4Indonesian = 0x02,
    eLanguageBits4Malaysian = 0x04,
    eLanguageBits4Vietnamese = 0x08,
    eLanguageBits4Burmese = 0x10,
    eLanguageBits4Mongolian = 0x20
} language_bits_4_e;

typedef enum
{
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

typedef enum
{
    eDisplayMeasureMetric = 0,
    eDisplayMeasureStatute = 1,
    eDisplayMeasureNautical = 2
} display_measure_e;

typedef enum
{
    eDisplayHeartBpm = 0,
    eDisplayHeartMax = 1,
    eDisplayHeartReserve = 2
} display_heart_e;

typedef enum
{
    eDisplayPowerWatts = 0,
    eDisplayPowerPercentFtp = 1
} display_power_e;

typedef enum
{
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
    eDisplayPositionIndiaZoneIa = 14,
    eDisplayPositionIndiaZoneIb = 15,
    eDisplayPositionIndiaZoneIia = 16,
    eDisplayPositionIndiaZoneIib = 17,
    eDisplayPositionIndiaZoneIiia = 18,
    eDisplayPositionIndiaZoneIiib = 19,
    eDisplayPositionIndiaZoneIva = 20,
    eDisplayPositionIndiaZoneIvb = 21,
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

typedef enum
{
    eSwitchOff = 0,
    eSwitchOn = 1,
    eSwitchAuto = 2
} switch_e;

typedef enum
{
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
    eSportTactical = 45,
    eSportJumpmaster = 46,
    eSportBoxing = 47,
    eSportFloorClimbing = 48,
    eSportAll = 254
} sport_e;

typedef enum
{
    eSportBits0Generic = 0x01,
    eSportBits0Running = 0x02,
    eSportBits0Cycling = 0x04,
    eSportBits0Transition = 0x08,
    eSportBits0FitnessEquipment = 0x10,
    eSportBits0Swimming = 0x20,
    eSportBits0Basketball = 0x40,
    eSportBits0Soccer = 0x80
} sport_bits_0_e;

typedef enum
{
    eSportBits1Tennis = 0x01,
    eSportBits1AmericanFootball = 0x02,
    eSportBits1Training = 0x04,
    eSportBits1Walking = 0x08,
    eSportBits1CrossCountrySkiing = 0x10,
    eSportBits1AlpineSkiing = 0x20,
    eSportBits1Snowboarding = 0x40,
    eSportBits1Rowing = 0x80
} sport_bits_1_e;

typedef enum
{
    eSportBits2Mountaineering = 0x01,
    eSportBits2Hiking = 0x02,
    eSportBits2Multisport = 0x04,
    eSportBits2Paddling = 0x08,
    eSportBits2Flying = 0x10,
    eSportBits2EBiking = 0x20,
    eSportBits2Motorcycling = 0x40,
    eSportBits2Boating = 0x80
} sport_bits_2_e;

typedef enum
{
    eSportBits3Driving = 0x01,
    eSportBits3Golf = 0x02,
    eSportBits3HangGliding = 0x04,
    eSportBits3HorsebackRiding = 0x08,
    eSportBits3Hunting = 0x10,
    eSportBits3Fishing = 0x20,
    eSportBits3InlineSkating = 0x40,
    eSportBits3RockClimbing = 0x80
} sport_bits_3_e;

typedef enum
{
    eSportBits4Sailing = 0x01,
    eSportBits4IceSkating = 0x02,
    eSportBits4SkyDiving = 0x04,
    eSportBits4Snowshoeing = 0x08,
    eSportBits4Snowmobiling = 0x10,
    eSportBits4StandUpPaddleboarding = 0x20,
    eSportBits4Surfing = 0x40,
    eSportBits4Wakeboarding = 0x80
} sport_bits_4_e;

typedef enum
{
    eSportBits5WaterSkiing = 0x01,
    eSportBits5Kayaking = 0x02,
    eSportBits5Rafting = 0x04,
    eSportBits5Windsurfing = 0x08,
    eSportBits5Kitesurfing = 0x10,
    eSportBits5Tactical = 0x20,
    eSportBits5Jumpmaster = 0x40,
    eSportBits5Boxing = 0x80
} sport_bits_5_e;

typedef enum
{
    eSportBits6FloorClimbing = 0x01
} sport_bits_6_e;

typedef enum
{
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
    eSubSportSkateSkiing = 42,
    eSubSportYoga = 43,
    eSubSportPilates = 44,
    eSubSportIndoorRunning = 45,
    eSubSportGravelCycling = 46,
    eSubSportEBikeMountain = 47,
    eSubSportCommuting = 48,
    eSubSportMixedSurface = 49,
    eSubSportNavigate = 50,
    eSubSportTrackMe = 51,
    eSubSportMap = 52,
    eSubSportAll = 254
} sub_sport_e;

typedef enum
{
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

typedef enum
{
    eActivityManual = 0,
    eActivityAutoMultiSport = 1
} activity_e;

typedef enum
{
    eIntensityActive = 0,
    eIntensityRest = 1,
    eIntensityWarmup = 2,
    eIntensityCooldown = 3
} intensity_e;

typedef enum
{
    eSessionTriggerActivityEnd = 0,
    eSessionTriggerManual = 1,
    eSessionTriggerAutoMultiSport = 2,
    eSessionTriggerFitnessEquipment = 3
} session_trigger_e;

typedef enum
{
    eAutolapTriggerTime = 0,
    eAutolapTriggerDistance = 1,
    eAutolapTriggerPositionStart = 2,
    eAutolapTriggerPositionLap = 3,
    eAutolapTriggerPositionWaypoint = 4,
    eAutolapTriggerPositionMarked = 5,
    eAutolapTriggerOff = 6
} autolap_trigger_e;

typedef enum
{
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

typedef enum
{
    eTimeModeHour12 = 0,
    eTimeModeHour24 = 1,
    eTimeModeMilitary = 2,
    eTimeModeHour12WithSeconds = 3,
    eTimeModeHour24WithSeconds = 4,
    eTimeModeUtc = 5
} time_mode_e;

typedef enum
{
    eBacklightModeOff = 0,
    eBacklightModeManual = 1,
    eBacklightModeKeyAndMessages = 2,
    eBacklightModeAutoBrightness = 3,
    eBacklightModeSmartNotifications = 4,
    eBacklightModeKeyAndMessagesNight = 5,
    eBacklightModeKeyAndMessagesAndSmartNotifications = 6
} backlight_mode_e;

typedef enum
{
    eDateModeDayMonth = 0,
    eDateModeMonthDay = 1
} date_mode_e;

typedef enum
{
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

typedef enum
{
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

typedef enum
{
    eTimerTriggerManual = 0,
    eTimerTriggerAuto = 1,
    eTimerTriggerFitnessEquipment = 2
} timer_trigger_e;

typedef enum
{
    eFitnessEquipmentStateReady = 0,
    eFitnessEquipmentStateInUse = 1,
    eFitnessEquipmentStatePaused = 2,
    eFitnessEquipmentStateUnknown = 3
} fitness_equipment_state_e;

typedef enum
{
    eAutoscrollNone = 0,
    eAutoscrollSlow = 1,
    eAutoscrollMedium = 2,
    eAutoscrollFast = 3
} autoscroll_e;

typedef enum
{
    eActivityClassLevel = 0x7F,
    eActivityClassLevelMax = 100,
    eActivityClassAthlete = 0x80
} activity_class_e;

typedef enum
{
    eHrZoneCalcCustom = 0,
    eHrZoneCalcPercentMaxHr = 1,
    eHrZoneCalcPercentHrr = 2
} hr_zone_calc_e;

typedef enum
{
    ePwrZoneCalcCustom = 0,
    ePwrZoneCalcPercentFtp = 1
} pwr_zone_calc_e;

typedef enum
{
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
    eWktStepDurationTrainingPeaksTss = 16,
    eWktStepDurationRepeatUntilPowerLastLapLessThan = 17,
    eWktStepDurationRepeatUntilMaxPowerLastLapLessThan = 18,
    eWktStepDurationPower3SLessThan = 19,
    eWktStepDurationPower10SLessThan = 20,
    eWktStepDurationPower30SLessThan = 21,
    eWktStepDurationPower3SGreaterThan = 22,
    eWktStepDurationPower10SGreaterThan = 23,
    eWktStepDurationPower30SGreaterThan = 24,
    eWktStepDurationPowerLapLessThan = 25,
    eWktStepDurationPowerLapGreaterThan = 26,
    eWktStepDurationRepeatUntilTrainingPeaksTss = 27,
    eWktStepDurationRepetitionTime = 28
} wkt_step_duration_e;

typedef enum
{
    eWktStepTargetSpeed = 0,
    eWktStepTargetHeartRate = 1,
    eWktStepTargetOpen = 2,
    eWktStepTargetCadence = 3,
    eWktStepTargetPower = 4,
    eWktStepTargetGrade = 5,
    eWktStepTargetResistance = 6,
    eWktStepTargetPower3S = 7,
    eWktStepTargetPower10S = 8,
    eWktStepTargetPower30S = 9,
    eWktStepTargetPowerLap = 10,
    eWktStepTargetSpeedLap = 12,
    eWktStepTargetHeartRateLap = 13
} wkt_step_target_e;

typedef enum
{
    eGoalTime = 0,
    eGoalDistance = 1,
    eGoalCalories = 2,
    eGoalFrequency = 3,
    eGoalSteps = 4,
    eGoalAscent = 5,
    eGoalActiveMinutes = 6
} goal_e;

typedef enum
{
    eGoalRecurrenceOff = 0,
    eGoalRecurrenceDaily = 1,
    eGoalRecurrenceWeekly = 2,
    eGoalRecurrenceMonthly = 3,
    eGoalRecurrenceYearly = 4,
    eGoalRecurrenceCustom = 5
} goal_recurrence_e;

typedef enum
{
    eGoalSourceAuto = 0,
    eGoalSourceCommunity = 1,
    eGoalSourceUser = 2
} goal_source_e;

typedef enum
{
    eScheduleWorkout = 0,
    eScheduleCourse = 1
} schedule_e;

typedef enum
{
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
    eCoursePointUTurn = 23,
    eCoursePointSegmentStart = 24,
    eCoursePointSegmentEnd = 25
} course_point_e;

typedef enum
{
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
    eManufacturerBf1Systems = 47,
    eManufacturerPioneer = 48,
    eManufacturerSpantec = 49,
    eManufacturerMetalogics = 50,
    eManufacturer4Iiiis = 51,
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
    eManufacturer1Partcarbon = 92,
    eManufacturerInsideRideTechnologies = 93,
    eManufacturerSoundOfMotion = 94,
    eManufacturerStryd = 95,
    eManufacturerIcg = 96,
    eManufacturerMipulse = 97,
    eManufacturerBsxAthletics = 98,
    eManufacturerLook = 99,
    eManufacturerCampagnoloSrl = 100,
    eManufacturerBodyBikeSmart = 101,
    eManufacturerPraxisworks = 102,
    eManufacturerLimitsTechnology = 103,
    eManufacturerTopactionTechnology = 104,
    eManufacturerCosinuss = 105,
    eManufacturerFitcare = 106,
    eManufacturerMagene = 107,
    eManufacturerGiantManufacturingCo = 108,
    eManufacturerTigrasport = 109,
    eManufacturerSalutron = 110,
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
    eManufacturerPrecor = 266,
    eManufacturerBryton = 267,
    eManufacturerSram = 268,
    eManufacturerNavman = 269,
    eManufacturerCobi = 270,
    eManufacturerSpivi = 271,
    eManufacturerMioMagellan = 272,
    eManufacturerEvesports = 273,
    eManufacturerSensitivusGauge = 274,
    eManufacturerPodoon = 275,
    eManufacturerLifeTimeFitness = 276,
    eManufacturerFalcoEMotors = 277,
    eManufacturerActigraphcorp = 5759
} manufacturer_e;

typedef enum
{
    eGarminProductHrm1 = 1,
    eGarminProductAxh01 = 2,
    eGarminProductAxb01 = 3,
    eGarminProductAxb02 = 4,
    eGarminProductHrm2Ss = 5,
    eGarminProductDsiAlf02 = 6,
    eGarminProductHrm3Ss = 7,
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
    eGarminProductFr310Xt = 1018,
    eGarminProductEdge500 = 1036,
    eGarminProductFr110 = 1124,
    eGarminProductEdge800 = 1169,
    eGarminProductEdge500Taiwan = 1199,
    eGarminProductEdge500Japan = 1213,
    eGarminProductChirp = 1253,
    eGarminProductFr110Japan = 1274,
    eGarminProductEdge200 = 1325,
    eGarminProductFr910Xt = 1328,
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
    eGarminProductFr310Xt4T = 1446,
    eGarminProductAmx = 1461,
    eGarminProductFr10 = 1482,
    eGarminProductEdge800Korea = 1497,
    eGarminProductSwim = 1499,
    eGarminProductFr910XtChina = 1537,
    eGarminProductFenix = 1551,
    eGarminProductEdge200Taiwan = 1555,
    eGarminProductEdge510 = 1561,
    eGarminProductEdge810 = 1567,
    eGarminProductTempe = 1570,
    eGarminProductFr910XtJapan = 1600,
    eGarminProductFr620 = 1623,
    eGarminProductFr220 = 1632,
    eGarminProductFr910XtKorea = 1664,
    eGarminProductFr10Japan = 1688,
    eGarminProductEdge810Japan = 1721,
    eGarminProductVirbElite = 1735,
    eGarminProductEdgeTouring = 1736,
    eGarminProductEdge510Japan = 1742,
    eGarminProductHrmTri = 1743,
    eGarminProductHrmRun = 1752,
    eGarminProductFr920Xt = 1765,
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
    eGarminProductFr920XtTaiwan = 2130,
    eGarminProductFr920XtChina = 2131,
    eGarminProductFr920XtJapan = 2132,
    eGarminProductVirbx = 2134,
    eGarminProductVivoSmartApac = 2135,
    eGarminProductEtrexTouch = 2140,
    eGarminProductEdge25 = 2147,
    eGarminProductFr25 = 2148,
    eGarminProductVivoFit2 = 2150,
    eGarminProductFr225 = 2153,
    eGarminProductFr630 = 2156,
    eGarminProductFr230 = 2157,
    eGarminProductVivoActiveApac = 2160,
    eGarminProductVector2 = 2161,
    eGarminProductVector2S = 2162,
    eGarminProductVirbxe = 2172,
    eGarminProductFr620Taiwan = 2173,
    eGarminProductFr220Taiwan = 2174,
    eGarminProductTruswing = 2175,
    eGarminProductFenix3China = 2188,
    eGarminProductFenix3Twn = 2189,
    eGarminProductVariaHeadlight = 2192,
    eGarminProductVariaTaillightOld = 2193,
    eGarminProductEdgeExplore1000 = 2204,
    eGarminProductFr225Asia = 2219,
    eGarminProductVariaRadarTaillight = 2225,
    eGarminProductVariaRadarDisplay = 2226,
    eGarminProductEdge20 = 2238,
    eGarminProductD2Bravo = 2262,
    eGarminProductApproachS20 = 2266,
    eGarminProductVariaRemote = 2276,
    eGarminProductHrm4Run = 2327,
    eGarminProductVivoActiveHr = 2337,
    eGarminProductVivoSmartGpsHr = 2347,
    eGarminProductVivoSmartHr = 2348,
    eGarminProductVivoMove = 2368,
    eGarminProductVariaVision = 2398,
    eGarminProductVivoFit3 = 2406,
    eGarminProductFenix3Hr = 2413,
    eGarminProductIndexSmartScale = 2429,
    eGarminProductFr235 = 2431,
    eGarminProductOregon7Xx = 2441,
    eGarminProductRino7Xx = 2444,
    eGarminProductNautix = 2496,
    eGarminProductEdge820 = 2530,
    eGarminProductEdgeExplore820 = 2531,
    eGarminProductSdm4 = 10007,
    eGarminProductEdgeRemote = 10014,
    eGarminProductTrainingCenter = 20119,
    eGarminProductConnectiqSimulator = 65531,
    eGarminProductAndroidAntplusPlugin = 65532,
    eGarminProductConnect = 65534
} garmin_product_e;

typedef enum
{
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
    eAntplusDeviceTypeControlHub = 27,
    eAntplusDeviceTypeMuscleOxygen = 31,
    eAntplusDeviceTypeBikeLightMain = 35,
    eAntplusDeviceTypeBikeLightShared = 36,
    eAntplusDeviceTypeExd = 38,
    eAntplusDeviceTypeBikeRadar = 40,
    eAntplusDeviceTypeWeightScale = 119,
    eAntplusDeviceTypeHeartRate = 120,
    eAntplusDeviceTypeBikeSpeedCadence = 121,
    eAntplusDeviceTypeBikeCadence = 122,
    eAntplusDeviceTypeBikeSpeed = 123,
    eAntplusDeviceTypeStrideSpeedDistance = 124
} antplus_device_type_e;

typedef enum
{
    eAntNetworkPublic = 0,
    eAntNetworkAntplus = 1,
    eAntNetworkAntfs = 2,
    eAntNetworkPrivate = 3
} ant_network_e;

typedef enum
{
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

typedef enum
{
    eBatteryStatusNew = 1,
    eBatteryStatusGood = 2,
    eBatteryStatusOk = 3,
    eBatteryStatusLow = 4,
    eBatteryStatusCritical = 5,
    eBatteryStatusCharging = 6,
    eBatteryStatusUnknown = 7
} battery_status_e;

typedef enum
{
    eHrTypeNormal = 0,
    eHrTypeIrregular = 1
} hr_type_e;

typedef enum
{
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

typedef enum
{
    eWeightCalculating = 0xFFFE
} weight_e;

typedef enum
{
    eWorkoutHrBpmOffset = 100
} workout_hr_e;

typedef enum
{
    eWorkoutPowerWattsOffset = 1000
} workout_power_e;

typedef enum
{
    eBpStatusNoError = 0,
    eBpStatusErrorIncompleteData = 1,
    eBpStatusErrorNoMeasurement = 2,
    eBpStatusErrorDataOutOfRange = 3,
    eBpStatusErrorIrregularHeartRate = 4
} bp_status_e;

typedef enum
{
    eUserLocalIdLocalMin = 0x0000,
    eUserLocalIdLocalMax = 0x000F,
    eUserLocalIdStationaryMin = 0x0010,
    eUserLocalIdStationaryMax = 0x00FF,
    eUserLocalIdPortableMin = 0x0100,
    eUserLocalIdPortableMax = 0xFFFE
} user_local_id_e;

typedef enum
{
    eSwimStrokeFreestyle = 0,
    eSwimStrokeBackstroke = 1,
    eSwimStrokeBreaststroke = 2,
    eSwimStrokeButterfly = 3,
    eSwimStrokeDrill = 4,
    eSwimStrokeMixed = 5,
    eSwimStrokeIm = 6
} swim_stroke_e;

typedef enum
{
    eActivityTypeGeneric = 0,
    eActivityTypeRunning = 1,
    eActivityTypeCycling = 2,
    eActivityTypeTransition = 3,
    eActivityTypeFitnessEquipment = 4,
    eActivityTypeSwimming = 5,
    eActivityTypeWalking = 6,
    eActivityTypeSedentary = 8,
    eActivityTypeAll = 254
} activity_type_e;

typedef enum
{
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

typedef enum
{
    eActivityLevelLow = 0,
    eActivityLevelMedium = 1,
    eActivityLevelHigh = 2
} activity_level_e;

typedef enum
{
    eSideRight = 0,
    eSideLeft = 1
} side_e;

typedef enum
{
    eLeftRightBalanceMask = 0x7F,
    eLeftRightBalanceRight = 0x80
} left_right_balance_e;

typedef enum
{
    eLeftRightBalance100Mask = 0x3FFF,
    eLeftRightBalance100Right = 0x8000
} left_right_balance_100_e;

typedef enum
{
    eLengthTypeIdle = 0,
    eLengthTypeActive = 1
} length_type_e;

typedef enum
{
    eDayOfWeekSunday = 0,
    eDayOfWeekMonday = 1,
    eDayOfWeekTuesday = 2,
    eDayOfWeekWednesday = 3,
    eDayOfWeekThursday = 4,
    eDayOfWeekFriday = 5,
    eDayOfWeekSaturday = 6
} day_of_week_e;

typedef enum
{
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
    eConnectivityCapabilitiesConnectIqAppDownload = 0x00002000,
    eConnectivityCapabilitiesGolfCourseDownload = 0x00004000,
    eConnectivityCapabilitiesDeviceInitiatesSync = 0x00008000,
    eConnectivityCapabilitiesConnectIqWatchAppDownload = 0x00010000,
    eConnectivityCapabilitiesConnectIqWidgetDownload = 0x00020000,
    eConnectivityCapabilitiesConnectIqWatchFaceDownload = 0x00040000,
    eConnectivityCapabilitiesConnectIqDataFieldDownload = 0x00080000,
    eConnectivityCapabilitiesConnectIqAppManagment = 0x00100000,
    eConnectivityCapabilitiesSwingSensor = 0x00200000,
    eConnectivityCapabilitiesSwingSensorRemote = 0x00400000,
    eConnectivityCapabilitiesIncidentDetection = 0x00800000,
    eConnectivityCapabilitiesAudioPrompts = 0x01000000,
    eConnectivityCapabilitiesWifiVerification = 0x02000000,
    eConnectivityCapabilitiesTrueUp = 0x04000000,
    eConnectivityCapabilitiesFindMyWatch = 0x08000000,
    eConnectivityCapabilitiesRemoteManualSync = 0x10000000,
    eConnectivityCapabilitiesLiveTrackAutoStart = 0x20000000,
    eConnectivityCapabilitiesLiveTrackMessaging = 0x40000000,
    eConnectivityCapabilitiesInstantInput = 0x80000000
} connectivity_capabilities_e;

typedef enum
{
    eWeatherReportCurrent = 0,
    eWeatherReportForecast = 1,
    eWeatherReportHourlyForecast = 1,
    eWeatherReportDailyForecast = 2
} weather_report_e;

typedef enum
{
    eWeatherStatusClear = 0,
    eWeatherStatusPartlyCloudy = 1,
    eWeatherStatusMostlyCloudy = 2,
    eWeatherStatusRain = 3,
    eWeatherStatusSnow = 4,
    eWeatherStatusWindy = 5,
    eWeatherStatusThunderstorms = 6,
    eWeatherStatusWintryMix = 7,
    eWeatherStatusFog = 8,
    eWeatherStatusHazy = 11,
    eWeatherStatusHail = 12,
    eWeatherStatusScatteredShowers = 13,
    eWeatherStatusScatteredThunderstorms = 14,
    eWeatherStatusUnknownPrecipitation = 15,
    eWeatherStatusLightRain = 16,
    eWeatherStatusHeavyRain = 17,
    eWeatherStatusLightSnow = 18,
    eWeatherStatusHeavySnow = 19,
    eWeatherStatusLightRainSnow = 20,
    eWeatherStatusHeavyRainSnow = 21,
    eWeatherStatusCloudy = 22
} weather_status_e;

typedef enum
{
    eWeatherSeverityUnknown = 0,
    eWeatherSeverityWarning = 1,
    eWeatherSeverityWatch = 2,
    eWeatherSeverityAdvisory = 3,
    eWeatherSeverityStatement = 4
} weather_severity_e;

typedef enum
{
    eWeatherSevereTypeUnspecified = 0,
    eWeatherSevereTypeTornado = 1,
    eWeatherSevereTypeTsunami = 2,
    eWeatherSevereTypeHurricane = 3,
    eWeatherSevereTypeExtremeWind = 4,
    eWeatherSevereTypeTyphoon = 5,
    eWeatherSevereTypeInlandHurricane = 6,
    eWeatherSevereTypeHurricaneForceWind = 7,
    eWeatherSevereTypeWaterspout = 8,
    eWeatherSevereTypeSevereThunderstorm = 9,
    eWeatherSevereTypeWreckhouseWinds = 10,
    eWeatherSevereTypeLesSuetesWind = 11,
    eWeatherSevereTypeAvalanche = 12,
    eWeatherSevereTypeFlashFlood = 13,
    eWeatherSevereTypeTropicalStorm = 14,
    eWeatherSevereTypeInlandTropicalStorm = 15,
    eWeatherSevereTypeBlizzard = 16,
    eWeatherSevereTypeIceStorm = 17,
    eWeatherSevereTypeFreezingRain = 18,
    eWeatherSevereTypeDebrisFlow = 19,
    eWeatherSevereTypeFlashFreeze = 20,
    eWeatherSevereTypeDustStorm = 21,
    eWeatherSevereTypeHighWind = 22,
    eWeatherSevereTypeWinterStorm = 23,
    eWeatherSevereTypeHeavyFreezingSpray = 24,
    eWeatherSevereTypeExtremeCold = 25,
    eWeatherSevereTypeWindChill = 26,
    eWeatherSevereTypeColdWave = 27,
    eWeatherSevereTypeHeavySnowAlert = 28,
    eWeatherSevereTypeLakeEffectBlowingSnow = 29,
    eWeatherSevereTypeSnowSquall = 30,
    eWeatherSevereTypeLakeEffectSnow = 31,
    eWeatherSevereTypeWinterWeather = 32,
    eWeatherSevereTypeSleet = 33,
    eWeatherSevereTypeSnowfall = 34,
    eWeatherSevereTypeSnowAndBlowingSnow = 35,
    eWeatherSevereTypeBlowingSnow = 36,
    eWeatherSevereTypeSnowAlert = 37,
    eWeatherSevereTypeArcticOutflow = 38,
    eWeatherSevereTypeFreezingDrizzle = 39,
    eWeatherSevereTypeStorm = 40,
    eWeatherSevereTypeStormSurge = 41,
    eWeatherSevereTypeRainfall = 42,
    eWeatherSevereTypeArealFlood = 43,
    eWeatherSevereTypeCoastalFlood = 44,
    eWeatherSevereTypeLakeshoreFlood = 45,
    eWeatherSevereTypeExcessiveHeat = 46,
    eWeatherSevereTypeHeat = 47,
    eWeatherSevereTypeWeather = 48,
    eWeatherSevereTypeHighHeatAndHumidity = 49,
    eWeatherSevereTypeHumidexAndHealth = 50,
    eWeatherSevereTypeHumidex = 51,
    eWeatherSevereTypeGale = 52,
    eWeatherSevereTypeFreezingSpray = 53,
    eWeatherSevereTypeSpecialMarine = 54,
    eWeatherSevereTypeSquall = 55,
    eWeatherSevereTypeStrongWind = 56,
    eWeatherSevereTypeLakeWind = 57,
    eWeatherSevereTypeMarineWeather = 58,
    eWeatherSevereTypeWind = 59,
    eWeatherSevereTypeSmallCraftHazardousSeas = 60,
    eWeatherSevereTypeHazardousSeas = 61,
    eWeatherSevereTypeSmallCraft = 62,
    eWeatherSevereTypeSmallCraftWinds = 63,
    eWeatherSevereTypeSmallCraftRoughBar = 64,
    eWeatherSevereTypeHighWaterLevel = 65,
    eWeatherSevereTypeAshfall = 66,
    eWeatherSevereTypeFreezingFog = 67,
    eWeatherSevereTypeDenseFog = 68,
    eWeatherSevereTypeDenseSmoke = 69,
    eWeatherSevereTypeBlowingDust = 70,
    eWeatherSevereTypeHardFreeze = 71,
    eWeatherSevereTypeFreeze = 72,
    eWeatherSevereTypeFrost = 73,
    eWeatherSevereTypeFireWeather = 74,
    eWeatherSevereTypeFlood = 75,
    eWeatherSevereTypeRipTide = 76,
    eWeatherSevereTypeHighSurf = 77,
    eWeatherSevereTypeSmog = 78,
    eWeatherSevereTypeAirQuality = 79,
    eWeatherSevereTypeBriskWind = 80,
    eWeatherSevereTypeAirStagnation = 81,
    eWeatherSevereTypeLowWater = 82,
    eWeatherSevereTypeHydrological = 83,
    eWeatherSevereTypeSpecialWeather = 84
} weather_severe_type_e;

typedef enum
{
    eStrokeTypeNoEvent = 0,
    eStrokeTypeOther = 1,
    eStrokeTypeServe = 2,
    eStrokeTypeForehand = 3,
    eStrokeTypeBackhand = 4,
    eStrokeTypeSmash = 5
} stroke_type_e;

typedef enum
{
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
    eBodyLocationThroat = 35,
    eBodyLocationWaistMidBack = 36,
    eBodyLocationWaistFront = 37,
    eBodyLocationWaistLeft = 38,
    eBodyLocationWaistRight = 39
} body_location_e;

typedef enum
{
    eSegmentLapStatusEnd = 0,
    eSegmentLapStatusFail = 1
} segment_lap_status_e;

typedef enum
{
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

typedef enum
{
    eSegmentDeleteStatusDoNotDelete = 0,
    eSegmentDeleteStatusDeleteOne = 1,
    eSegmentDeleteStatusDeleteAll = 2
} segment_delete_status_e;

typedef enum
{
    eSegmentSelectionTypeStarred = 0,
    eSegmentSelectionTypeSuggested = 1
} segment_selection_type_e;

typedef enum
{
    eSourceTypeAnt = 0,
    eSourceTypeAntplus = 1,
    eSourceTypeBluetooth = 2,
    eSourceTypeBluetoothLowEnergy = 3,
    eSourceTypeWifi = 4,
    eSourceTypeLocal = 5
} source_type_e;

typedef enum
{
    eDisplayOrientationAuto = 0,
    eDisplayOrientationPortrait = 1,
    eDisplayOrientationLandscape = 2,
    eDisplayOrientationPortraitFlipped = 3,
    eDisplayOrientationLandscapeFlipped = 4
} display_orientation_e;

typedef enum
{
    eWatchfaceModeDigital = 0,
    eWatchfaceModeAnalog = 1,
    eWatchfaceModeConnectIq = 2
} watchface_mode_e;

typedef enum
{
    eDigitalWatchfaceLayoutTraditional = 0,
    eDigitalWatchfaceLayoutModern = 1,
    eDigitalWatchfaceLayoutBold = 2
} digital_watchface_layout_e;

typedef enum
{
    eAnalogWatchfaceLayoutMinimal = 0,
    eAnalogWatchfaceLayoutTraditional = 1,
    eAnalogWatchfaceLayoutModern = 2
} analog_watchface_layout_e;

typedef enum
{
    eRiderPositionTypeSeated = 0,
    eRiderPositionTypeStanding = 1
} rider_position_type_e;

typedef enum
{
    ePowerPhaseTypePowerPhaseStartAngle = 0,
    ePowerPhaseTypePowerPhaseEndAngle = 1,
    ePowerPhaseTypePowerPhaseArcLength = 2,
    ePowerPhaseTypePowerPhaseCenter = 3
} power_phase_type_e;

typedef enum
{
    eCameraEventTypeVideoStart = 0,
    eCameraEventTypeVideoSplit = 1,
    eCameraEventTypeVideoEnd = 2,
    eCameraEventTypePhotoTaken = 3,
    eCameraEventTypeVideoSecondStreamStart = 4,
    eCameraEventTypeVideoSecondStreamSplit = 5,
    eCameraEventTypeVideoSecondStreamEnd = 6,
    eCameraEventTypeVideoSplitStart = 7,
    eCameraEventTypeVideoSecondStreamSplitStart = 8,
    eCameraEventTypeVideoPause = 11,
    eCameraEventTypeVideoSecondStreamPause = 12,
    eCameraEventTypeVideoResume = 13,
    eCameraEventTypeVideoSecondStreamResume = 14
} camera_event_type_e;

typedef enum
{
    eSensorTypeAccelerometer = 0,
    eSensorTypeGyroscope = 1,
    eSensorTypeCompass = 2
} sensor_type_e;

typedef enum
{
    eBikeLightNetworkConfigTypeAuto = 0,
    eBikeLightNetworkConfigTypeIndividual = 4,
    eBikeLightNetworkConfigTypeHighVisibility = 5,
    eBikeLightNetworkConfigTypeTrail = 6
} bike_light_network_config_type_e;

typedef enum
{
    eCommTimeoutTypeWildcardPairingTimeout = 0,
    eCommTimeoutTypePairingTimeout = 1,
    eCommTimeoutTypeConnectionLost = 2,
    eCommTimeoutTypeConnectionTimeout = 3
} comm_timeout_type_e;

typedef enum
{
    eCameraOrientationTypeCameraOrientation0 = 0,
    eCameraOrientationTypeCameraOrientation90 = 1,
    eCameraOrientationTypeCameraOrientation180 = 2,
    eCameraOrientationTypeCameraOrientation270 = 3
} camera_orientation_type_e;

typedef enum
{
    eAttitudeStageFailed = 0,
    eAttitudeStageAligning = 1,
    eAttitudeStageDegraded = 2,
    eAttitudeStageValid = 3
} attitude_stage_e;

typedef enum
{
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

typedef enum
{
    eAutoSyncFrequencyNever = 0,
    eAutoSyncFrequencyOccasionally = 1,
    eAutoSyncFrequencyFrequent = 2,
    eAutoSyncFrequencyOnceADay = 3
} auto_sync_frequency_e;

typedef enum
{
    eExdLayoutFullScreen = 0,
    eExdLayoutHalfVertical = 1,
    eExdLayoutHalfHorizontal = 2,
    eExdLayoutHalfVerticalRightSplit = 3,
    eExdLayoutHalfHorizontalBottomSplit = 4,
    eExdLayoutFullQuarterSplit = 5,
    eExdLayoutHalfVerticalLeftSplit = 6,
    eExdLayoutHalfHorizontalTopSplit = 7
} exd_layout_e;

typedef enum
{
    eExdDisplayTypeNumerical = 0,
    eExdDisplayTypeSimple = 1,
    eExdDisplayTypeGraph = 2,
    eExdDisplayTypeBar = 3,
    eExdDisplayTypeCircleGraph = 4,
    eExdDisplayTypeVirtualPartner = 5,
    eExdDisplayTypeBalance = 6,
    eExdDisplayTypeStringList = 7,
    eExdDisplayTypeString = 8,
    eExdDisplayTypeSimpleDynamicIcon = 9,
    eExdDisplayTypeGauge = 10
} exd_display_type_e;

typedef enum
{
    eExdDataUnitsNoUnits = 0,
    eExdDataUnitsLaps = 1,
    eExdDataUnitsMilesPerHour = 2,
    eExdDataUnitsKilometersPerHour = 3,
    eExdDataUnitsFeetPerHour = 4,
    eExdDataUnitsMetersPerHour = 5,
    eExdDataUnitsDegreesCelsius = 6,
    eExdDataUnitsDegreesFarenheit = 7,
    eExdDataUnitsZone = 8,
    eExdDataUnitsGear = 9,
    eExdDataUnitsRpm = 10,
    eExdDataUnitsBpm = 11,
    eExdDataUnitsDegrees = 12,
    eExdDataUnitsMillimeters = 13,
    eExdDataUnitsMeters = 14,
    eExdDataUnitsKilometers = 15,
    eExdDataUnitsFeet = 16,
    eExdDataUnitsYards = 17,
    eExdDataUnitsKilofeet = 18,
    eExdDataUnitsMiles = 19,
    eExdDataUnitsTime = 20,
    eExdDataUnitsEnumTurnType = 21,
    eExdDataUnitsPercent = 22,
    eExdDataUnitsWatts = 23,
    eExdDataUnitsWattsPerKilogram = 24,
    eExdDataUnitsEnumBatteryStatus = 25,
    eExdDataUnitsEnumBikeLightBeamAngleMode = 26,
    eExdDataUnitsEnumBikeLightBatteryStatus = 27,
    eExdDataUnitsEnumBikeLightNetworkConfigType = 28,
    eExdDataUnitsLights = 29,
    eExdDataUnitsSeconds = 30,
    eExdDataUnitsMinutes = 31,
    eExdDataUnitsHours = 32,
    eExdDataUnitsCalories = 33,
    eExdDataUnitsKilojoules = 34,
    eExdDataUnitsMilliseconds = 35,
    eExdDataUnitsSecondPerMile = 36,
    eExdDataUnitsSecondPerKilometer = 37,
    eExdDataUnitsCentimeter = 38,
    eExdDataUnitsEnumCoursePoint = 39,
    eExdDataUnitsBradians = 40,
    eExdDataUnitsEnumSport = 41,
    eExdDataUnitsInchesHg = 42,
    eExdDataUnitsMmHg = 43,
    eExdDataUnitsMbars = 44,
    eExdDataUnitsHectoPascals = 45,
    eExdDataUnitsFeetPerMin = 46,
    eExdDataUnitsMetersPerMin = 47,
    eExdDataUnitsMetersPerSec = 48,
    eExdDataUnitsEightCardinal = 49
} exd_data_units_e;

typedef enum
{
    eExdQualifiersNoQualifier = 0,
    eExdQualifiersInstantaneous = 1,
    eExdQualifiersAverage = 2,
    eExdQualifiersLap = 3,
    eExdQualifiersMaximum = 4,
    eExdQualifiersMaximumAverage = 5,
    eExdQualifiersMaximumLap = 6,
    eExdQualifiersLastLap = 7,
    eExdQualifiersAverageLap = 8,
    eExdQualifiersToDestination = 9,
    eExdQualifiersToGo = 10,
    eExdQualifiersToNext = 11,
    eExdQualifiersNextCoursePoint = 12,
    eExdQualifiersTotal = 13,
    eExdQualifiersThreeSecondAverage = 14,
    eExdQualifiersTenSecondAverage = 15,
    eExdQualifiersThirtySecondAverage = 16,
    eExdQualifiersPercentMaximum = 17,
    eExdQualifiersPercentMaximumAverage = 18,
    eExdQualifiersLapPercentMaximum = 19,
    eExdQualifiersElapsed = 20,
    eExdQualifiersSunrise = 21,
    eExdQualifiersSunset = 22,
    eExdQualifiersComparedToVirtualPartner = 23,
    eExdQualifiersMaximum24H = 24,
    eExdQualifiersMinimum24H = 25,
    eExdQualifiersMinimum = 26,
    eExdQualifiersFirst = 27,
    eExdQualifiersSecond = 28,
    eExdQualifiersThird = 29,
    eExdQualifiersShifter = 30,
    eExdQualifiersLastSport = 31,
    eExdQualifiersMoving = 32,
    eExdQualifiersStopped = 33,
    eExdQualifiersZone9 = 242,
    eExdQualifiersZone8 = 243,
    eExdQualifiersZone7 = 244,
    eExdQualifiersZone6 = 245,
    eExdQualifiersZone5 = 246,
    eExdQualifiersZone4 = 247,
    eExdQualifiersZone3 = 248,
    eExdQualifiersZone2 = 249,
    eExdQualifiersZone1 = 250
} exd_qualifiers_e;

typedef enum
{
    eExdDescriptorsBikeLightBatteryStatus = 0,
    eExdDescriptorsBeamAngleStatus = 1,
    eExdDescriptorsBateryLevel = 2,
    eExdDescriptorsLightNetworkMode = 3,
    eExdDescriptorsNumberLightsConnected = 4,
    eExdDescriptorsCadence = 5,
    eExdDescriptorsDistance = 6,
    eExdDescriptorsEstimatedTimeOfArrival = 7,
    eExdDescriptorsHeading = 8,
    eExdDescriptorsTime = 9,
    eExdDescriptorsBatteryLevel = 10,
    eExdDescriptorsTrainerResistance = 11,
    eExdDescriptorsTrainerTargetPower = 12,
    eExdDescriptorsTimeSeated = 13,
    eExdDescriptorsTimeStanding = 14,
    eExdDescriptorsElevation = 15,
    eExdDescriptorsGrade = 16,
    eExdDescriptorsAscent = 17,
    eExdDescriptorsDescent = 18,
    eExdDescriptorsVerticalSpeed = 19,
    eExdDescriptorsDi2BatteryLevel = 20,
    eExdDescriptorsFrontGear = 21,
    eExdDescriptorsRearGear = 22,
    eExdDescriptorsGearRatio = 23,
    eExdDescriptorsHeartRate = 24,
    eExdDescriptorsHeartRateZone = 25,
    eExdDescriptorsTimeInHeartRateZone = 26,
    eExdDescriptorsHeartRateReserve = 27,
    eExdDescriptorsCalories = 28,
    eExdDescriptorsGpsAccuracy = 29,
    eExdDescriptorsGpsSignalStrength = 30,
    eExdDescriptorsTemperature = 31,
    eExdDescriptorsTimeOfDay = 32,
    eExdDescriptorsBalance = 33,
    eExdDescriptorsPedalSmoothness = 34,
    eExdDescriptorsPower = 35,
    eExdDescriptorsFunctionalThresholdPower = 36,
    eExdDescriptorsIntensityFactor = 37,
    eExdDescriptorsWork = 38,
    eExdDescriptorsPowerRatio = 39,
    eExdDescriptorsNormalizedPower = 40,
    eExdDescriptorsTrainingStressScore = 41,
    eExdDescriptorsTimeOnZone = 42,
    eExdDescriptorsSpeed = 43,
    eExdDescriptorsLaps = 44,
    eExdDescriptorsReps = 45,
    eExdDescriptorsWorkoutStep = 46,
    eExdDescriptorsCourseDistance = 47,
    eExdDescriptorsNavigationDistance = 48,
    eExdDescriptorsCourseEstimatedTimeOfArrival = 49,
    eExdDescriptorsNavigationEstimatedTimeOfArrival = 50,
    eExdDescriptorsCourseTime = 51,
    eExdDescriptorsNavigationTime = 52,
    eExdDescriptorsCourseHeading = 53,
    eExdDescriptorsNavigationHeading = 54,
    eExdDescriptorsPowerZone = 55,
    eExdDescriptorsTorqueEffectiveness = 56,
    eExdDescriptorsTimerTime = 57,
    eExdDescriptorsPowerWeightRatio = 58,
    eExdDescriptorsLeftPlatformCenterOffset = 59,
    eExdDescriptorsRightPlatformCenterOffset = 60,
    eExdDescriptorsLeftPowerPhaseStartAngle = 61,
    eExdDescriptorsRightPowerPhaseStartAngle = 62,
    eExdDescriptorsLeftPowerPhaseFinishAngle = 63,
    eExdDescriptorsRightPowerPhaseFinishAngle = 64,
    eExdDescriptorsGears = 65,
    eExdDescriptorsPace = 66,
    eExdDescriptorsTrainingEffect = 67,
    eExdDescriptorsVerticalOscillation = 68,
    eExdDescriptorsVerticalRatio = 69,
    eExdDescriptorsGroundContactTime = 70,
    eExdDescriptorsLeftGroundContactTimeBalance = 71,
    eExdDescriptorsRightGroundContactTimeBalance = 72,
    eExdDescriptorsStrideLength = 73,
    eExdDescriptorsRunningCadence = 74,
    eExdDescriptorsPerformanceCondition = 75,
    eExdDescriptorsCourseType = 76,
    eExdDescriptorsTimeInPowerZone = 77,
    eExdDescriptorsNavigationTurn = 78,
    eExdDescriptorsCourseLocation = 79,
    eExdDescriptorsNavigationLocation = 80,
    eExdDescriptorsCompass = 81,
    eExdDescriptorsGearCombo = 82,
    eExdDescriptorsMuscleOxygen = 83,
    eExdDescriptorsIcon = 84,
    eExdDescriptorsCompassHeading = 85,
    eExdDescriptorsGpsHeading = 86,
    eExdDescriptorsGpsElevation = 87,
    eExdDescriptorsAnaerobicTrainingEffect = 88,
    eExdDescriptorsCourse = 89,
    eExdDescriptorsOffCourse = 90,
    eExdDescriptorsGlideRatio = 91,
    eExdDescriptorsVerticalDistance = 92,
    eExdDescriptorsVmg = 93,
    eExdDescriptorsAmbientPressure = 94,
    eExdDescriptorsPressure = 95
} exd_descriptors_e;

typedef enum
{
    eAutoActivityDetectNone = 0x00000000,
    eAutoActivityDetectRunning = 0x00000001,
    eAutoActivityDetectCycling = 0x00000002,
    eAutoActivityDetectSwimming = 0x00000004,
    eAutoActivityDetectWalking = 0x00000008,
    eAutoActivityDetectElliptical = 0x00000020,
    eAutoActivityDetectSedentary = 0x00000400
} auto_activity_detect_e;

typedef enum
{
    eSupportedExdScreenLayoutsFullScreen = 0x00000001,
    eSupportedExdScreenLayoutsHalfVertical = 0x00000002,
    eSupportedExdScreenLayoutsHalfHorizontal = 0x00000004,
    eSupportedExdScreenLayoutsHalfVerticalRightSplit = 0x00000008,
    eSupportedExdScreenLayoutsHalfHorizontalBottomSplit = 0x00000010,
    eSupportedExdScreenLayoutsFullQuarterSplit = 0x00000020,
    eSupportedExdScreenLayoutsHalfVerticalLeftSplit = 0x00000040,
    eSupportedExdScreenLayoutsHalfHorizontalTopSplit = 0x00000080
} supported_exd_screen_layouts_e;

typedef enum
{
    eFitBaseTypeEnum = 0,
    eFitBaseTypeSint8 = 1,
    eFitBaseTypeUint8 = 2,
    eFitBaseTypeSint16 = 131,
    eFitBaseTypeUint16 = 132,
    eFitBaseTypeSint32 = 133,
    eFitBaseTypeUint32 = 134,
    eFitBaseTypeString = 7,
    eFitBaseTypeFloat32 = 136,
    eFitBaseTypeFloat64 = 137,
    eFitBaseTypeUint8Z = 10,
    eFitBaseTypeUint16Z = 139,
    eFitBaseTypeUint32Z = 140,
    eFitBaseTypeByte = 13,
    eFitBaseTypeSint64 = 142,
    eFitBaseTypeUint64 = 143,
    eFitBaseTypeUint64Z = 144
} fit_base_type_e;

typedef enum
{
    eTurnTypeArrivingIdx = 0,
    eTurnTypeArrivingLeftIdx = 1,
    eTurnTypeArrivingRightIdx = 2,
    eTurnTypeArrivingViaIdx = 3,
    eTurnTypeArrivingViaLeftIdx = 4,
    eTurnTypeArrivingViaRightIdx = 5,
    eTurnTypeBearKeepLeftIdx = 6,
    eTurnTypeBearKeepRightIdx = 7,
    eTurnTypeContinueIdx = 8,
    eTurnTypeExitLeftIdx = 9,
    eTurnTypeExitRightIdx = 10,
    eTurnTypeFerryIdx = 11,
    eTurnTypeRoundabout45Idx = 12,
    eTurnTypeRoundabout90Idx = 13,
    eTurnTypeRoundabout135Idx = 14,
    eTurnTypeRoundabout180Idx = 15,
    eTurnTypeRoundabout225Idx = 16,
    eTurnTypeRoundabout270Idx = 17,
    eTurnTypeRoundabout315Idx = 18,
    eTurnTypeRoundabout360Idx = 19,
    eTurnTypeRoundaboutNeg45Idx = 20,
    eTurnTypeRoundaboutNeg90Idx = 21,
    eTurnTypeRoundaboutNeg135Idx = 22,
    eTurnTypeRoundaboutNeg180Idx = 23,
    eTurnTypeRoundaboutNeg225Idx = 24,
    eTurnTypeRoundaboutNeg270Idx = 25,
    eTurnTypeRoundaboutNeg315Idx = 26,
    eTurnTypeRoundaboutNeg360Idx = 27,
    eTurnTypeRoundaboutGenericIdx = 28,
    eTurnTypeRoundaboutNegGenericIdx = 29,
    eTurnTypeSharpTurnLeftIdx = 30,
    eTurnTypeSharpTurnRightIdx = 31,
    eTurnTypeTurnLeftIdx = 32,
    eTurnTypeTurnRightIdx = 33,
    eTurnTypeUturnLeftIdx = 34,
    eTurnTypeUturnRightIdx = 35,
    eTurnTypeIconInvIdx = 36,
    eTurnTypeIconIdxCnt = 37
} turn_type_e;

typedef enum
{
    eBikeLightBeamAngleModeManual = 0,
    eBikeLightBeamAngleModeAuto = 1
} bike_light_beam_angle_mode_e;

typedef enum
{
    eFitBaseUnitOther = 0,
    eFitBaseUnitKilogram = 1,
    eFitBaseUnitPound = 2
} fit_base_unit_e;
// ----------- end generated code -----------

#endif // FIT_PROFILE_H
