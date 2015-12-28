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
    FileDevice = 1,
    FileSettings = 2,
    FileSport = 3,
    FileActivity = 4,
    FileWorkout = 5,
    FileCourse = 6,
    FileSchedules = 7,
    FileWeight = 9,
    FileTotals = 10,
    FileGoals = 11,
    FileBloodPressure = 14,
    FileMonitoringA = 15,
    FileActivitySummary = 20,
    FileMonitoringDaily = 28,
    FileMonitoringB = 32,
    FileSegment = 34,
    FileSegmentList = 35,
    FileMfgRangeMin = 0xF7,
    FileMfgRangeMax = 0xFE
} EnumFile;

typedef enum {
    MesgNumFileId = 0,
    MesgNumCapabilities = 1,
    MesgNumDeviceSettings = 2,
    MesgNumUserProfile = 3,
    MesgNumHrmProfile = 4,
    MesgNumSdmProfile = 5,
    MesgNumBikeProfile = 6,
    MesgNumZonesTarget = 7,
    MesgNumHrZone = 8,
    MesgNumPowerZone = 9,
    MesgNumMetZone = 10,
    MesgNumSport = 12,
    MesgNumGoal = 15,
    MesgNumSession = 18,
    MesgNumLap = 19,
    MesgNumRecord = 20,
    MesgNumEvent = 21,
    MesgNumDeviceInfo = 23,
    MesgNumWorkout = 26,
    MesgNumWorkoutStep = 27,
    MesgNumSchedule = 28,
    MesgNumWeightScale = 30,
    MesgNumCourse = 31,
    MesgNumCoursePoint = 32,
    MesgNumTotals = 33,
    MesgNumActivity = 34,
    MesgNumSoftware = 35,
    MesgNumFileCapabilities = 37,
    MesgNumMesgCapabilities = 38,
    MesgNumFieldCapabilities = 39,
    MesgNumFileCreator = 49,
    MesgNumBloodPressure = 51,
    MesgNumSpeedZone = 53,
    MesgNumMonitoring = 55,
    MesgNumTrainingFile = 72,
    MesgNumHrv = 78,
    MesgNumLength = 101,
    MesgNumMonitoringInfo = 103,
    MesgNumPad = 105,
    MesgNumSlaveDevice = 106,
    MesgNumCadenceZone = 131,
    MesgNumSegmentLap = 142,
    MesgNumMemoGlob = 145,
    MesgNumSegmentId = 148,
    MesgNumSegmentLeaderboardEntry = 149,
    MesgNumSegmentPoint = 150,
    MesgNumSegmentFile = 151,
    MesgNumGpsMetadata = 160,
    MesgNumCameraEvent = 161,
    MesgNumTimestampCorrelation = 162,
    MesgNumGyroscopeData = 164,
    MesgNumAccelerometerData = 165,
    MesgNumThreeDSensorCalibration = 167,
    MesgNumVideoFrame = 169,
    MesgNumObdiiData = 174,
    MesgNumNmeaSentence = 177,
    MesgNumAviationAttitude = 178,
    MesgNumVideo = 184,
    MesgNumVideoTitle = 185,
    MesgNumVideoDescription = 186,
    MesgNumVideoClip = 187,
    MesgNumMfgRangeMin = 0xFF00,
    MesgNumMfgRangeMax = 0xFFFE
} EnumMesgNum;

typedef enum {
    ChecksumClear = 0,
    ChecksumOk = 1
} EnumChecksum;

typedef enum {
    FileFlagsRead = 0x02,
    FileFlagsWrite = 0x04,
    FileFlagsErase = 0x08
} EnumFileFlags;

typedef enum {
    MesgCountNumPerFile = 0,
    MesgCountMaxPerFile = 1,
    MesgCountMaxPerFileType = 2
} EnumMesgCount;

typedef enum {
    DateTimeMin = 0x10000000
} EnumDateTime;

typedef enum {
    LocalDateTimeMin = 0x10000000
} EnumLocalDateTime;

typedef enum {
    MessageIndexSelected = 0x8000,
    MessageIndexReserved = 0x7000,
    MessageIndexMask = 0x0FFF
} EnumMessageIndex;

typedef enum {
    DeviceIndexCreator = 0
} EnumDeviceIndex;

typedef enum {
    GenderFemale = 0,
    GenderMale = 1
} EnumGender;

typedef enum {
    LanguageEnglish = 0,
    LanguageFrench = 1,
    LanguageItalian = 2,
    LanguageGerman = 3,
    LanguageSpanish = 4,
    LanguageCroatian = 5,
    LanguageCzech = 6,
    LanguageDanish = 7,
    LanguageDutch = 8,
    LanguageFinnish = 9,
    LanguageGreek = 10,
    LanguageHungarian = 11,
    LanguageNorwegian = 12,
    LanguagePolish = 13,
    LanguagePortuguese = 14,
    LanguageSlovakian = 15,
    LanguageSlovenian = 16,
    LanguageSwedish = 17,
    LanguageRussian = 18,
    LanguageTurkish = 19,
    LanguageLatvian = 20,
    LanguageUkrainian = 21,
    LanguageArabic = 22,
    LanguageFarsi = 23,
    LanguageBulgarian = 24,
    LanguageRomanian = 25,
    LanguageCustom = 254
} EnumLanguage;

typedef enum {
    TimeZoneAlmaty = 0,
    TimeZoneBangkok = 1,
    TimeZoneBombay = 2,
    TimeZoneBrasilia = 3,
    TimeZoneCairo = 4,
    TimeZoneCapeVerdeIs = 5,
    TimeZoneDarwin = 6,
    TimeZoneEniwetok = 7,
    TimeZoneFiji = 8,
    TimeZoneHongKong = 9,
    TimeZoneIslamabad = 10,
    TimeZoneKabul = 11,
    TimeZoneMagadan = 12,
    TimeZoneMidAtlantic = 13,
    TimeZoneMoscow = 14,
    TimeZoneMuscat = 15,
    TimeZoneNewfoundland = 16,
    TimeZoneSamoa = 17,
    TimeZoneSydney = 18,
    TimeZoneTehran = 19,
    TimeZoneTokyo = 20,
    TimeZoneUsAlaska = 21,
    TimeZoneUsAtlantic = 22,
    TimeZoneUsCentral = 23,
    TimeZoneUsEastern = 24,
    TimeZoneUsHawaii = 25,
    TimeZoneUsMountain = 26,
    TimeZoneUsPacific = 27,
    TimeZoneOther = 28,
    TimeZoneAuckland = 29,
    TimeZoneKathmandu = 30,
    TimeZoneEuropeWesternWet = 31,
    TimeZoneEuropeCentralCet = 32,
    TimeZoneEuropeEasternEet = 33,
    TimeZoneJakarta = 34,
    TimeZonePerth = 35,
    TimeZoneAdelaide = 36,
    TimeZoneBrisbane = 37,
    TimeZoneTasmania = 38,
    TimeZoneIceland = 39,
    TimeZoneAmsterdam = 40,
    TimeZoneAthens = 41,
    TimeZoneBarcelona = 42,
    TimeZoneBerlin = 43,
    TimeZoneBrussels = 44,
    TimeZoneBudapest = 45,
    TimeZoneCopenhagen = 46,
    TimeZoneDublin = 47,
    TimeZoneHelsinki = 48,
    TimeZoneLisbon = 49,
    TimeZoneLondon = 50,
    TimeZoneMadrid = 51,
    TimeZoneMunich = 52,
    TimeZoneOslo = 53,
    TimeZoneParis = 54,
    TimeZonePrague = 55,
    TimeZoneReykjavik = 56,
    TimeZoneRome = 57,
    TimeZoneStockholm = 58,
    TimeZoneVienna = 59,
    TimeZoneWarsaw = 60,
    TimeZoneZurich = 61,
    TimeZoneQuebec = 62,
    TimeZoneOntario = 63,
    TimeZoneManitoba = 64,
    TimeZoneSaskatchewan = 65,
    TimeZoneAlberta = 66,
    TimeZoneBritishColumbia = 67,
    TimeZoneBoise = 68,
    TimeZoneBoston = 69,
    TimeZoneChicago = 70,
    TimeZoneDallas = 71,
    TimeZoneDenver = 72,
    TimeZoneKansasCity = 73,
    TimeZoneLasVegas = 74,
    TimeZoneLosAngeles = 75,
    TimeZoneMiami = 76,
    TimeZoneMinneapolis = 77,
    TimeZoneNewYork = 78,
    TimeZoneNewOrleans = 79,
    TimeZonePhoenix = 80,
    TimeZoneSantaFe = 81,
    TimeZoneSeattle = 82,
    TimeZoneWashingtonDc = 83,
    TimeZoneUsArizona = 84,
    TimeZoneChita = 85,
    TimeZoneEkaterinburg = 86,
    TimeZoneIrkutsk = 87,
    TimeZoneKaliningrad = 88,
    TimeZoneKrasnoyarsk = 89,
    TimeZoneNovosibirsk = 90,
    TimeZonePetropavlovskKamchatskiy = 91,
    TimeZoneSamara = 92,
    TimeZoneVladivostok = 93,
    TimeZoneMexicoCentral = 94,
    TimeZoneMexicoMountain = 95,
    TimeZoneMexicoPacific = 96,
    TimeZoneCapeTown = 97,
    TimeZoneWinkhoek = 98,
    TimeZoneLagos = 99,
    TimeZoneRiyahd = 100,
    TimeZoneVenezuela = 101,
    TimeZoneAustraliaLh = 102,
    TimeZoneSantiago = 103,
    TimeZoneManual = 253,
    TimeZoneAutomatic = 254
} EnumTimeZone;

typedef enum {
    DisplayMeasureMetric = 0,
    DisplayMeasureStatute = 1
} EnumDisplayMeasure;

typedef enum {
    DisplayHeartBpm = 0,
    DisplayHeartMax = 1,
    DisplayHeartReserve = 2
} EnumDisplayHeart;

typedef enum {
    DisplayPowerWatts = 0,
    DisplayPowerPercentFtp = 1
} EnumDisplayPower;

typedef enum {
    DisplayPositionDegree = 0,
    DisplayPositionDegreeMinute = 1,
    DisplayPositionDegreeMinuteSecond = 2,
    DisplayPositionAustrianGrid = 3,
    DisplayPositionBritishGrid = 4,
    DisplayPositionDutchGrid = 5,
    DisplayPositionHungarianGrid = 6,
    DisplayPositionFinnishGrid = 7,
    DisplayPositionGermanGrid = 8,
    DisplayPositionIcelandicGrid = 9,
    DisplayPositionIndonesianEquatorial = 10,
    DisplayPositionIndonesianIrian = 11,
    DisplayPositionIndonesianSouthern = 12,
    DisplayPositionIndiaZone0 = 13,
    DisplayPositionIndiaZoneIA = 14,
    DisplayPositionIndiaZoneIB = 15,
    DisplayPositionIndiaZoneIIA = 16,
    DisplayPositionIndiaZoneIIB = 17,
    DisplayPositionIndiaZoneIIIA = 18,
    DisplayPositionIndiaZoneIIIB = 19,
    DisplayPositionIndiaZoneIVA = 20,
    DisplayPositionIndiaZoneIVB = 21,
    DisplayPositionIrishTransverse = 22,
    DisplayPositionIrishGrid = 23,
    DisplayPositionLoran = 24,
    DisplayPositionMaidenheadGrid = 25,
    DisplayPositionMgrsGrid = 26,
    DisplayPositionNewZealandGrid = 27,
    DisplayPositionNewZealandTransverse = 28,
    DisplayPositionQatarGrid = 29,
    DisplayPositionModifiedSwedishGrid = 30,
    DisplayPositionSwedishGrid = 31,
    DisplayPositionSouthAfricanGrid = 32,
    DisplayPositionSwissGrid = 33,
    DisplayPositionTaiwanGrid = 34,
    DisplayPositionUnitedStatesGrid = 35,
    DisplayPositionUtmUpsGrid = 36,
    DisplayPositionWestMalayan = 37,
    DisplayPositionBorneoRso = 38,
    DisplayPositionEstonianGrid = 39,
    DisplayPositionLatvianGrid = 40,
    DisplayPositionSwedishRef99Grid = 41
} EnumDisplayPosition;

typedef enum {
    SportGeneric = 0,
    SportRunning = 1,
    SportCycling = 2,
    SportTransition = 3,
    SportFitnessEquipment = 4,
    SportSwimming = 5,
    SportBasketball = 6,
    SportSoccer = 7,
    SportTennis = 8,
    SportAmericanFootball = 9,
    SportTraining = 10,
    SportWalking = 11,
    SportCrossCountrySkiing = 12,
    SportAlpineSkiing = 13,
    SportSnowboarding = 14,
    SportRowing = 15,
    SportMountaineering = 16,
    SportHiking = 17,
    SportMultisport = 18,
    SportPaddling = 19,
    SportFlying = 20,
    SportEBiking = 21,
    SportMotorcycling = 22,
    SportBoating = 23,
    SportDriving = 24,
    SportGolf = 25,
    SportHangGliding = 26,
    SportHorsebackRiding = 27,
    SportHunting = 28,
    SportFishing = 29,
    SportInlineSkating = 30,
    SportRockClimbing = 31,
    SportSailing = 32,
    SportIceSkating = 33,
    SportSkyDiving = 34,
    SportSnowshoeing = 35,
    SportSnowmobiling = 36,
    SportStandUpPaddleboarding = 37,
    SportSurfing = 38,
    SportWakeboarding = 39,
    SportWaterSkiing = 40,
    SportKayaking = 41,
    SportRafting = 42,
    SportWindsurfing = 43,
    SportKitesurfing = 44,
    SportAll = 254
} EnumSport;

typedef enum {
    SportBits0Generic = 0x01,
    SportBits0Running = 0x02,
    SportBits0Cycling = 0x04,
    SportBits0Transition = 0x08,
    SportBits0FitnessEquipment = 0x10,
    SportBits0Swimming = 0x20,
    SportBits0Basketball = 0x40,
    SportBits0Soccer = 0x80
} EnumSportBits0;

typedef enum {
    SportBits1Tennis = 0x01,
    SportBits1AmericanFootball = 0x02,
    SportBits1Training = 0x04,
    SportBits1Walking = 0x08,
    SportBits1CrossCountrySkiing = 0x10,
    SportBits1AlpineSkiing = 0x20,
    SportBits1Snowboarding = 0x40,
    SportBits1Rowing = 0x80
} EnumSportBits1;

typedef enum {
    SportBits2Mountaineering = 0x01,
    SportBits2Hiking = 0x02,
    SportBits2Multisport = 0x04,
    SportBits2Paddling = 0x08,
    SportBits2Flying = 0x10,
    SportBits2EBiking = 0x20,
    SportBits2Motorcycling = 0x40,
    SportBits2Boating = 0x80
} EnumSportBits2;

typedef enum {
    SportBits3Driving = 0x01,
    SportBits3Golf = 0x02,
    SportBits3HangGliding = 0x04,
    SportBits3HorsebackRiding = 0x08,
    SportBits3Hunting = 0x10,
    SportBits3Fishing = 0x20,
    SportBits3InlineSkating = 0x40,
    SportBits3RockClimbing = 0x80
} EnumSportBits3;

typedef enum {
    SportBits4Sailing = 0x01,
    SportBits4IceSkating = 0x02,
    SportBits4SkyDiving = 0x04,
    SportBits4Snowshoeing = 0x08,
    SportBits4Snowmobiling = 0x10,
    SportBits4StandUpPaddleboarding = 0x20,
    SportBits4Surfing = 0x40,
    SportBits4Wakeboarding = 0x80
} EnumSportBits4;

typedef enum {
    SportBits5WaterSkiing = 0x01,
    SportBits5Kayaking = 0x02,
    SportBits5Rafting = 0x04,
    SportBits5Windsurfing = 0x08,
    SportBits5Kitesurfing = 0x10
} EnumSportBits5;

typedef enum {
    SubSportGeneric = 0,
    SubSportTreadmill = 1,
    SubSportStreet = 2,
    SubSportTrail = 3,
    SubSportTrack = 4,
    SubSportSpin = 5,
    SubSportIndoorCycling = 6,
    SubSportRoad = 7,
    SubSportMountain = 8,
    SubSportDownhill = 9,
    SubSportRecumbent = 10,
    SubSportCyclocross = 11,
    SubSportHandCycling = 12,
    SubSportTrackCycling = 13,
    SubSportIndoorRowing = 14,
    SubSportElliptical = 15,
    SubSportStairClimbing = 16,
    SubSportLapSwimming = 17,
    SubSportOpenWater = 18,
    SubSportFlexibilityTraining = 19,
    SubSportStrengthTraining = 20,
    SubSportWarmUp = 21,
    SubSportMatch = 22,
    SubSportExercise = 23,
    SubSportChallenge = 24,
    SubSportIndoorSkiing = 25,
    SubSportCardioTraining = 26,
    SubSportIndoorWalking = 27,
    SubSportEBikeFitness = 28,
    SubSportBmx = 29,
    SubSportCasualWalking = 30,
    SubSportSpeedWalking = 31,
    SubSportBikeToRunTransition = 32,
    SubSportRunToBikeTransition = 33,
    SubSportSwimToBikeTransition = 34,
    SubSportAtv = 35,
    SubSportMotocross = 36,
    SubSportBackcountry = 37,
    SubSportResort = 38,
    SubSportRcDrone = 39,
    SubSportWingsuit = 40,
    SubSportWhitewater = 41,
    SubSportAll = 254
} EnumSubSport;

typedef enum {
    SportEventUncategorized = 0,
    SportEventGeocaching = 1,
    SportEventFitness = 2,
    SportEventRecreation = 3,
    SportEventRace = 4,
    SportEventSpecialEvent = 5,
    SportEventTraining = 6,
    SportEventTransportation = 7,
    SportEventTouring = 8
} EnumSportEvent;

typedef enum {
    ActivityManual = 0,
    ActivityAutoMultiSport = 1
} EnumActivity;

typedef enum {
    IntensityActive = 0,
    IntensityRest = 1,
    IntensityWarmup = 2,
    IntensityCooldown = 3
} EnumIntensity;

typedef enum {
    SessionTriggerActivityEnd = 0,
    SessionTriggerManual = 1,
    SessionTriggerAutoMultiSport = 2,
    SessionTriggerFitnessEquipment = 3
} EnumSessionTrigger;

typedef enum {
    AutolapTriggerTime = 0,
    AutolapTriggerDistance = 1,
    AutolapTriggerPositionStart = 2,
    AutolapTriggerPositionLap = 3,
    AutolapTriggerPositionWaypoint = 4,
    AutolapTriggerPositionMarked = 5,
    AutolapTriggerOff = 6
} EnumAutolapTrigger;

typedef enum {
    LapTriggerManual = 0,
    LapTriggerTime = 1,
    LapTriggerDistance = 2,
    LapTriggerPositionStart = 3,
    LapTriggerPositionLap = 4,
    LapTriggerPositionWaypoint = 5,
    LapTriggerPositionMarked = 6,
    LapTriggerSessionEnd = 7,
    LapTriggerFitnessEquipment = 8
} EnumLapTrigger;

typedef enum {
    EventTimer = 0,
    EventWorkout = 3,
    EventWorkoutStep = 4,
    EventPowerDown = 5,
    EventPowerUp = 6,
    EventOffCourse = 7,
    EventSession = 8,
    EventLap = 9,
    EventCoursePoint = 10,
    EventBattery = 11,
    EventVirtualPartnerPace = 12,
    EventHrHighAlert = 13,
    EventHrLowAlert = 14,
    EventSpeedHighAlert = 15,
    EventSpeedLowAlert = 16,
    EventCadHighAlert = 17,
    EventCadLowAlert = 18,
    EventPowerHighAlert = 19,
    EventPowerLowAlert = 20,
    EventRecoveryHr = 21,
    EventBatteryLow = 22,
    EventTimeDurationAlert = 23,
    EventDistanceDurationAlert = 24,
    EventCalorieDurationAlert = 25,
    EventActivity = 26,
    EventFitnessEquipment = 27,
    EventLength = 28,
    EventUserMarker = 32,
    EventSportPoint = 33,
    EventCalibration = 36,
    EventFrontGearChange = 42,
    EventRearGearChange = 43,
    EventRiderPositionChange = 44,
    EventElevHighAlert = 45,
    EventElevLowAlert = 46,
    EventCommTimeout = 47
} EnumEvent;

typedef enum {
    EventTypeStart = 0,
    EventTypeStop = 1,
    EventTypeConsecutiveDepreciated = 2,
    EventTypeMarker = 3,
    EventTypeStopAll = 4,
    EventTypeBeginDepreciated = 5,
    EventTypeEndDepreciated = 6,
    EventTypeEndAllDepreciated = 7,
    EventTypeStopDisable = 8,
    EventTypeStopDisableAll = 9
} EnumEventType;

typedef enum {
    TimerTriggerManual = 0,
    TimerTriggerAuto = 1,
    TimerTriggerFitnessEquipment = 2
} EnumTimerTrigger;

typedef enum {
    FitnessEquipmentStateReady = 0,
    FitnessEquipmentStateInUse = 1,
    FitnessEquipmentStatePaused = 2,
    FitnessEquipmentStateUnknown = 3
} EnumFitnessEquipmentState;

typedef enum {
    ActivityClassLevel = 0x7F,
    ActivityClassLevelMax = 100,
    ActivityClassAthlete = 0x80
} EnumActivityClass;

typedef enum {
    HrZoneCalcCustom = 0,
    HrZoneCalcPercentMaxHr = 1,
    HrZoneCalcPercentHrr = 2
} EnumHrZoneCalc;

typedef enum {
    PwrZoneCalcCustom = 0,
    PwrZoneCalcPercentFtp = 1
} EnumPwrZoneCalc;

typedef enum {
    WktStepDurationTime = 0,
    WktStepDurationDistance = 1,
    WktStepDurationHrLessThan = 2,
    WktStepDurationHrGreaterThan = 3,
    WktStepDurationCalories = 4,
    WktStepDurationOpen = 5,
    WktStepDurationRepeatUntilStepsCmplt = 6,
    WktStepDurationRepeatUntilTime = 7,
    WktStepDurationRepeatUntilDistance = 8,
    WktStepDurationRepeatUntilCalories = 9,
    WktStepDurationRepeatUntilHrLessThan = 10,
    WktStepDurationRepeatUntilHrGreaterThan = 11,
    WktStepDurationRepeatUntilPowerLessThan = 12,
    WktStepDurationRepeatUntilPowerGreaterThan = 13,
    WktStepDurationPowerLessThan = 14,
    WktStepDurationPowerGreaterThan = 15,
    WktStepDurationRepetitionTime = 28
} EnumWktStepDuration;

typedef enum {
    WktStepTargetSpeed = 0,
    WktStepTargetHeartRate = 1,
    WktStepTargetOpen = 2,
    WktStepTargetCadence = 3,
    WktStepTargetPower = 4,
    WktStepTargetGrade = 5,
    WktStepTargetResistance = 6
} EnumWktStepTarget;

typedef enum {
    GoalTime = 0,
    GoalDistance = 1,
    GoalCalories = 2,
    GoalFrequency = 3,
    GoalSteps = 4
} EnumGoal;

typedef enum {
    GoalRecurrenceOff = 0,
    GoalRecurrenceDaily = 1,
    GoalRecurrenceWeekly = 2,
    GoalRecurrenceMonthly = 3,
    GoalRecurrenceYearly = 4,
    GoalRecurrenceCustom = 5
} EnumGoalRecurrence;

typedef enum {
    ScheduleWorkout = 0,
    ScheduleCourse = 1
} EnumSchedule;

typedef enum {
    CoursePointGeneric = 0,
    CoursePointSummit = 1,
    CoursePointValley = 2,
    CoursePointWater = 3,
    CoursePointFood = 4,
    CoursePointDanger = 5,
    CoursePointLeft = 6,
    CoursePointRight = 7,
    CoursePointStraight = 8,
    CoursePointFirstAid = 9,
    CoursePointFourthCategory = 10,
    CoursePointThirdCategory = 11,
    CoursePointSecondCategory = 12,
    CoursePointFirstCategory = 13,
    CoursePointHorsCategory = 14,
    CoursePointSprint = 15,
    CoursePointLeftFork = 16,
    CoursePointRightFork = 17,
    CoursePointMiddleFork = 18,
    CoursePointSlightLeft = 19,
    CoursePointSharpLeft = 20,
    CoursePointSlightRight = 21,
    CoursePointSharpRight = 22,
    CoursePointUTurn = 23
} EnumCoursePoint;

typedef enum {
    ManufacturerGarmin = 1,
    ManufacturerGarminFr405Antfs = 2,
    ManufacturerZephyr = 3,
    ManufacturerDayton = 4,
    ManufacturerIdt = 5,
    ManufacturerSrm = 6,
    ManufacturerQuarq = 7,
    ManufacturerIbike = 8,
    ManufacturerSaris = 9,
    ManufacturerSparkHk = 10,
    ManufacturerTanita = 11,
    ManufacturerEchowell = 12,
    ManufacturerDynastreamOem = 13,
    ManufacturerNautilus = 14,
    ManufacturerDynastream = 15,
    ManufacturerTimex = 16,
    ManufacturerMetrigear = 17,
    ManufacturerXelic = 18,
    ManufacturerBeurer = 19,
    ManufacturerCardiosport = 20,
    ManufacturerAAndD = 21,
    ManufacturerHmm = 22,
    ManufacturerSuunto = 23,
    ManufacturerThitaElektronik = 24,
    ManufacturerGpulse = 25,
    ManufacturerCleanMobile = 26,
    ManufacturerPedalBrain = 27,
    ManufacturerPeaksware = 28,
    ManufacturerSaxonar = 29,
    ManufacturerLemondFitness = 30,
    ManufacturerDexcom = 31,
    ManufacturerWahooFitness = 32,
    ManufacturerOctaneFitness = 33,
    ManufacturerArchinoetics = 34,
    ManufacturerTheHurtBox = 35,
    ManufacturerCitizenSystems = 36,
    ManufacturerMagellan = 37,
    ManufacturerOsynce = 38,
    ManufacturerHolux = 39,
    ManufacturerConcept2 = 40,
    ManufacturerOneGiantLeap = 42,
    ManufacturerAceSensor = 43,
    ManufacturerBrimBrothers = 44,
    ManufacturerXplova = 45,
    ManufacturerPerceptionDigital = 46,
    ManufacturerBf1systems = 47,
    ManufacturerPioneer = 48,
    ManufacturerSpantec = 49,
    ManufacturerMetalogics = 50,
    Manufacturer4iiiis = 51,
    ManufacturerSeikoEpson = 52,
    ManufacturerSeikoEpsonOem = 53,
    ManufacturerIforPowell = 54,
    ManufacturerMaxwellGuider = 55,
    ManufacturerStarTrac = 56,
    ManufacturerBreakaway = 57,
    ManufacturerAlatechTechnologyLtd = 58,
    ManufacturerMioTechnologyEurope = 59,
    ManufacturerRotor = 60,
    ManufacturerGeonaute = 61,
    ManufacturerIdBike = 62,
    ManufacturerSpecialized = 63,
    ManufacturerWtek = 64,
    ManufacturerPhysicalEnterprises = 65,
    ManufacturerNorthPoleEngineering = 66,
    ManufacturerBkool = 67,
    ManufacturerCateye = 68,
    ManufacturerStagesCycling = 69,
    ManufacturerSigmasport = 70,
    ManufacturerTomtom = 71,
    ManufacturerPeripedal = 72,
    ManufacturerWattbike = 73,
    ManufacturerMoxy = 76,
    ManufacturerCiclosport = 77,
    ManufacturerPowerbahn = 78,
    ManufacturerAcornProjectsAps = 79,
    ManufacturerLifebeam = 80,
    ManufacturerBontrager = 81,
    ManufacturerWellgo = 82,
    ManufacturerScosche = 83,
    ManufacturerMagura = 84,
    ManufacturerWoodway = 85,
    ManufacturerElite = 86,
    ManufacturerNielsenKellerman = 87,
    ManufacturerDkCity = 88,
    ManufacturerTacx = 89,
    ManufacturerDirectionTechnology = 90,
    ManufacturerMagtonic = 91,
    Manufacturer1partcarbon = 92,
    ManufacturerInsideRideTechnologies = 93,
    ManufacturerSoundOfMotion = 94,
    ManufacturerStryd = 95,
    ManufacturerIcg = 96,
    ManufacturerMiPulse = 97,
    ManufacturerBsxAthletics = 98,
    ManufacturerLook = 99,
    ManufacturerDevelopment = 255,
    ManufacturerHealthandlife = 257,
    ManufacturerLezyne = 258,
    ManufacturerScribeLabs = 259,
    ManufacturerZwift = 260,
    ManufacturerWatteam = 261,
    ManufacturerRecon = 262,
    ManufacturerFaveroElectronics = 263,
    ManufacturerDynovelo = 264,
    ManufacturerStrava = 265,
    ManufacturerActigraphcorp = 5759
} EnumManufacturer;

typedef enum {
    GarminProductHrm1 = 1,
    GarminProductAxh01 = 2,
    GarminProductAxb01 = 3,
    GarminProductAxb02 = 4,
    GarminProductHrm2ss = 5,
    GarminProductDsiAlf02 = 6,
    GarminProductHrm3ss = 7,
    GarminProductHrmRunSingleByteProductId = 8,
    GarminProductBsm = 9,
    GarminProductBcm = 10,
    GarminProductAxs01 = 11,
    GarminProductHrmTriSingleByteProductId = 12,
    GarminProductFr225SingleByteProductId = 14,
    GarminProductFr301China = 473,
    GarminProductFr301Japan = 474,
    GarminProductFr301Korea = 475,
    GarminProductFr301Taiwan = 494,
    GarminProductFr405 = 717,
    GarminProductFr50 = 782,
    GarminProductFr405Japan = 987,
    GarminProductFr60 = 988,
    GarminProductDsiAlf01 = 1011,
    GarminProductFr310xt = 1018,
    GarminProductEdge500 = 1036,
    GarminProductFr110 = 1124,
    GarminProductEdge800 = 1169,
    GarminProductEdge500Taiwan = 1199,
    GarminProductEdge500Japan = 1213,
    GarminProductChirp = 1253,
    GarminProductFr110Japan = 1274,
    GarminProductEdge200 = 1325,
    GarminProductFr910xt = 1328,
    GarminProductEdge800Taiwan = 1333,
    GarminProductEdge800Japan = 1334,
    GarminProductAlf04 = 1341,
    GarminProductFr610 = 1345,
    GarminProductFr210Japan = 1360,
    GarminProductVectorSs = 1380,
    GarminProductVectorCp = 1381,
    GarminProductEdge800China = 1386,
    GarminProductEdge500China = 1387,
    GarminProductFr610Japan = 1410,
    GarminProductEdge500Korea = 1422,
    GarminProductFr70 = 1436,
    GarminProductFr310xt4t = 1446,
    GarminProductAmx = 1461,
    GarminProductFr10 = 1482,
    GarminProductEdge800Korea = 1497,
    GarminProductSwim = 1499,
    GarminProductFr910xtChina = 1537,
    GarminProductFenix = 1551,
    GarminProductEdge200Taiwan = 1555,
    GarminProductEdge510 = 1561,
    GarminProductEdge810 = 1567,
    GarminProductTempe = 1570,
    GarminProductFr910xtJapan = 1600,
    GarminProductFr620 = 1623,
    GarminProductFr220 = 1632,
    GarminProductFr910xtKorea = 1664,
    GarminProductFr10Japan = 1688,
    GarminProductEdge810Japan = 1721,
    GarminProductVirbElite = 1735,
    GarminProductEdgeTouring = 1736,
    GarminProductEdge510Japan = 1742,
    GarminProductHrmTri = 1743,
    GarminProductHrmRun = 1752,
    GarminProductFr920xt = 1765,
    GarminProductEdge510Asia = 1821,
    GarminProductEdge810China = 1822,
    GarminProductEdge810Taiwan = 1823,
    GarminProductEdge1000 = 1836,
    GarminProductVivoFit = 1837,
    GarminProductVirbRemote = 1853,
    GarminProductVivoKi = 1885,
    GarminProductFr15 = 1903,
    GarminProductVivoActive = 1907,
    GarminProductEdge510Korea = 1918,
    GarminProductFr620Japan = 1928,
    GarminProductFr620China = 1929,
    GarminProductFr220Japan = 1930,
    GarminProductFr220China = 1931,
    GarminProductApproachS6 = 1936,
    GarminProductVivoSmart = 1956,
    GarminProductFenix2 = 1967,
    GarminProductEpix = 1988,
    GarminProductFenix3 = 2050,
    GarminProductEdge1000Taiwan = 2052,
    GarminProductEdge1000Japan = 2053,
    GarminProductFr15Japan = 2061,
    GarminProductEdge520 = 2067,
    GarminProductEdge1000China = 2070,
    GarminProductFr620Russia = 2072,
    GarminProductFr220Russia = 2073,
    GarminProductVectorS = 2079,
    GarminProductEdge1000Korea = 2100,
    GarminProductFr920xtTaiwan = 2130,
    GarminProductFr920xtChina = 2131,
    GarminProductFr920xtJapan = 2132,
    GarminProductVirbx = 2134,
    GarminProductVivoSmartApac = 2135,
    GarminProductEtrexTouch = 2140,
    GarminProductEdge25 = 2147,
    GarminProductVivoFit2 = 2150,
    GarminProductFr225 = 2153,
    GarminProductVivoActiveApac = 2160,
    GarminProductVector2 = 2161,
    GarminProductVector2s = 2162,
    GarminProductVirbxe = 2172,
    GarminProductFr620Taiwan = 2173,
    GarminProductFr220Taiwan = 2174,
    GarminProductFenix3China = 2188,
    GarminProductFenix3Twn = 2189,
    GarminProductVariaHeadlight = 2192,
    GarminProductVariaTaillightOld = 2193,
    GarminProductFr225Asia = 2219,
    GarminProductVariaRadarTaillight = 2225,
    GarminProductVariaRadarDisplay = 2226,
    GarminProductEdge20 = 2238,
    GarminProductD2Bravo = 2262,
    GarminProductVariaRemote = 2276,
    GarminProductSdm4 = 10007,
    GarminProductEdgeRemote = 10014,
    GarminProductTrainingCenter = 20119,
    GarminProductAndroidAntplusPlugin = 65532,
    GarminProductConnect = 65534
} EnumGarminProduct;

typedef enum {
    AntplusDeviceTypeAntfs = 1,
    AntplusDeviceTypeBikePower = 11,
    AntplusDeviceTypeEnvironmentSensorLegacy = 12,
    AntplusDeviceTypeMultiSportSpeedDistance = 15,
    AntplusDeviceTypeControl = 16,
    AntplusDeviceTypeFitnessEquipment = 17,
    AntplusDeviceTypeBloodPressure = 18,
    AntplusDeviceTypeGeocacheNode = 19,
    AntplusDeviceTypeLightElectricVehicle = 20,
    AntplusDeviceTypeEnvSensor = 25,
    AntplusDeviceTypeRacquet = 26,
    AntplusDeviceTypeWeightScale = 119,
    AntplusDeviceTypeHeartRate = 120,
    AntplusDeviceTypeBikeSpeedCadence = 121,
    AntplusDeviceTypeBikeCadence = 122,
    AntplusDeviceTypeBikeSpeed = 123,
    AntplusDeviceTypeStrideSpeedDistance = 124
} EnumAntplusDeviceType;

typedef enum {
    AntNetworkPublic = 0,
    AntNetworkAntplus = 1,
    AntNetworkAntfs = 2,
    AntNetworkPrivate = 3
} EnumAntNetwork;

typedef enum {
    WorkoutCapabilitiesInterval = 0x00000001,
    WorkoutCapabilitiesCustom = 0x00000002,
    WorkoutCapabilitiesFitnessEquipment = 0x00000004,
    WorkoutCapabilitiesFirstbeat = 0x00000008,
    WorkoutCapabilitiesNewLeaf = 0x00000010,
    WorkoutCapabilitiesTcx = 0x00000020,
    WorkoutCapabilitiesSpeed = 0x00000080,
    WorkoutCapabilitiesHeartRate = 0x00000100,
    WorkoutCapabilitiesDistance = 0x00000200,
    WorkoutCapabilitiesCadence = 0x00000400,
    WorkoutCapabilitiesPower = 0x00000800,
    WorkoutCapabilitiesGrade = 0x00001000,
    WorkoutCapabilitiesResistance = 0x00002000,
    WorkoutCapabilitiesProtected = 0x00004000
} EnumWorkoutCapabilities;

typedef enum {
    BatteryStatusNew = 1,
    BatteryStatusGood = 2,
    BatteryStatusOk = 3,
    BatteryStatusLow = 4,
    BatteryStatusCritical = 5,
    BatteryStatusUnknown = 7
} EnumBatteryStatus;

typedef enum {
    HrTypeNormal = 0,
    HrTypeIrregular = 1
} EnumHrType;

typedef enum {
    CourseCapabilitiesProcessed = 0x00000001,
    CourseCapabilitiesValid = 0x00000002,
    CourseCapabilitiesTime = 0x00000004,
    CourseCapabilitiesDistance = 0x00000008,
    CourseCapabilitiesPosition = 0x00000010,
    CourseCapabilitiesHeartRate = 0x00000020,
    CourseCapabilitiesPower = 0x00000040,
    CourseCapabilitiesCadence = 0x00000080,
    CourseCapabilitiesTraining = 0x00000100,
    CourseCapabilitiesNavigation = 0x00000200,
    CourseCapabilitiesBikeway = 0x00000400
} EnumCourseCapabilities;

typedef enum {
    WeightCalculating = 0xFFFE
} EnumWeight;

typedef enum {
    WorkoutHrBpmOffset = 100
} EnumWorkoutHr;

typedef enum {
    WorkoutPowerWattsOffset = 1000
} EnumWorkoutPower;

typedef enum {
    BpStatusNoError = 0,
    BpStatusErrorIncompleteData = 1,
    BpStatusErrorNoMeasurement = 2,
    BpStatusErrorDataOutOfRange = 3,
    BpStatusErrorIrregularHeartRate = 4
} EnumBpStatus;

typedef enum {
    UserLocalIdLocalMin = 0x0000,
    UserLocalIdLocalMax = 0x000F,
    UserLocalIdStationaryMin = 0x0010,
    UserLocalIdStationaryMax = 0x00FF,
    UserLocalIdPortableMin = 0x0100,
    UserLocalIdPortableMax = 0xFFFE
} EnumUserLocalId;

typedef enum {
    SwimStrokeFreestyle = 0,
    SwimStrokeBackstroke = 1,
    SwimStrokeBreaststroke = 2,
    SwimStrokeButterfly = 3,
    SwimStrokeDrill = 4,
    SwimStrokeMixed = 5,
    SwimStrokeIm = 6
} EnumSwimStroke;

typedef enum {
    ActivityTypeGeneric = 0,
    ActivityTypeRunning = 1,
    ActivityTypeCycling = 2,
    ActivityTypeTransition = 3,
    ActivityTypeFitnessEquipment = 4,
    ActivityTypeSwimming = 5,
    ActivityTypeWalking = 6,
    ActivityTypeAll = 254
} EnumActivityType;

typedef enum {
    ActivitySubtypeGeneric = 0,
    ActivitySubtypeTreadmill = 1,
    ActivitySubtypeStreet = 2,
    ActivitySubtypeTrail = 3,
    ActivitySubtypeTrack = 4,
    ActivitySubtypeSpin = 5,
    ActivitySubtypeIndoorCycling = 6,
    ActivitySubtypeRoad = 7,
    ActivitySubtypeMountain = 8,
    ActivitySubtypeDownhill = 9,
    ActivitySubtypeRecumbent = 10,
    ActivitySubtypeCyclocross = 11,
    ActivitySubtypeHandCycling = 12,
    ActivitySubtypeTrackCycling = 13,
    ActivitySubtypeIndoorRowing = 14,
    ActivitySubtypeElliptical = 15,
    ActivitySubtypeStairClimbing = 16,
    ActivitySubtypeLapSwimming = 17,
    ActivitySubtypeOpenWater = 18,
    ActivitySubtypeAll = 254
} EnumActivitySubtype;

typedef enum {
    ActivityLevelLow = 0,
    ActivityLevelMedium = 1,
    ActivityLevelHigh = 2
} EnumActivityLevel;

typedef enum {
    LeftRightBalanceMask = 0x7F,
    LeftRightBalanceRight = 0x80
} EnumLeftRightBalance;

typedef enum {
    LeftRightBalance100Mask = 0x3FFF,
    LeftRightBalance100Right = 0x8000
} EnumLeftRightBalance100;

typedef enum {
    LengthTypeIdle = 0,
    LengthTypeActive = 1
} EnumLengthType;

typedef enum {
    ConnectivityCapabilitiesBluetooth = 0x00000001,
    ConnectivityCapabilitiesBluetoothLe = 0x00000002,
    ConnectivityCapabilitiesAnt = 0x00000004,
    ConnectivityCapabilitiesActivityUpload = 0x00000008,
    ConnectivityCapabilitiesCourseDownload = 0x00000010,
    ConnectivityCapabilitiesWorkoutDownload = 0x00000020,
    ConnectivityCapabilitiesLiveTrack = 0x00000040,
    ConnectivityCapabilitiesWeatherConditions = 0x00000080,
    ConnectivityCapabilitiesWeatherAlerts = 0x00000100,
    ConnectivityCapabilitiesGpsEphemerisDownload = 0x00000200,
    ConnectivityCapabilitiesExplicitArchive = 0x00000400,
    ConnectivityCapabilitiesSetupIncomplete = 0x00000800,
    ConnectivityCapabilitiesContinueSyncAfterSoftwareUpdate = 0x00001000,
    ConnectivityCapabilitiesConnectIqAppDownload = 0x00002000
} EnumConnectivityCapabilities;

typedef enum {
    StrokeTypeNoEvent = 0,
    StrokeTypeOther = 1,
    StrokeTypeServe = 2,
    StrokeTypeForehand = 3,
    StrokeTypeBackhand = 4,
    StrokeTypeSmash = 5
} EnumStrokeType;

typedef enum {
    BodyLocationLeftLeg = 0,
    BodyLocationLeftCalf = 1,
    BodyLocationLeftShin = 2,
    BodyLocationLeftHamstring = 3,
    BodyLocationLeftQuad = 4,
    BodyLocationLeftGlute = 5,
    BodyLocationRightLeg = 6,
    BodyLocationRightCalf = 7,
    BodyLocationRightShin = 8,
    BodyLocationRightHamstring = 9,
    BodyLocationRightQuad = 10,
    BodyLocationRightGlute = 11,
    BodyLocationTorsoBack = 12,
    BodyLocationLeftLowerBack = 13,
    BodyLocationLeftUpperBack = 14,
    BodyLocationRightLowerBack = 15,
    BodyLocationRightUpperBack = 16,
    BodyLocationTorsoFront = 17,
    BodyLocationLeftAbdomen = 18,
    BodyLocationLeftChest = 19,
    BodyLocationRightAbdomen = 20,
    BodyLocationRightChest = 21,
    BodyLocationLeftArm = 22,
    BodyLocationLeftShoulder = 23,
    BodyLocationLeftBicep = 24,
    BodyLocationLeftTricep = 25,
    BodyLocationLeftBrachioradialis = 26,
    BodyLocationLeftForearmExtensors = 27,
    BodyLocationRightArm = 28,
    BodyLocationRightShoulder = 29,
    BodyLocationRightBicep = 30,
    BodyLocationRightTricep = 31,
    BodyLocationRightBrachioradialis = 32,
    BodyLocationRightForearmExtensors = 33,
    BodyLocationNeck = 34,
    BodyLocationThroat = 35
} EnumBodyLocation;

typedef enum {
    SegmentLapStatusEnd = 0,
    SegmentLapStatusFail = 1
} EnumSegmentLapStatus;

typedef enum {
    SegmentLeaderboardTypeOverall = 0,
    SegmentLeaderboardTypePersonalBest = 1,
    SegmentLeaderboardTypeConnections = 2,
    SegmentLeaderboardTypeGroup = 3,
    SegmentLeaderboardTypeChallenger = 4,
    SegmentLeaderboardTypeKom = 5,
    SegmentLeaderboardTypeQom = 6,
    SegmentLeaderboardTypePr = 7,
    SegmentLeaderboardTypeGoal = 8,
    SegmentLeaderboardTypeRival = 9,
    SegmentLeaderboardTypeClubLeader = 10
} EnumSegmentLeaderboardType;

typedef enum {
    SegmentDeleteStatusDoNotDelete = 0,
    SegmentDeleteStatusDeleteOne = 1,
    SegmentDeleteStatusDeleteAll = 2
} EnumSegmentDeleteStatus;

typedef enum {
    SegmentSelectionTypeStarred = 0,
    SegmentSelectionTypeSuggested = 1
} EnumSegmentSelectionType;

typedef enum {
    SourceTypeAnt = 0,
    SourceTypeAntplus = 1,
    SourceTypeBluetooth = 2,
    SourceTypeBluetoothLowEnergy = 3,
    SourceTypeWifi = 4,
    SourceTypeLocal = 5
} EnumSourceType;

typedef enum {
    RiderPositionTypeSeated = 0,
    RiderPositionTypeStanding = 1
} EnumRiderPositionType;

typedef enum {
    PowerPhaseTypePowerPhaseStartAngle = 0,
    PowerPhaseTypePowerPhaseEndAngle = 1,
    PowerPhaseTypePowerPhaseArcLength = 2,
    PowerPhaseTypePowerPhaseCenter = 3
} EnumPowerPhaseType;

typedef enum {
    CameraEventTypeVideoStart = 0,
    CameraEventTypeVideoSplit = 1,
    CameraEventTypeVideoEnd = 2,
    CameraEventTypePhotoTaken = 3,
    CameraEventTypeVideoSecondStreamStart = 4,
    CameraEventTypeVideoSecondStreamSplit = 5,
    CameraEventTypeVideoSecondStreamEnd = 6,
    CameraEventTypeVideoSplitStart = 7,
    CameraEventTypeVideoSecondStreamSplitStart = 8
} EnumCameraEventType;

typedef enum {
    SensorTypeAccelerometer = 0,
    SensorTypeGyroscope = 1,
    SensorTypeCompass = 2
} EnumSensorType;

typedef enum {
    CommTimeoutTypeWildcardPairingTimeout = 0,
    CommTimeoutTypePairingTimeout = 1,
    CommTimeoutTypeConnectionLost = 2,
} EnumCommTimeoutType;

    typedef enum {
        CameraOrientationTypeCameraOrientation0 = 0,
        CameraOrientationTypeCameraOrientation90 = 1,
        CameraOrientationTypeCameraOrientation180 = 2,
        CameraOrientationTypeCameraOrientation270 = 3
    } EnumCameraOrientationType;

    typedef enum {
        AttitudeStageFailed = 0,
        AttitudeStageAligning = 1,
        AttitudeStageDegraded = 2,
        AttitudeStageValid = 3
    } EnumAttitudeStage;

    typedef enum {
        AttitudeValidityTrackAngleHeadingValid = 0x0001,
        AttitudeValidityPitchValid = 0x0002,
        AttitudeValidityRollValid = 0x0004,
        AttitudeValidityLateralBodyAccelValid = 0x0008,
        AttitudeValidityNormalBodyAccelValid = 0x0010,
        AttitudeValidityTurnRateValid = 0x0020,
        AttitudeValidityHwFail = 0x0040,
        AttitudeValidityMagInvalid = 0x0080,
        AttitudeValidityNoGps = 0x0100,
        AttitudeValidityGpsInvalid = 0x0200,
        AttitudeValiditySolutionCoasting = 0x0400,
        AttitudeValidityTrueTrackAngle = 0x0800,
        AttitudeValidityMagneticHeading = 0x1000
    } EnumAttitudeValidity;

// ----------- end generated code -----------

#endif // FIT_PROFILE_H
