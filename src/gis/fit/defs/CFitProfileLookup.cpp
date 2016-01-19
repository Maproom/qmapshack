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

#include "CFitProfileLookup.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_fields.h"

#include <QtWidgets>


// ----------- start generated code -----------
void initFileId(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("file_id", eMesgNumFileId);
    f->addField("type", fitEnumType, eFileIdType, 0, 0, "");
    f->addField("manufacturer", fitEnumType, eFileIdManufacturer, 0, 0, "");
    f->addField("product", fitUint16Type, eFileIdProduct, 0, 0, "");
    f->addSubfield("garmin_product", fitEnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fitEnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fitEnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", fitUint32zType, eFileIdSerialNumber, 0, 0, "");
    f->addField("time_created", fitEnumType, eFileIdTimeCreated, 0, 0, "");
    f->addField("number", fitUint16Type, eFileIdNumber, 0, 0, "");
    f->addField("product_name", fitStringType, eFileIdProductName, 0, 0, "");
    profiles.insert(eMesgNumFileId, f);
}

void initFileCreator(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("file_creator", eMesgNumFileCreator);
    f->addField("software_version", fitUint16Type, eFileCreatorSoftwareVersion, 0, 0, "");
    f->addField("hardware_version", fitUint8Type, eFileCreatorHardwareVersion, 0, 0, "");
    profiles.insert(eMesgNumFileCreator, f);
}

void initTimestampCorrelation(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("timestamp_correlation", eMesgNumTimestampCorrelation);
    f->addField("timestamp", fitEnumType, eTimestampCorrelationTimestamp, 0, 0, "s");
    f->addField("fractional_timestamp", fitUint16Type, eTimestampCorrelationFractionalTimestamp, 32768, 0, "s");
    f->addField("system_timestamp", fitEnumType, eTimestampCorrelationSystemTimestamp, 0, 0, "s");
    f->addField("fractional_system_timestamp", fitUint16Type, eTimestampCorrelationFractionalSystemTimestamp, 32768, 0, "s");
    f->addField("local_timestamp", fitEnumType, eTimestampCorrelationLocalTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fitUint16Type, eTimestampCorrelationTimestampMs, 0, 0, "ms");
    f->addField("system_timestamp_ms", fitUint16Type, eTimestampCorrelationSystemTimestampMs, 0, 0, "ms");
    profiles.insert(eMesgNumTimestampCorrelation, f);
}

void initSoftware(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("software", eMesgNumSoftware);
    f->addField("message_index", fitEnumType, eSoftwareMessageIndex, 0, 0, "");
    f->addField("version", fitUint16Type, eSoftwareVersion, 100, 0, "");
    f->addField("part_number", fitStringType, eSoftwarePartNumber, 0, 0, "");
    profiles.insert(eMesgNumSoftware, f);
}

void initSlaveDevice(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("slave_device", eMesgNumSlaveDevice);
    f->addField("manufacturer", fitEnumType, eSlaveDeviceManufacturer, 0, 0, "");
    f->addField("product", fitUint16Type, eSlaveDeviceProduct, 0, 0, "");
    f->addSubfield("garmin_product", fitEnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fitEnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fitEnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerDynastreamOem);
    profiles.insert(eMesgNumSlaveDevice, f);
}

void initCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("capabilities", eMesgNumCapabilities);
    f->addField("languages", fitUint8zType, eCapabilitiesLanguages, 0, 0, "");
    f->addField("sports", fitEnumType, eCapabilitiesSports, 0, 0, "");
    f->addField("workouts_supported", fitEnumType, eCapabilitiesWorkoutsSupported, 0, 0, "");
    f->addField("connectivity_supported", fitEnumType, eCapabilitiesConnectivitySupported, 0, 0, "");
    profiles.insert(eMesgNumCapabilities, f);
}

void initFileCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("file_capabilities", eMesgNumFileCapabilities);
    f->addField("message_index", fitEnumType, eFileCapabilitiesMessageIndex, 0, 0, "");
    f->addField("type", fitEnumType, eFileCapabilitiesType, 0, 0, "");
    f->addField("flags", fitEnumType, eFileCapabilitiesFlags, 0, 0, "");
    f->addField("directory", fitStringType, eFileCapabilitiesDirectory, 0, 0, "");
    f->addField("max_count", fitUint16Type, eFileCapabilitiesMaxCount, 0, 0, "");
    f->addField("max_size", fitUint32Type, eFileCapabilitiesMaxSize, 0, 0, "bytes");
    profiles.insert(eMesgNumFileCapabilities, f);
}

void initMesgCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("mesg_capabilities", eMesgNumMesgCapabilities);
    f->addField("message_index", fitEnumType, eMesgCapabilitiesMessageIndex, 0, 0, "");
    f->addField("file", fitEnumType, eMesgCapabilitiesFile, 0, 0, "");
    f->addField("mesg_num", fitEnumType, eMesgCapabilitiesMesgNum, 0, 0, "");
    f->addField("count_type", fitEnumType, eMesgCapabilitiesCountType, 0, 0, "");
    f->addField("count", fitUint16Type, eMesgCapabilitiesCount, 0, 0, "");
    f->addSubfield("num_per_file", fitUint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountNumPerFile);
    f->addSubfield("max_per_file", fitUint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountMaxPerFile);
    f->addSubfield("max_per_file_type", fitUint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountMaxPerFileType);
    profiles.insert(eMesgNumMesgCapabilities, f);
}

void initFieldCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("field_capabilities", eMesgNumFieldCapabilities);
    f->addField("message_index", fitEnumType, eFieldCapabilitiesMessageIndex, 0, 0, "");
    f->addField("file", fitEnumType, eFieldCapabilitiesFile, 0, 0, "");
    f->addField("mesg_num", fitEnumType, eFieldCapabilitiesMesgNum, 0, 0, "");
    f->addField("field_num", fitUint8Type, eFieldCapabilitiesFieldNum, 0, 0, "");
    f->addField("count", fitUint16Type, eFieldCapabilitiesCount, 0, 0, "");
    profiles.insert(eMesgNumFieldCapabilities, f);
}

void initDeviceSettings(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("device_settings", eMesgNumDeviceSettings);
    f->addField("active_time_zone", fitUint8Type, eDeviceSettingsActiveTimeZone, 0, 0, "");
    f->addField("utc_offset", fitUint32Type, eDeviceSettingsUtcOffset, 0, 0, "");
    f->addField("time_zone_offset", fitSint8Type, eDeviceSettingsTimeZoneOffset, 4, 0, "hr");
    profiles.insert(eMesgNumDeviceSettings, f);
}

void initUserProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("user_profile", eMesgNumUserProfile);
    f->addField("message_index", fitEnumType, eUserProfileMessageIndex, 0, 0, "");
    f->addField("friendly_name", fitStringType, eUserProfileFriendlyName, 0, 0, "");
    f->addField("gender", fitEnumType, eUserProfileGender, 0, 0, "");
    f->addField("age", fitUint8Type, eUserProfileAge, 0, 0, "years");
    f->addField("height", fitUint8Type, eUserProfileHeight, 100, 0, "m");
    f->addField("weight", fitUint16Type, eUserProfileWeight, 10, 0, "kg");
    f->addField("language", fitEnumType, eUserProfileLanguage, 0, 0, "");
    f->addField("elev_setting", fitEnumType, eUserProfileElevSetting, 0, 0, "");
    f->addField("weight_setting", fitEnumType, eUserProfileWeightSetting, 0, 0, "");
    f->addField("resting_heart_rate", fitUint8Type, eUserProfileRestingHeartRate, 0, 0, "bpm");
    f->addField("default_max_running_heart_rate", fitUint8Type, eUserProfileDefaultMaxRunningHeartRate, 0, 0, "bpm");
    f->addField("default_max_biking_heart_rate", fitUint8Type, eUserProfileDefaultMaxBikingHeartRate, 0, 0, "bpm");
    f->addField("default_max_heart_rate", fitUint8Type, eUserProfileDefaultMaxHeartRate, 0, 0, "bpm");
    f->addField("hr_setting", fitEnumType, eUserProfileHrSetting, 0, 0, "");
    f->addField("speed_setting", fitEnumType, eUserProfileSpeedSetting, 0, 0, "");
    f->addField("dist_setting", fitEnumType, eUserProfileDistSetting, 0, 0, "");
    f->addField("power_setting", fitEnumType, eUserProfilePowerSetting, 0, 0, "");
    f->addField("activity_class", fitEnumType, eUserProfileActivityClass, 0, 0, "");
    f->addField("position_setting", fitEnumType, eUserProfilePositionSetting, 0, 0, "");
    f->addField("temperature_setting", fitEnumType, eUserProfileTemperatureSetting, 0, 0, "");
    f->addField("local_id", fitEnumType, eUserProfileLocalId, 0, 0, "");
    f->addField("global_id", fitByteType, eUserProfileGlobalId, 0, 0, "");
    f->addField("height_setting", fitEnumType, eUserProfileHeightSetting, 0, 0, "");
    profiles.insert(eMesgNumUserProfile, f);
}

void initHrmProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hrm_profile", eMesgNumHrmProfile);
    f->addField("message_index", fitEnumType, eHrmProfileMessageIndex, 0, 0, "");
    f->addField("enabled", fitEnumType, eHrmProfileEnabled, 0, 0, "");
    f->addField("hrm_ant_id", fitUint16zType, eHrmProfileHrmAntId, 0, 0, "");
    f->addField("log_hrv", fitEnumType, eHrmProfileLogHrv, 0, 0, "");
    f->addField("hrm_ant_id_trans_type", fitUint8zType, eHrmProfileHrmAntIdTransType, 0, 0, "");
    profiles.insert(eMesgNumHrmProfile, f);
}

void initSdmProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("sdm_profile", eMesgNumSdmProfile);
    f->addField("message_index", fitEnumType, eSdmProfileMessageIndex, 0, 0, "");
    f->addField("enabled", fitEnumType, eSdmProfileEnabled, 0, 0, "");
    f->addField("sdm_ant_id", fitUint16zType, eSdmProfileSdmAntId, 0, 0, "");
    f->addField("sdm_cal_factor", fitUint16Type, eSdmProfileSdmCalFactor, 10, 0, "%");
    f->addField("odometer", fitUint32Type, eSdmProfileOdometer, 100, 0, "m");
    f->addField("speed_source", fitEnumType, eSdmProfileSpeedSource, 0, 0, "");
    f->addField("sdm_ant_id_trans_type", fitUint8zType, eSdmProfileSdmAntIdTransType, 0, 0, "");
    f->addField("odometer_rollover", fitUint8Type, eSdmProfileOdometerRollover, 0, 0, "");
    profiles.insert(eMesgNumSdmProfile, f);
}

void initBikeProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("bike_profile", eMesgNumBikeProfile);
    f->addField("message_index", fitEnumType, eBikeProfileMessageIndex, 0, 0, "");
    f->addField("name", fitStringType, eBikeProfileName, 0, 0, "");
    f->addField("sport", fitEnumType, eBikeProfileSport, 0, 0, "");
    f->addField("sub_sport", fitEnumType, eBikeProfileSubSport, 0, 0, "");
    f->addField("odometer", fitUint32Type, eBikeProfileOdometer, 100, 0, "m");
    f->addField("bike_spd_ant_id", fitUint16zType, eBikeProfileBikeSpdAntId, 0, 0, "");
    f->addField("bike_cad_ant_id", fitUint16zType, eBikeProfileBikeCadAntId, 0, 0, "");
    f->addField("bike_spdcad_ant_id", fitUint16zType, eBikeProfileBikeSpdcadAntId, 0, 0, "");
    f->addField("bike_power_ant_id", fitUint16zType, eBikeProfileBikePowerAntId, 0, 0, "");
    f->addField("custom_wheelsize", fitUint16Type, eBikeProfileCustomWheelsize, 1000, 0, "m");
    f->addField("auto_wheelsize", fitUint16Type, eBikeProfileAutoWheelsize, 1000, 0, "m");
    f->addField("bike_weight", fitUint16Type, eBikeProfileBikeWeight, 10, 0, "kg");
    f->addField("power_cal_factor", fitUint16Type, eBikeProfilePowerCalFactor, 10, 0, "%");
    f->addField("auto_wheel_cal", fitEnumType, eBikeProfileAutoWheelCal, 0, 0, "");
    f->addField("auto_power_zero", fitEnumType, eBikeProfileAutoPowerZero, 0, 0, "");
    f->addField("id", fitUint8Type, eBikeProfileId, 0, 0, "");
    f->addField("spd_enabled", fitEnumType, eBikeProfileSpdEnabled, 0, 0, "");
    f->addField("cad_enabled", fitEnumType, eBikeProfileCadEnabled, 0, 0, "");
    f->addField("spdcad_enabled", fitEnumType, eBikeProfileSpdcadEnabled, 0, 0, "");
    f->addField("power_enabled", fitEnumType, eBikeProfilePowerEnabled, 0, 0, "");
    f->addField("crank_length", fitUint8Type, eBikeProfileCrankLength, 2, -110, "mm");
    f->addField("enabled", fitEnumType, eBikeProfileEnabled, 0, 0, "");
    f->addField("bike_spd_ant_id_trans_type", fitUint8zType, eBikeProfileBikeSpdAntIdTransType, 0, 0, "");
    f->addField("bike_cad_ant_id_trans_type", fitUint8zType, eBikeProfileBikeCadAntIdTransType, 0, 0, "");
    f->addField("bike_spdcad_ant_id_trans_type", fitUint8zType, eBikeProfileBikeSpdcadAntIdTransType, 0, 0, "");
    f->addField("bike_power_ant_id_trans_type", fitUint8zType, eBikeProfileBikePowerAntIdTransType, 0, 0, "");
    f->addField("odometer_rollover", fitUint8Type, eBikeProfileOdometerRollover, 0, 0, "");
    f->addField("front_gear_num", fitUint8zType, eBikeProfileFrontGearNum, 0, 0, "");
    f->addField("front_gear", fitUint8zType, eBikeProfileFrontGear, 0, 0, "");
    f->addField("rear_gear_num", fitUint8zType, eBikeProfileRearGearNum, 0, 0, "");
    f->addField("rear_gear", fitUint8zType, eBikeProfileRearGear, 0, 0, "");
    f->addField("shimano_di2_enabled", fitEnumType, eBikeProfileShimanoDi2Enabled, 0, 0, "");
    profiles.insert(eMesgNumBikeProfile, f);
}

void initZonesTarget(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("zones_target", eMesgNumZonesTarget);
    f->addField("max_heart_rate", fitUint8Type, eZonesTargetMaxHeartRate, 0, 0, "");
    f->addField("threshold_heart_rate", fitUint8Type, eZonesTargetThresholdHeartRate, 0, 0, "");
    f->addField("functional_threshold_power", fitUint16Type, eZonesTargetFunctionalThresholdPower, 0, 0, "");
    f->addField("hr_calc_type", fitEnumType, eZonesTargetHrCalcType, 0, 0, "");
    f->addField("pwr_calc_type", fitEnumType, eZonesTargetPwrCalcType, 0, 0, "");
    profiles.insert(eMesgNumZonesTarget, f);
}

void initSport(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("sport", eMesgNumSport);
    f->addField("sport", fitEnumType, eSportSport, 0, 0, "");
    f->addField("sub_sport", fitEnumType, eSportSubSport, 0, 0, "");
    f->addField("name", fitStringType, eSportName, 0, 0, "");
    profiles.insert(eMesgNumSport, f);
}

void initHrZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hr_zone", eMesgNumHrZone);
    f->addField("message_index", fitEnumType, eHrZoneMessageIndex, 0, 0, "");
    f->addField("high_bpm", fitUint8Type, eHrZoneHighBpm, 0, 0, "bpm");
    f->addField("name", fitStringType, eHrZoneName, 0, 0, "");
    profiles.insert(eMesgNumHrZone, f);
}

void initSpeedZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("speed_zone", eMesgNumSpeedZone);
    f->addField("message_index", fitEnumType, eSpeedZoneMessageIndex, 0, 0, "");
    f->addField("high_value", fitUint16Type, eSpeedZoneHighValue, 1000, 0, "m/s");
    f->addField("name", fitStringType, eSpeedZoneName, 0, 0, "");
    profiles.insert(eMesgNumSpeedZone, f);
}

void initCadenceZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("cadence_zone", eMesgNumCadenceZone);
    f->addField("message_index", fitEnumType, eCadenceZoneMessageIndex, 0, 0, "");
    f->addField("high_value", fitUint8Type, eCadenceZoneHighValue, 0, 0, "rpm");
    f->addField("name", fitStringType, eCadenceZoneName, 0, 0, "");
    profiles.insert(eMesgNumCadenceZone, f);
}

void initPowerZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("power_zone", eMesgNumPowerZone);
    f->addField("message_index", fitEnumType, ePowerZoneMessageIndex, 0, 0, "");
    f->addField("high_value", fitUint16Type, ePowerZoneHighValue, 0, 0, "watts");
    f->addField("name", fitStringType, ePowerZoneName, 0, 0, "");
    profiles.insert(eMesgNumPowerZone, f);
}

void initMetZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("met_zone", eMesgNumMetZone);
    f->addField("message_index", fitEnumType, eMetZoneMessageIndex, 0, 0, "");
    f->addField("high_bpm", fitUint8Type, eMetZoneHighBpm, 0, 0, "");
    f->addField("calories", fitUint16Type, eMetZoneCalories, 10, 0, "kcal / min");
    f->addField("fat_calories", fitUint8Type, eMetZoneFatCalories, 10, 0, "kcal / min");
    profiles.insert(eMesgNumMetZone, f);
}

void initGoal(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("goal", eMesgNumGoal);
    f->addField("message_index", fitEnumType, eGoalMessageIndex, 0, 0, "");
    f->addField("sport", fitEnumType, eGoalSport, 0, 0, "");
    f->addField("sub_sport", fitEnumType, eGoalSubSport, 0, 0, "");
    f->addField("start_date", fitEnumType, eGoalStartDate, 0, 0, "");
    f->addField("end_date", fitEnumType, eGoalEndDate, 0, 0, "");
    f->addField("type", fitEnumType, eGoalType, 0, 0, "");
    f->addField("value", fitUint32Type, eGoalValue, 0, 0, "");
    f->addField("repeat", fitEnumType, eGoalRepeat, 0, 0, "");
    f->addField("target_value", fitUint32Type, eGoalTargetValue, 0, 0, "");
    f->addField("recurrence", fitEnumType, eGoalRecurrence, 0, 0, "");
    f->addField("recurrence_value", fitUint16Type, eGoalRecurrenceValue, 0, 0, "");
    f->addField("enabled", fitEnumType, eGoalEnabled, 0, 0, "");
    profiles.insert(eMesgNumGoal, f);
}

void initActivity(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("activity", eMesgNumActivity);
    f->addField("timestamp", fitEnumType, eActivityTimestamp, 0, 0, "");
    f->addField("total_timer_time", fitUint32Type, eActivityTotalTimerTime, 1000, 0, "s");
    f->addField("num_sessions", fitUint16Type, eActivityNumSessions, 0, 0, "");
    f->addField("type", fitEnumType, eActivityType, 0, 0, "");
    f->addField("event", fitEnumType, eActivityEvent, 0, 0, "");
    f->addField("event_type", fitEnumType, eActivityEventType, 0, 0, "");
    f->addField("local_timestamp", fitEnumType, eActivityLocalTimestamp, 0, 0, "");
    f->addField("event_group", fitUint8Type, eActivityEventGroup, 0, 0, "");
    profiles.insert(eMesgNumActivity, f);
}

void initSession(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("session", eMesgNumSession);
    f->addField("message_index", fitEnumType, eSessionMessageIndex, 0, 0, "");
    f->addField("timestamp", fitEnumType, eSessionTimestamp, 0, 0, "s");
    f->addField("event", fitEnumType, eSessionEvent, 0, 0, "");
    f->addField("event_type", fitEnumType, eSessionEventType, 0, 0, "");
    f->addField("start_time", fitEnumType, eSessionStartTime, 0, 0, "");
    f->addField("start_position_lat", fitSint32Type, eSessionStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", fitSint32Type, eSessionStartPositionLong, 0, 0, "semicircles");
    f->addField("sport", fitEnumType, eSessionSport, 0, 0, "");
    f->addField("sub_sport", fitEnumType, eSessionSubSport, 0, 0, "");
    f->addField("total_elapsed_time", fitUint32Type, eSessionTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", fitUint32Type, eSessionTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", fitUint32Type, eSessionTotalDistance, 100, 0, "m");
    f->addField("total_cycles", fitUint32Type, eSessionTotalCycles, 0, 0, "cycles");
    f->addSubfield("total_strides", fitUint32Type, eSessionTotalCycles, 0, 0, "strides", eSessionSport, eSportRunning);
    f->addSubfield("total_strides", fitUint32Type, eSessionTotalCycles, 0, 0, "", eSessionSport, eSportWalking);
    f->addField("total_calories", fitUint16Type, eSessionTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", fitUint16Type, eSessionTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", fitUint16Type, eSessionAvgSpeed, 0, 0, "");
    f->addComponent("avg_speed", fitUint16Type, eSessionAvgSpeed, 1000, 0, "m/s", eSessionEnhancedAvgSpeed, 16);
    f->addField("max_speed", fitUint16Type, eSessionMaxSpeed, 0, 0, "");
    f->addComponent("max_speed", fitUint16Type, eSessionMaxSpeed, 1000, 0, "m/s", eSessionEnhancedMaxSpeed, 16);
    f->addField("avg_heart_rate", fitUint8Type, eSessionAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", fitUint8Type, eSessionMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", fitUint8Type, eSessionAvgCadence, 0, 0, "rpm");
    f->addSubfield("avg_running_cadence", fitUint8Type, eSessionAvgCadence, 0, 0, "strides/min", eSessionSport, eSportRunning);
    f->addField("max_cadence", fitUint8Type, eSessionMaxCadence, 0, 0, "rpm");
    f->addSubfield("max_running_cadence", fitUint8Type, eSessionMaxCadence, 0, 0, "strides/min", eSessionSport, eSportRunning);
    f->addField("avg_power", fitUint16Type, eSessionAvgPower, 0, 0, "watts");
    f->addField("max_power", fitUint16Type, eSessionMaxPower, 0, 0, "watts");
    f->addField("total_ascent", fitUint16Type, eSessionTotalAscent, 0, 0, "m");
    f->addField("total_descent", fitUint16Type, eSessionTotalDescent, 0, 0, "m");
    f->addField("total_training_effect", fitUint8Type, eSessionTotalTrainingEffect, 10, 0, "");
    f->addField("first_lap_index", fitUint16Type, eSessionFirstLapIndex, 0, 0, "");
    f->addField("num_laps", fitUint16Type, eSessionNumLaps, 0, 0, "");
    f->addField("event_group", fitUint8Type, eSessionEventGroup, 0, 0, "");
    f->addField("trigger", fitEnumType, eSessionTrigger, 0, 0, "");
    f->addField("nec_lat", fitSint32Type, eSessionNecLat, 0, 0, "semicircles");
    f->addField("nec_long", fitSint32Type, eSessionNecLong, 0, 0, "semicircles");
    f->addField("swc_lat", fitSint32Type, eSessionSwcLat, 0, 0, "semicircles");
    f->addField("swc_long", fitSint32Type, eSessionSwcLong, 0, 0, "semicircles");
    f->addField("normalized_power", fitUint16Type, eSessionNormalizedPower, 0, 0, "watts");
    f->addField("training_stress_score", fitUint16Type, eSessionTrainingStressScore, 10, 0, "tss");
    f->addField("intensity_factor", fitUint16Type, eSessionIntensityFactor, 1000, 0, "if");
    f->addField("left_right_balance", fitEnumType, eSessionLeftRightBalance, 0, 0, "");
    f->addField("avg_stroke_count", fitUint32Type, eSessionAvgStrokeCount, 10, 0, "strokes/lap");
    f->addField("avg_stroke_distance", fitUint16Type, eSessionAvgStrokeDistance, 100, 0, "m");
    f->addField("swim_stroke", fitEnumType, eSessionSwimStroke, 0, 0, "swim_stroke");
    f->addField("pool_length", fitUint16Type, eSessionPoolLength, 100, 0, "m");
    f->addField("threshold_power", fitUint16Type, eSessionThresholdPower, 0, 0, "watts");
    f->addField("pool_length_unit", fitEnumType, eSessionPoolLengthUnit, 0, 0, "");
    f->addField("num_active_lengths", fitUint16Type, eSessionNumActiveLengths, 0, 0, "lengths");
    f->addField("total_work", fitUint32Type, eSessionTotalWork, 0, 0, "J");
    f->addField("avg_altitude", fitUint16Type, eSessionAvgAltitude, 0, 0, "");
    f->addComponent("avg_altitude", fitUint16Type, eSessionAvgAltitude, 5, 500, "m", eSessionEnhancedAvgAltitude, 16);
    f->addField("max_altitude", fitUint16Type, eSessionMaxAltitude, 0, 0, "");
    f->addComponent("max_altitude", fitUint16Type, eSessionMaxAltitude, 5, 500, "m", eSessionEnhancedMaxAltitude, 16);
    f->addField("gps_accuracy", fitUint8Type, eSessionGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", fitSint16Type, eSessionAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", fitSint16Type, eSessionAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", fitSint16Type, eSessionAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", fitSint16Type, eSessionMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", fitSint16Type, eSessionMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", fitSint8Type, eSessionAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", fitSint8Type, eSessionMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", fitUint32Type, eSessionTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", fitSint16Type, eSessionAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", fitSint16Type, eSessionAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", fitSint16Type, eSessionMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", fitSint16Type, eSessionMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("min_heart_rate", fitUint8Type, eSessionMinHeartRate, 0, 0, "bpm");
    f->addField("time_in_hr_zone", fitUint32Type, eSessionTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", fitUint32Type, eSessionTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", fitUint32Type, eSessionTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", fitUint32Type, eSessionTimeInPowerZone, 1000, 0, "s");
    f->addField("avg_lap_time", fitUint32Type, eSessionAvgLapTime, 1000, 0, "s");
    f->addField("best_lap_index", fitUint16Type, eSessionBestLapIndex, 0, 0, "");
    f->addField("min_altitude", fitUint16Type, eSessionMinAltitude, 0, 0, "");
    f->addComponent("min_altitude", fitUint16Type, eSessionMinAltitude, 5, 500, "m", eSessionEnhancedMinAltitude, 16);
    f->addField("player_score", fitUint16Type, eSessionPlayerScore, 0, 0, "");
    f->addField("opponent_score", fitUint16Type, eSessionOpponentScore, 0, 0, "");
    f->addField("opponent_name", fitStringType, eSessionOpponentName, 0, 0, "");
    f->addField("stroke_count", fitUint16Type, eSessionStrokeCount, 0, 0, "counts");
    f->addField("zone_count", fitUint16Type, eSessionZoneCount, 0, 0, "counts");
    f->addField("max_ball_speed", fitUint16Type, eSessionMaxBallSpeed, 100, 0, "m/s");
    f->addField("avg_ball_speed", fitUint16Type, eSessionAvgBallSpeed, 100, 0, "m/s");
    f->addField("avg_vertical_oscillation", fitUint16Type, eSessionAvgVerticalOscillation, 10, 0, "mm");
    f->addField("avg_stance_time_percent", fitUint16Type, eSessionAvgStanceTimePercent, 100, 0, "percent");
    f->addField("avg_stance_time", fitUint16Type, eSessionAvgStanceTime, 10, 0, "ms");
    f->addField("avg_fractional_cadence", fitUint8Type, eSessionAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", fitUint8Type, eSessionMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", fitUint8Type, eSessionTotalFractionalCycles, 128, 0, "cycles");
    f->addField("avg_total_hemoglobin_conc", fitUint16Type, eSessionAvgTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("min_total_hemoglobin_conc", fitUint16Type, eSessionMinTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("max_total_hemoglobin_conc", fitUint16Type, eSessionMaxTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("avg_saturated_hemoglobin_percent", fitUint16Type, eSessionAvgSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("min_saturated_hemoglobin_percent", fitUint16Type, eSessionMinSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("max_saturated_hemoglobin_percent", fitUint16Type, eSessionMaxSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("avg_left_torque_effectiveness", fitUint8Type, eSessionAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", fitUint8Type, eSessionAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", fitUint8Type, eSessionAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", fitUint8Type, eSessionAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", fitUint8Type, eSessionAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("sport_index", fitUint8Type, eSessionSportIndex, 0, 0, "");
    f->addField("time_standing", fitUint32Type, eSessionTimeStanding, 1000, 0, "s");
    f->addField("stand_count", fitUint16Type, eSessionStandCount, 0, 0, "");
    f->addField("avg_left_pco", fitSint8Type, eSessionAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", fitSint8Type, eSessionAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", fitUint8Type, eSessionAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", fitUint8Type, eSessionAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", fitUint8Type, eSessionAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", fitUint8Type, eSessionAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", fitUint16Type, eSessionAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", fitUint16Type, eSessionMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", fitUint8Type, eSessionAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", fitUint8Type, eSessionMaxCadencePosition, 0, 0, "rpm");
    f->addField("enhanced_avg_speed", fitUint32Type, eSessionEnhancedAvgSpeed, 1000, 0, "m/s");
    f->addField("enhanced_max_speed", fitUint32Type, eSessionEnhancedMaxSpeed, 1000, 0, "m/s");
    f->addField("enhanced_avg_altitude", fitUint32Type, eSessionEnhancedAvgAltitude, 5, 500, "m");
    f->addField("enhanced_min_altitude", fitUint32Type, eSessionEnhancedMinAltitude, 5, 500, "m");
    f->addField("enhanced_max_altitude", fitUint32Type, eSessionEnhancedMaxAltitude, 5, 500, "m");
    f->addField("avg_lev_motor_power", fitUint16Type, eSessionAvgLevMotorPower, 0, 0, "watts");
    f->addField("max_lev_motor_power", fitUint16Type, eSessionMaxLevMotorPower, 0, 0, "watts");
    f->addField("lev_battery_consumption", fitUint8Type, eSessionLevBatteryConsumption, 2, 0, "percent");
    profiles.insert(eMesgNumSession, f);
}

void initLap(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("lap", eMesgNumLap);
    f->addField("message_index", fitEnumType, eLapMessageIndex, 0, 0, "");
    f->addField("timestamp", fitEnumType, eLapTimestamp, 0, 0, "s");
    f->addField("event", fitEnumType, eLapEvent, 0, 0, "");
    f->addField("event_type", fitEnumType, eLapEventType, 0, 0, "");
    f->addField("start_time", fitEnumType, eLapStartTime, 0, 0, "");
    f->addField("start_position_lat", fitSint32Type, eLapStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", fitSint32Type, eLapStartPositionLong, 0, 0, "semicircles");
    f->addField("end_position_lat", fitSint32Type, eLapEndPositionLat, 0, 0, "semicircles");
    f->addField("end_position_long", fitSint32Type, eLapEndPositionLong, 0, 0, "semicircles");
    f->addField("total_elapsed_time", fitUint32Type, eLapTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", fitUint32Type, eLapTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", fitUint32Type, eLapTotalDistance, 100, 0, "m");
    f->addField("total_cycles", fitUint32Type, eLapTotalCycles, 0, 0, "cycles");
    f->addSubfield("total_strides", fitUint32Type, eLapTotalCycles, 0, 0, "strides", eLapSport, eSportRunning);
    f->addSubfield("total_strides", fitUint32Type, eLapTotalCycles, 0, 0, "", eLapSport, eSportWalking);
    f->addField("total_calories", fitUint16Type, eLapTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", fitUint16Type, eLapTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", fitUint16Type, eLapAvgSpeed, 0, 0, "");
    f->addComponent("avg_speed", fitUint16Type, eLapAvgSpeed, 1000, 0, "m/s", eLapEnhancedAvgSpeed, 16);
    f->addField("max_speed", fitUint16Type, eLapMaxSpeed, 0, 0, "");
    f->addComponent("max_speed", fitUint16Type, eLapMaxSpeed, 1000, 0, "m/s", eLapEnhancedMaxSpeed, 16);
    f->addField("avg_heart_rate", fitUint8Type, eLapAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", fitUint8Type, eLapMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", fitUint8Type, eLapAvgCadence, 0, 0, "rpm");
    f->addSubfield("avg_running_cadence", fitUint8Type, eLapAvgCadence, 0, 0, "strides/min", eLapSport, eSportRunning);
    f->addField("max_cadence", fitUint8Type, eLapMaxCadence, 0, 0, "rpm");
    f->addSubfield("max_running_cadence", fitUint8Type, eLapMaxCadence, 0, 0, "strides/min", eLapSport, eSportRunning);
    f->addField("avg_power", fitUint16Type, eLapAvgPower, 0, 0, "watts");
    f->addField("max_power", fitUint16Type, eLapMaxPower, 0, 0, "watts");
    f->addField("total_ascent", fitUint16Type, eLapTotalAscent, 0, 0, "m");
    f->addField("total_descent", fitUint16Type, eLapTotalDescent, 0, 0, "m");
    f->addField("intensity", fitEnumType, eLapIntensity, 0, 0, "");
    f->addField("lap_trigger", fitEnumType, eLapLapTrigger, 0, 0, "");
    f->addField("sport", fitEnumType, eLapSport, 0, 0, "");
    f->addField("event_group", fitUint8Type, eLapEventGroup, 0, 0, "");
    f->addField("num_lengths", fitUint16Type, eLapNumLengths, 0, 0, "lengths");
    f->addField("normalized_power", fitUint16Type, eLapNormalizedPower, 0, 0, "watts");
    f->addField("left_right_balance", fitEnumType, eLapLeftRightBalance, 0, 0, "");
    f->addField("first_length_index", fitUint16Type, eLapFirstLengthIndex, 0, 0, "");
    f->addField("avg_stroke_distance", fitUint16Type, eLapAvgStrokeDistance, 100, 0, "m");
    f->addField("swim_stroke", fitEnumType, eLapSwimStroke, 0, 0, "");
    f->addField("sub_sport", fitEnumType, eLapSubSport, 0, 0, "");
    f->addField("num_active_lengths", fitUint16Type, eLapNumActiveLengths, 0, 0, "lengths");
    f->addField("total_work", fitUint32Type, eLapTotalWork, 0, 0, "J");
    f->addField("avg_altitude", fitUint16Type, eLapAvgAltitude, 0, 0, "");
    f->addComponent("avg_altitude", fitUint16Type, eLapAvgAltitude, 5, 500, "m", eLapEnhancedAvgAltitude, 16);
    f->addField("max_altitude", fitUint16Type, eLapMaxAltitude, 0, 0, "");
    f->addComponent("max_altitude", fitUint16Type, eLapMaxAltitude, 5, 500, "m", eLapEnhancedMaxAltitude, 16);
    f->addField("gps_accuracy", fitUint8Type, eLapGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", fitSint16Type, eLapAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", fitSint16Type, eLapAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", fitSint16Type, eLapAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", fitSint16Type, eLapMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", fitSint16Type, eLapMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", fitSint8Type, eLapAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", fitSint8Type, eLapMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", fitUint32Type, eLapTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", fitSint16Type, eLapAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", fitSint16Type, eLapAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", fitSint16Type, eLapMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", fitSint16Type, eLapMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("time_in_hr_zone", fitUint32Type, eLapTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", fitUint32Type, eLapTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", fitUint32Type, eLapTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", fitUint32Type, eLapTimeInPowerZone, 1000, 0, "s");
    f->addField("repetition_num", fitUint16Type, eLapRepetitionNum, 0, 0, "");
    f->addField("min_altitude", fitUint16Type, eLapMinAltitude, 0, 0, "");
    f->addComponent("min_altitude", fitUint16Type, eLapMinAltitude, 5, 500, "m", eLapEnhancedMinAltitude, 16);
    f->addField("min_heart_rate", fitUint8Type, eLapMinHeartRate, 0, 0, "bpm");
    f->addField("wkt_step_index", fitEnumType, eLapWktStepIndex, 0, 0, "");
    f->addField("opponent_score", fitUint16Type, eLapOpponentScore, 0, 0, "");
    f->addField("stroke_count", fitUint16Type, eLapStrokeCount, 0, 0, "counts");
    f->addField("zone_count", fitUint16Type, eLapZoneCount, 0, 0, "counts");
    f->addField("avg_vertical_oscillation", fitUint16Type, eLapAvgVerticalOscillation, 10, 0, "mm");
    f->addField("avg_stance_time_percent", fitUint16Type, eLapAvgStanceTimePercent, 100, 0, "percent");
    f->addField("avg_stance_time", fitUint16Type, eLapAvgStanceTime, 10, 0, "ms");
    f->addField("avg_fractional_cadence", fitUint8Type, eLapAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", fitUint8Type, eLapMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", fitUint8Type, eLapTotalFractionalCycles, 128, 0, "cycles");
    f->addField("player_score", fitUint16Type, eLapPlayerScore, 0, 0, "");
    f->addField("avg_total_hemoglobin_conc", fitUint16Type, eLapAvgTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("min_total_hemoglobin_conc", fitUint16Type, eLapMinTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("max_total_hemoglobin_conc", fitUint16Type, eLapMaxTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("avg_saturated_hemoglobin_percent", fitUint16Type, eLapAvgSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("min_saturated_hemoglobin_percent", fitUint16Type, eLapMinSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("max_saturated_hemoglobin_percent", fitUint16Type, eLapMaxSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("avg_left_torque_effectiveness", fitUint8Type, eLapAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", fitUint8Type, eLapAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", fitUint8Type, eLapAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", fitUint8Type, eLapAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", fitUint8Type, eLapAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("time_standing", fitUint32Type, eLapTimeStanding, 1000, 0, "s");
    f->addField("stand_count", fitUint16Type, eLapStandCount, 0, 0, "");
    f->addField("avg_left_pco", fitSint8Type, eLapAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", fitSint8Type, eLapAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", fitUint8Type, eLapAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", fitUint8Type, eLapAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", fitUint8Type, eLapAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", fitUint8Type, eLapAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", fitUint16Type, eLapAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", fitUint16Type, eLapMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", fitUint8Type, eLapAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", fitUint8Type, eLapMaxCadencePosition, 0, 0, "rpm");
    f->addField("enhanced_avg_speed", fitUint32Type, eLapEnhancedAvgSpeed, 1000, 0, "m/s");
    f->addField("enhanced_max_speed", fitUint32Type, eLapEnhancedMaxSpeed, 1000, 0, "m/s");
    f->addField("enhanced_avg_altitude", fitUint32Type, eLapEnhancedAvgAltitude, 5, 500, "m");
    f->addField("enhanced_min_altitude", fitUint32Type, eLapEnhancedMinAltitude, 5, 500, "m");
    f->addField("enhanced_max_altitude", fitUint32Type, eLapEnhancedMaxAltitude, 5, 500, "m");
    f->addField("avg_lev_motor_power", fitUint16Type, eLapAvgLevMotorPower, 0, 0, "watts");
    f->addField("max_lev_motor_power", fitUint16Type, eLapMaxLevMotorPower, 0, 0, "watts");
    f->addField("lev_battery_consumption", fitUint8Type, eLapLevBatteryConsumption, 2, 0, "percent");
    profiles.insert(eMesgNumLap, f);
}

void initLength(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("length", eMesgNumLength);
    f->addField("message_index", fitEnumType, eLengthMessageIndex, 0, 0, "");
    f->addField("timestamp", fitEnumType, eLengthTimestamp, 0, 0, "");
    f->addField("event", fitEnumType, eLengthEvent, 0, 0, "");
    f->addField("event_type", fitEnumType, eLengthEventType, 0, 0, "");
    f->addField("start_time", fitEnumType, eLengthStartTime, 0, 0, "");
    f->addField("total_elapsed_time", fitUint32Type, eLengthTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", fitUint32Type, eLengthTotalTimerTime, 1000, 0, "s");
    f->addField("total_strokes", fitUint16Type, eLengthTotalStrokes, 0, 0, "strokes");
    f->addField("avg_speed", fitUint16Type, eLengthAvgSpeed, 1000, 0, "m/s");
    f->addField("swim_stroke", fitEnumType, eLengthSwimStroke, 0, 0, "swim_stroke");
    f->addField("avg_swimming_cadence", fitUint8Type, eLengthAvgSwimmingCadence, 0, 0, "strokes/min");
    f->addField("event_group", fitUint8Type, eLengthEventGroup, 0, 0, "");
    f->addField("total_calories", fitUint16Type, eLengthTotalCalories, 0, 0, "kcal");
    f->addField("length_type", fitEnumType, eLengthLengthType, 0, 0, "");
    f->addField("player_score", fitUint16Type, eLengthPlayerScore, 0, 0, "");
    f->addField("opponent_score", fitUint16Type, eLengthOpponentScore, 0, 0, "");
    f->addField("stroke_count", fitUint16Type, eLengthStrokeCount, 0, 0, "counts");
    f->addField("zone_count", fitUint16Type, eLengthZoneCount, 0, 0, "counts");
    profiles.insert(eMesgNumLength, f);
}

void initRecord(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("record", eMesgNumRecord);
    f->addField("timestamp", fitEnumType, eRecordTimestamp, 0, 0, "s");
    f->addField("position_lat", fitSint32Type, eRecordPositionLat, 0, 0, "semicircles");
    f->addField("position_long", fitSint32Type, eRecordPositionLong, 0, 0, "semicircles");
    f->addField("altitude", fitUint16Type, eRecordAltitude, 0, 0, "");
    f->addComponent("altitude", fitUint16Type, eRecordAltitude, 5, 500, "m", eRecordEnhancedAltitude, 16);
    f->addField("heart_rate", fitUint8Type, eRecordHeartRate, 0, 0, "bpm");
    f->addField("cadence", fitUint8Type, eRecordCadence, 0, 0, "rpm");
    f->addField("distance", fitUint32Type, eRecordDistance, 100, 0, "m");
    f->addField("speed", fitUint16Type, eRecordSpeed, 0, 0, "");
    f->addComponent("speed", fitUint16Type, eRecordSpeed, 1000, 0, "m/s", eRecordEnhancedSpeed, 16);
    f->addField("power", fitUint16Type, eRecordPower, 0, 0, "watts");
    f->addField("compressed_speed_distance", fitByteType, eRecordCompressedSpeedDistance, 0, 0, "");
    f->addComponent("compressed_speed_distance", fitByteType, eRecordCompressedSpeedDistance, 100, 0, "m/s", eRecordSpeed, 12);
    f->addComponent("compressed_speed_distance", fitByteType, eRecordCompressedSpeedDistance, 16, 0, "m", eRecordDistance, 12);
    f->addField("grade", fitSint16Type, eRecordGrade, 100, 0, "%");
    f->addField("resistance", fitUint8Type, eRecordResistance, 0, 0, "");
    f->addField("time_from_course", fitSint32Type, eRecordTimeFromCourse, 1000, 0, "s");
    f->addField("cycle_length", fitUint8Type, eRecordCycleLength, 100, 0, "m");
    f->addField("temperature", fitSint8Type, eRecordTemperature, 0, 0, "C");
    f->addField("speed_1s", fitUint8Type, eRecordSpeed1s, 16, 0, "m/s");
    f->addField("cycles", fitUint8Type, eRecordCycles, 0, 0, "");
    f->addComponent("cycles", fitUint8Type, eRecordCycles, 0, 0, "cycles", eRecordTotalCycles, 8);
    f->addField("total_cycles", fitUint32Type, eRecordTotalCycles, 0, 0, "cycles");
    f->addField("compressed_accumulated_power", fitUint16Type, eRecordCompressedAccumulatedPower, 0, 0, "");
    f->addComponent("compressed_accumulated_power", fitUint16Type, eRecordCompressedAccumulatedPower, 0, 0, "watts", eRecordAccumulatedPower, 16);
    f->addField("accumulated_power", fitUint32Type, eRecordAccumulatedPower, 0, 0, "watts");
    f->addField("left_right_balance", fitEnumType, eRecordLeftRightBalance, 0, 0, "");
    f->addField("gps_accuracy", fitUint8Type, eRecordGpsAccuracy, 0, 0, "m");
    f->addField("vertical_speed", fitSint16Type, eRecordVerticalSpeed, 1000, 0, "m/s");
    f->addField("calories", fitUint16Type, eRecordCalories, 0, 0, "kcal");
    f->addField("vertical_oscillation", fitUint16Type, eRecordVerticalOscillation, 10, 0, "mm");
    f->addField("stance_time_percent", fitUint16Type, eRecordStanceTimePercent, 100, 0, "percent");
    f->addField("stance_time", fitUint16Type, eRecordStanceTime, 10, 0, "ms");
    f->addField("activity_type", fitEnumType, eRecordActivityType, 0, 0, "");
    f->addField("left_torque_effectiveness", fitUint8Type, eRecordLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("right_torque_effectiveness", fitUint8Type, eRecordRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("left_pedal_smoothness", fitUint8Type, eRecordLeftPedalSmoothness, 2, 0, "percent");
    f->addField("right_pedal_smoothness", fitUint8Type, eRecordRightPedalSmoothness, 2, 0, "percent");
    f->addField("combined_pedal_smoothness", fitUint8Type, eRecordCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("time128", fitUint8Type, eRecordTime128, 128, 0, "s");
    f->addField("stroke_type", fitEnumType, eRecordStrokeType, 0, 0, "");
    f->addField("zone", fitUint8Type, eRecordZone, 0, 0, "");
    f->addField("ball_speed", fitUint16Type, eRecordBallSpeed, 100, 0, "m/s");
    f->addField("cadence256", fitUint16Type, eRecordCadence256, 256, 0, "rpm");
    f->addField("fractional_cadence", fitUint8Type, eRecordFractionalCadence, 128, 0, "rpm");
    f->addField("total_hemoglobin_conc", fitUint16Type, eRecordTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("total_hemoglobin_conc_min", fitUint16Type, eRecordTotalHemoglobinConcMin, 100, 0, "g/dL");
    f->addField("total_hemoglobin_conc_max", fitUint16Type, eRecordTotalHemoglobinConcMax, 100, 0, "g/dL");
    f->addField("saturated_hemoglobin_percent", fitUint16Type, eRecordSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("saturated_hemoglobin_percent_min", fitUint16Type, eRecordSaturatedHemoglobinPercentMin, 10, 0, "%");
    f->addField("saturated_hemoglobin_percent_max", fitUint16Type, eRecordSaturatedHemoglobinPercentMax, 10, 0, "%");
    f->addField("device_index", fitEnumType, eRecordDeviceIndex, 0, 0, "");
    f->addField("left_pco", fitSint8Type, eRecordLeftPco, 0, 0, "mm");
    f->addField("right_pco", fitSint8Type, eRecordRightPco, 0, 0, "mm");
    f->addField("left_power_phase", fitUint8Type, eRecordLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("left_power_phase_peak", fitUint8Type, eRecordLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("right_power_phase", fitUint8Type, eRecordRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("right_power_phase_peak", fitUint8Type, eRecordRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("enhanced_speed", fitUint32Type, eRecordEnhancedSpeed, 1000, 0, "m/s");
    f->addField("enhanced_altitude", fitUint32Type, eRecordEnhancedAltitude, 5, 500, "m");
    f->addField("battery_soc", fitUint8Type, eRecordBatterySoc, 2, 0, "percent");
    f->addField("motor_power", fitUint16Type, eRecordMotorPower, 0, 0, "watts");
    profiles.insert(eMesgNumRecord, f);
}

void initEvent(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("event", eMesgNumEvent);
    f->addField("timestamp", fitEnumType, eEventTimestamp, 0, 0, "s");
    f->addField("event", fitEnumType, eEventEvent, 0, 0, "");
    f->addField("event_type", fitEnumType, eEventEventType, 0, 0, "");
    f->addField("data16", fitUint16Type, eEventData16, 0, 0, "");
    f->addComponent("data16", fitUint16Type, eEventData16, 0, 0, "", eEventData, 16);
    f->addField("data", fitUint32Type, eEventData, 0, 0, "");
    f->addSubfield("timer_trigger", fitEnumType, eEventData, 0, 0, "", eEventEvent, eEventTimer);
    f->addSubfield("course_point_index", fitEnumType, eEventData, 0, 0, "", eEventEvent, eEventCoursePoint);
    f->addSubfield("battery_level", fitUint16Type, eEventData, 1000, 0, "V", eEventEvent, eEventBattery);
    f->addSubfield("virtual_partner_speed", fitUint16Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventVirtualPartnerPace);
    f->addSubfield("hr_high_alert", fitUint8Type, eEventData, 0, 0, "bpm", eEventEvent, eEventHrHighAlert);
    f->addSubfield("hr_low_alert", fitUint8Type, eEventData, 0, 0, "bpm", eEventEvent, eEventHrLowAlert);
    f->addSubfield("speed_high_alert", fitUint32Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventSpeedHighAlert);
    f->addSubfield("speed_low_alert", fitUint32Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventSpeedLowAlert);
    f->addSubfield("cad_high_alert", fitUint16Type, eEventData, 0, 0, "rpm", eEventEvent, eEventCadHighAlert);
    f->addSubfield("cad_low_alert", fitUint16Type, eEventData, 0, 0, "rpm", eEventEvent, eEventCadLowAlert);
    f->addSubfield("power_high_alert", fitUint16Type, eEventData, 0, 0, "watts", eEventEvent, eEventPowerHighAlert);
    f->addSubfield("power_low_alert", fitUint16Type, eEventData, 0, 0, "watts", eEventEvent, eEventPowerLowAlert);
    f->addSubfield("time_duration_alert", fitUint32Type, eEventData, 1000, 0, "s", eEventEvent, eEventTimeDurationAlert);
    f->addSubfield("distance_duration_alert", fitUint32Type, eEventData, 100, 0, "m", eEventEvent, eEventDistanceDurationAlert);
    f->addSubfield("calorie_duration_alert", fitUint32Type, eEventData, 0, 0, "calories", eEventEvent, eEventCalorieDurationAlert);
    f->addSubfield("fitness_equipment_state", fitEnumType, eEventData, 0, 0, "", eEventEvent, eEventFitnessEquipment);
    f->addSubfield("sport_point", fitUint32Type, eEventData, 0, 0, "", eEventEvent, eEventSportPoint);
    f->addComponent(16, "sport_point", fitUint32Type, eEventData, 1, 0, "", eEventScore, 16);
    f->addComponent(16, "sport_point", fitUint32Type, eEventData, 1, 0, "", eEventOpponentScore, 16);
    f->addSubfield("gear_change_data", fitUint32Type, eEventData, 0, 0, "", eEventEvent, eEventFrontGearChange);
    f->addSubfield("gear_change_data", fitUint32Type, eEventData, 0, 0, "", eEventEvent, eEventRearGearChange);
    f->addComponent(18, "gear_change_data", fitUint32Type, eEventData, 1, 0, "", eEventRearGearNum, 8);
    f->addComponent(18, "gear_change_data", fitUint32Type, eEventData, 1, 0, "", eEventRearGear, 8);
    f->addComponent(18, "gear_change_data", fitUint32Type, eEventData, 1, 0, "", eEventFrontGearNum, 8);
    f->addComponent(18, "gear_change_data", fitUint32Type, eEventData, 1, 0, "", eEventFrontGear, 8);
    f->addSubfield("rider_position", fitEnumType, eEventData, 0, 0, "", eEventEvent, eEventRiderPositionChange);
    f->addSubfield("comm_timeout", fitEnumType, eEventData, 0, 0, "", eEventEvent, eEventCommTimeout);
    f->addField("event_group", fitUint8Type, eEventEventGroup, 0, 0, "");
    f->addField("score", fitUint16Type, eEventScore, 0, 0, "");
    f->addField("opponent_score", fitUint16Type, eEventOpponentScore, 0, 0, "");
    f->addField("front_gear_num", fitUint8zType, eEventFrontGearNum, 0, 0, "");
    f->addField("front_gear", fitUint8zType, eEventFrontGear, 0, 0, "");
    f->addField("rear_gear_num", fitUint8zType, eEventRearGearNum, 0, 0, "");
    f->addField("rear_gear", fitUint8zType, eEventRearGear, 0, 0, "");
    f->addField("device_index", fitEnumType, eEventDeviceIndex, 0, 0, "");
    profiles.insert(eMesgNumEvent, f);
}

void initDeviceInfo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("device_info", eMesgNumDeviceInfo);
    f->addField("timestamp", fitEnumType, eDeviceInfoTimestamp, 0, 0, "s");
    f->addField("device_index", fitEnumType, eDeviceInfoDeviceIndex, 0, 0, "");
    f->addField("device_type", fitUint8Type, eDeviceInfoDeviceType, 0, 0, "");
    f->addSubfield("antplus_device_type", fitEnumType, eDeviceInfoDeviceType, 0, 0, "", eDeviceInfoSourceType, eSourceTypeAntplus);
    f->addSubfield("ant_device_type", fitUint8Type, eDeviceInfoDeviceType, 0, 0, "", eDeviceInfoSourceType, eSourceTypeAnt);
    f->addField("manufacturer", fitEnumType, eDeviceInfoManufacturer, 0, 0, "");
    f->addField("serial_number", fitUint32zType, eDeviceInfoSerialNumber, 0, 0, "");
    f->addField("product", fitUint16Type, eDeviceInfoProduct, 0, 0, "");
    f->addSubfield("garmin_product", fitEnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fitEnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fitEnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerDynastreamOem);
    f->addField("software_version", fitUint16Type, eDeviceInfoSoftwareVersion, 100, 0, "");
    f->addField("hardware_version", fitUint8Type, eDeviceInfoHardwareVersion, 0, 0, "");
    f->addField("cum_operating_time", fitUint32Type, eDeviceInfoCumOperatingTime, 0, 0, "s");
    f->addField("battery_voltage", fitUint16Type, eDeviceInfoBatteryVoltage, 256, 0, "V");
    f->addField("battery_status", fitEnumType, eDeviceInfoBatteryStatus, 0, 0, "");
    f->addField("sensor_position", fitEnumType, eDeviceInfoSensorPosition, 0, 0, "");
    f->addField("descriptor", fitStringType, eDeviceInfoDescriptor, 0, 0, "");
    f->addField("ant_transmission_type", fitUint8zType, eDeviceInfoAntTransmissionType, 0, 0, "");
    f->addField("ant_device_number", fitUint16zType, eDeviceInfoAntDeviceNumber, 0, 0, "");
    f->addField("ant_network", fitEnumType, eDeviceInfoAntNetwork, 0, 0, "");
    f->addField("source_type", fitEnumType, eDeviceInfoSourceType, 0, 0, "");
    f->addField("product_name", fitStringType, eDeviceInfoProductName, 0, 0, "");
    profiles.insert(eMesgNumDeviceInfo, f);
}

void initTrainingFile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("training_file", eMesgNumTrainingFile);
    f->addField("timestamp", fitEnumType, eTrainingFileTimestamp, 0, 0, "");
    f->addField("type", fitEnumType, eTrainingFileType, 0, 0, "");
    f->addField("manufacturer", fitEnumType, eTrainingFileManufacturer, 0, 0, "");
    f->addField("product", fitUint16Type, eTrainingFileProduct, 0, 0, "");
    f->addSubfield("garmin_product", fitEnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fitEnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fitEnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", fitUint32zType, eTrainingFileSerialNumber, 0, 0, "");
    f->addField("time_created", fitEnumType, eTrainingFileTimeCreated, 0, 0, "");
    profiles.insert(eMesgNumTrainingFile, f);
}

void initHrv(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hrv", eMesgNumHrv);
    f->addField("time", fitUint16Type, eHrvTime, 1000, 0, "s");
    profiles.insert(eMesgNumHrv, f);
}

void initCameraEvent(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("camera_event", eMesgNumCameraEvent);
    f->addField("timestamp", fitEnumType, eCameraEventTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fitUint16Type, eCameraEventTimestampMs, 0, 0, "ms");
    f->addField("camera_event_type", fitEnumType, eCameraEventCameraEventType, 0, 0, "");
    f->addField("camera_file_uuid", fitStringType, eCameraEventCameraFileUuid, 0, 0, "");
    f->addField("camera_orientation", fitEnumType, eCameraEventCameraOrientation, 0, 0, "");
    profiles.insert(eMesgNumCameraEvent, f);
}

void initGyroscopeData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("gyroscope_data", eMesgNumGyroscopeData);
    f->addField("timestamp", fitEnumType, eGyroscopeDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fitUint16Type, eGyroscopeDataTimestampMs, 0, 0, "ms");
    f->addField("sample_time_offset", fitUint16Type, eGyroscopeDataSampleTimeOffset, 0, 0, "ms");
    f->addField("gyro_x", fitUint16Type, eGyroscopeDataGyroX, 0, 0, "counts");
    f->addField("gyro_y", fitUint16Type, eGyroscopeDataGyroY, 0, 0, "counts");
    f->addField("gyro_z", fitUint16Type, eGyroscopeDataGyroZ, 0, 0, "counts");
    f->addField("calibrated_gyro_x", fitFloat32Type, eGyroscopeDataCalibratedGyroX, 0, 0, "deg/s");
    f->addField("calibrated_gyro_y", fitFloat32Type, eGyroscopeDataCalibratedGyroY, 0, 0, "deg/s");
    f->addField("calibrated_gyro_z", fitFloat32Type, eGyroscopeDataCalibratedGyroZ, 0, 0, "deg/s");
    profiles.insert(eMesgNumGyroscopeData, f);
}

void initAccelerometerData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("accelerometer_data", eMesgNumAccelerometerData);
    f->addField("timestamp", fitEnumType, eAccelerometerDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fitUint16Type, eAccelerometerDataTimestampMs, 0, 0, "ms");
    f->addField("sample_time_offset", fitUint16Type, eAccelerometerDataSampleTimeOffset, 0, 0, "ms");
    f->addField("accel_x", fitUint16Type, eAccelerometerDataAccelX, 0, 0, "counts");
    f->addField("accel_y", fitUint16Type, eAccelerometerDataAccelY, 0, 0, "counts");
    f->addField("accel_z", fitUint16Type, eAccelerometerDataAccelZ, 0, 0, "counts");
    f->addField("calibrated_accel_x", fitFloat32Type, eAccelerometerDataCalibratedAccelX, 0, 0, "g");
    f->addField("calibrated_accel_y", fitFloat32Type, eAccelerometerDataCalibratedAccelY, 0, 0, "g");
    f->addField("calibrated_accel_z", fitFloat32Type, eAccelerometerDataCalibratedAccelZ, 0, 0, "g");
    profiles.insert(eMesgNumAccelerometerData, f);
}

void initThreeDSensorCalibration(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("three_d_sensor_calibration", eMesgNumThreeDSensorCalibration);
    f->addField("timestamp", fitEnumType, eThreeDSensorCalibrationTimestamp, 0, 0, "s");
    f->addField("sensor_type", fitEnumType, eThreeDSensorCalibrationSensorType, 0, 0, "");
    f->addField("calibration_factor", fitUint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "");
    f->addSubfield("accel_cal_factor", fitUint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "g", eThreeDSensorCalibrationSensorType, eSensorTypeAccelerometer);
    f->addSubfield("gyro_cal_factor", fitUint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "deg/s", eThreeDSensorCalibrationSensorType, eSensorTypeGyroscope);
    f->addField("calibration_divisor", fitUint32Type, eThreeDSensorCalibrationCalibrationDivisor, 0, 0, "counts");
    f->addField("level_shift", fitUint32Type, eThreeDSensorCalibrationLevelShift, 0, 0, "");
    f->addField("offset_cal", fitSint32Type, eThreeDSensorCalibrationOffsetCal, 0, 0, "");
    f->addField("orientation_matrix", fitSint32Type, eThreeDSensorCalibrationOrientationMatrix, 65535, 0, "");
    profiles.insert(eMesgNumThreeDSensorCalibration, f);
}

void initVideoFrame(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_frame", eMesgNumVideoFrame);
    f->addField("timestamp", fitEnumType, eVideoFrameTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fitUint16Type, eVideoFrameTimestampMs, 0, 0, "ms");
    f->addField("frame_number", fitUint32Type, eVideoFrameFrameNumber, 0, 0, "");
    profiles.insert(eMesgNumVideoFrame, f);
}

void initObdiiData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("obdii_data", eMesgNumObdiiData);
    f->addField("timestamp", fitEnumType, eObdiiDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fitUint16Type, eObdiiDataTimestampMs, 0, 0, "ms");
    f->addField("time_offset", fitUint16Type, eObdiiDataTimeOffset, 0, 0, "ms");
    f->addField("pid", fitByteType, eObdiiDataPid, 0, 0, "");
    f->addField("raw_data", fitByteType, eObdiiDataRawData, 0, 0, "");
    f->addField("pid_data_size", fitUint8Type, eObdiiDataPidDataSize, 0, 0, "");
    f->addField("system_time", fitUint32Type, eObdiiDataSystemTime, 0, 0, "");
    f->addField("start_timestamp", fitEnumType, eObdiiDataStartTimestamp, 0, 0, "");
    f->addField("start_timestamp_ms", fitUint16Type, eObdiiDataStartTimestampMs, 0, 0, "ms");
    profiles.insert(eMesgNumObdiiData, f);
}

void initNmeaSentence(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("nmea_sentence", eMesgNumNmeaSentence);
    f->addField("timestamp", fitEnumType, eNmeaSentenceTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fitUint16Type, eNmeaSentenceTimestampMs, 0, 0, "ms");
    f->addField("sentence", fitStringType, eNmeaSentenceSentence, 0, 0, "");
    profiles.insert(eMesgNumNmeaSentence, f);
}

void initAviationAttitude(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("aviation_attitude", eMesgNumAviationAttitude);
    f->addField("timestamp", fitEnumType, eAviationAttitudeTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fitUint16Type, eAviationAttitudeTimestampMs, 0, 0, "ms");
    f->addField("system_time", fitUint32Type, eAviationAttitudeSystemTime, 0, 0, "ms");
    f->addField("pitch", fitSint16Type, eAviationAttitudePitch, 10430.38, 0, "radians");
    f->addField("roll", fitSint16Type, eAviationAttitudeRoll, 10430.38, 0, "radians");
    f->addField("accel_lateral", fitSint16Type, eAviationAttitudeAccelLateral, 100, 0, "m/s^2");
    f->addField("accel_normal", fitSint16Type, eAviationAttitudeAccelNormal, 100, 0, "m/s^2");
    f->addField("turn_rate", fitSint16Type, eAviationAttitudeTurnRate, 1024, 0, "radians/second");
    f->addField("stage", fitEnumType, eAviationAttitudeStage, 0, 0, "");
    f->addField("attitude_stage_complete", fitUint8Type, eAviationAttitudeAttitudeStageComplete, 0, 0, "%");
    f->addField("track", fitUint16Type, eAviationAttitudeTrack, 10430.38, 0, "radians");
    f->addField("validity", fitEnumType, eAviationAttitudeValidity, 0, 0, "");
    profiles.insert(eMesgNumAviationAttitude, f);
}

void initVideo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video", eMesgNumVideo);
    f->addField("url", fitStringType, eVideoUrl, 0, 0, "");
    f->addField("hosting_provider", fitStringType, eVideoHostingProvider, 0, 0, "");
    f->addField("duration", fitUint32Type, eVideoDuration, 0, 0, "ms");
    profiles.insert(eMesgNumVideo, f);
}

void initVideoTitle(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_title", eMesgNumVideoTitle);
    f->addField("message_index", fitEnumType, eVideoTitleMessageIndex, 0, 0, "");
    f->addField("message_count", fitUint16Type, eVideoTitleMessageCount, 0, 0, "");
    f->addField("text", fitStringType, eVideoTitleText, 0, 0, "");
    profiles.insert(eMesgNumVideoTitle, f);
}

void initVideoDescription(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_description", eMesgNumVideoDescription);
    f->addField("message_index", fitEnumType, eVideoDescriptionMessageIndex, 0, 0, "");
    f->addField("message_count", fitUint16Type, eVideoDescriptionMessageCount, 0, 0, "");
    f->addField("text", fitStringType, eVideoDescriptionText, 0, 0, "");
    profiles.insert(eMesgNumVideoDescription, f);
}

void initVideoClip(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_clip", eMesgNumVideoClip);
    f->addField("clip_number", fitUint16Type, eVideoClipClipNumber, 0, 0, "");
    f->addField("start_timestamp", fitEnumType, eVideoClipStartTimestamp, 0, 0, "");
    f->addField("start_timestamp_ms", fitUint16Type, eVideoClipStartTimestampMs, 0, 0, "");
    f->addField("end_timestamp", fitEnumType, eVideoClipEndTimestamp, 0, 0, "");
    f->addField("end_timestamp_ms", fitUint16Type, eVideoClipEndTimestampMs, 0, 0, "");
    f->addField("clip_start", fitUint32Type, eVideoClipClipStart, 0, 0, "ms");
    f->addField("clip_end", fitUint32Type, eVideoClipClipEnd, 0, 0, "ms");
    profiles.insert(eMesgNumVideoClip, f);
}

void initCourse(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("course", eMesgNumCourse);
    f->addField("sport", fitEnumType, eCourseSport, 0, 0, "");
    f->addField("name", fitStringType, eCourseName, 0, 0, "");
    f->addField("capabilities", fitEnumType, eCourseCapabilities, 0, 0, "");
    profiles.insert(eMesgNumCourse, f);
}

void initCoursePoint(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("course_point", eMesgNumCoursePoint);
    f->addField("message_index", fitEnumType, eCoursePointMessageIndex, 0, 0, "");
    f->addField("timestamp", fitEnumType, eCoursePointTimestamp, 0, 0, "");
    f->addField("position_lat", fitSint32Type, eCoursePointPositionLat, 0, 0, "semicircles");
    f->addField("position_long", fitSint32Type, eCoursePointPositionLong, 0, 0, "semicircles");
    f->addField("distance", fitUint32Type, eCoursePointDistance, 100, 0, "m");
    f->addField("type", fitEnumType, eCoursePointType, 0, 0, "");
    f->addField("name", fitStringType, eCoursePointName, 0, 0, "");
    f->addField("favorite", fitEnumType, eCoursePointFavorite, 0, 0, "");
    profiles.insert(eMesgNumCoursePoint, f);
}

void initSegmentId(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_id", eMesgNumSegmentId);
    f->addField("name", fitStringType, eSegmentIdName, 0, 0, "");
    f->addField("uuid", fitStringType, eSegmentIdUuid, 0, 0, "");
    f->addField("sport", fitEnumType, eSegmentIdSport, 0, 0, "");
    f->addField("enabled", fitEnumType, eSegmentIdEnabled, 0, 0, "");
    f->addField("user_profile_primary_key", fitUint32Type, eSegmentIdUserProfilePrimaryKey, 0, 0, "");
    f->addField("device_id", fitUint32Type, eSegmentIdDeviceId, 0, 0, "");
    f->addField("default_race_leader", fitUint8Type, eSegmentIdDefaultRaceLeader, 0, 0, "");
    f->addField("delete_status", fitEnumType, eSegmentIdDeleteStatus, 0, 0, "");
    f->addField("selection_type", fitEnumType, eSegmentIdSelectionType, 0, 0, "");
    profiles.insert(eMesgNumSegmentId, f);
}

void initSegmentLeaderboardEntry(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_leaderboard_entry", eMesgNumSegmentLeaderboardEntry);
    f->addField("message_index", fitEnumType, eSegmentLeaderboardEntryMessageIndex, 0, 0, "");
    f->addField("name", fitStringType, eSegmentLeaderboardEntryName, 0, 0, "");
    f->addField("type", fitEnumType, eSegmentLeaderboardEntryType, 0, 0, "");
    f->addField("group_primary_key", fitUint32Type, eSegmentLeaderboardEntryGroupPrimaryKey, 0, 0, "");
    f->addField("activity_id", fitUint32Type, eSegmentLeaderboardEntryActivityId, 0, 0, "");
    f->addField("segment_time", fitUint32Type, eSegmentLeaderboardEntrySegmentTime, 1000, 0, "s");
    f->addField("activity_id_string", fitStringType, eSegmentLeaderboardEntryActivityIdString, 0, 0, "");
    profiles.insert(eMesgNumSegmentLeaderboardEntry, f);
}

void initSegmentPoint(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_point", eMesgNumSegmentPoint);
    f->addField("message_index", fitEnumType, eSegmentPointMessageIndex, 0, 0, "");
    f->addField("position_lat", fitSint32Type, eSegmentPointPositionLat, 0, 0, "semicircles");
    f->addField("position_long", fitSint32Type, eSegmentPointPositionLong, 0, 0, "semicircles");
    f->addField("distance", fitUint32Type, eSegmentPointDistance, 100, 0, "m");
    f->addField("altitude", fitUint16Type, eSegmentPointAltitude, 5, 500, "m");
    f->addField("leader_time", fitUint32Type, eSegmentPointLeaderTime, 1000, 0, "s");
    profiles.insert(eMesgNumSegmentPoint, f);
}

void initSegmentLap(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_lap", eMesgNumSegmentLap);
    f->addField("message_index", fitEnumType, eSegmentLapMessageIndex, 0, 0, "");
    f->addField("timestamp", fitEnumType, eSegmentLapTimestamp, 0, 0, "s");
    f->addField("event", fitEnumType, eSegmentLapEvent, 0, 0, "");
    f->addField("event_type", fitEnumType, eSegmentLapEventType, 0, 0, "");
    f->addField("start_time", fitEnumType, eSegmentLapStartTime, 0, 0, "");
    f->addField("start_position_lat", fitSint32Type, eSegmentLapStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", fitSint32Type, eSegmentLapStartPositionLong, 0, 0, "semicircles");
    f->addField("end_position_lat", fitSint32Type, eSegmentLapEndPositionLat, 0, 0, "semicircles");
    f->addField("end_position_long", fitSint32Type, eSegmentLapEndPositionLong, 0, 0, "semicircles");
    f->addField("total_elapsed_time", fitUint32Type, eSegmentLapTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", fitUint32Type, eSegmentLapTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", fitUint32Type, eSegmentLapTotalDistance, 100, 0, "m");
    f->addField("total_cycles", fitUint32Type, eSegmentLapTotalCycles, 0, 0, "cycles");
    f->addSubfield("total_strokes", fitUint32Type, eSegmentLapTotalCycles, 0, 0, "strokes", eSegmentLapSport, eSportCycling);
    f->addField("total_calories", fitUint16Type, eSegmentLapTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", fitUint16Type, eSegmentLapTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", fitUint16Type, eSegmentLapAvgSpeed, 1000, 0, "m/s");
    f->addField("max_speed", fitUint16Type, eSegmentLapMaxSpeed, 1000, 0, "m/s");
    f->addField("avg_heart_rate", fitUint8Type, eSegmentLapAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", fitUint8Type, eSegmentLapMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", fitUint8Type, eSegmentLapAvgCadence, 0, 0, "rpm");
    f->addField("max_cadence", fitUint8Type, eSegmentLapMaxCadence, 0, 0, "rpm");
    f->addField("avg_power", fitUint16Type, eSegmentLapAvgPower, 0, 0, "watts");
    f->addField("max_power", fitUint16Type, eSegmentLapMaxPower, 0, 0, "watts");
    f->addField("total_ascent", fitUint16Type, eSegmentLapTotalAscent, 0, 0, "m");
    f->addField("total_descent", fitUint16Type, eSegmentLapTotalDescent, 0, 0, "m");
    f->addField("sport", fitEnumType, eSegmentLapSport, 0, 0, "");
    f->addField("event_group", fitUint8Type, eSegmentLapEventGroup, 0, 0, "");
    f->addField("nec_lat", fitSint32Type, eSegmentLapNecLat, 0, 0, "semicircles");
    f->addField("nec_long", fitSint32Type, eSegmentLapNecLong, 0, 0, "semicircles");
    f->addField("swc_lat", fitSint32Type, eSegmentLapSwcLat, 0, 0, "semicircles");
    f->addField("swc_long", fitSint32Type, eSegmentLapSwcLong, 0, 0, "semicircles");
    f->addField("name", fitStringType, eSegmentLapName, 0, 0, "");
    f->addField("normalized_power", fitUint16Type, eSegmentLapNormalizedPower, 0, 0, "watts");
    f->addField("left_right_balance", fitEnumType, eSegmentLapLeftRightBalance, 0, 0, "");
    f->addField("sub_sport", fitEnumType, eSegmentLapSubSport, 0, 0, "");
    f->addField("total_work", fitUint32Type, eSegmentLapTotalWork, 0, 0, "J");
    f->addField("avg_altitude", fitUint16Type, eSegmentLapAvgAltitude, 5, 500, "m");
    f->addField("max_altitude", fitUint16Type, eSegmentLapMaxAltitude, 5, 500, "m");
    f->addField("gps_accuracy", fitUint8Type, eSegmentLapGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", fitSint16Type, eSegmentLapAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", fitSint16Type, eSegmentLapAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", fitSint16Type, eSegmentLapAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", fitSint16Type, eSegmentLapMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", fitSint16Type, eSegmentLapMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", fitSint8Type, eSegmentLapAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", fitSint8Type, eSegmentLapMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", fitUint32Type, eSegmentLapTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", fitSint16Type, eSegmentLapAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", fitSint16Type, eSegmentLapAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", fitSint16Type, eSegmentLapMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", fitSint16Type, eSegmentLapMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("time_in_hr_zone", fitUint32Type, eSegmentLapTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", fitUint32Type, eSegmentLapTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", fitUint32Type, eSegmentLapTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", fitUint32Type, eSegmentLapTimeInPowerZone, 1000, 0, "s");
    f->addField("repetition_num", fitUint16Type, eSegmentLapRepetitionNum, 0, 0, "");
    f->addField("min_altitude", fitUint16Type, eSegmentLapMinAltitude, 5, 500, "m");
    f->addField("min_heart_rate", fitUint8Type, eSegmentLapMinHeartRate, 0, 0, "bpm");
    f->addField("active_time", fitUint32Type, eSegmentLapActiveTime, 1000, 0, "s");
    f->addField("wkt_step_index", fitEnumType, eSegmentLapWktStepIndex, 0, 0, "");
    f->addField("sport_event", fitEnumType, eSegmentLapSportEvent, 0, 0, "");
    f->addField("avg_left_torque_effectiveness", fitUint8Type, eSegmentLapAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", fitUint8Type, eSegmentLapAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", fitUint8Type, eSegmentLapAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", fitUint8Type, eSegmentLapAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", fitUint8Type, eSegmentLapAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("status", fitEnumType, eSegmentLapStatus, 0, 0, "");
    f->addField("uuid", fitStringType, eSegmentLapUuid, 0, 0, "");
    f->addField("avg_fractional_cadence", fitUint8Type, eSegmentLapAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", fitUint8Type, eSegmentLapMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", fitUint8Type, eSegmentLapTotalFractionalCycles, 128, 0, "cycles");
    f->addField("front_gear_shift_count", fitUint16Type, eSegmentLapFrontGearShiftCount, 0, 0, "");
    f->addField("rear_gear_shift_count", fitUint16Type, eSegmentLapRearGearShiftCount, 0, 0, "");
    f->addField("time_standing", fitUint32Type, eSegmentLapTimeStanding, 1000, 0, "s");
    f->addField("stand_count", fitUint16Type, eSegmentLapStandCount, 0, 0, "");
    f->addField("avg_left_pco", fitSint8Type, eSegmentLapAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", fitSint8Type, eSegmentLapAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", fitUint8Type, eSegmentLapAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", fitUint8Type, eSegmentLapAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", fitUint8Type, eSegmentLapAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", fitUint8Type, eSegmentLapAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", fitUint16Type, eSegmentLapAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", fitUint16Type, eSegmentLapMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", fitUint8Type, eSegmentLapAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", fitUint8Type, eSegmentLapMaxCadencePosition, 0, 0, "rpm");
    profiles.insert(eMesgNumSegmentLap, f);
}

void initSegmentFile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_file", eMesgNumSegmentFile);
    f->addField("message_index", fitEnumType, eSegmentFileMessageIndex, 0, 0, "");
    f->addField("file_uuid", fitStringType, eSegmentFileFileUuid, 0, 0, "");
    f->addField("enabled", fitEnumType, eSegmentFileEnabled, 0, 0, "");
    f->addField("user_profile_primary_key", fitUint32Type, eSegmentFileUserProfilePrimaryKey, 0, 0, "");
    f->addField("leader_type", fitEnumType, eSegmentFileLeaderType, 0, 0, "");
    f->addField("leader_group_primary_key", fitUint32Type, eSegmentFileLeaderGroupPrimaryKey, 0, 0, "");
    f->addField("leader_activity_id", fitUint32Type, eSegmentFileLeaderActivityId, 0, 0, "");
    f->addField("leader_activity_id_string", fitStringType, eSegmentFileLeaderActivityIdString, 0, 0, "");
    profiles.insert(eMesgNumSegmentFile, f);
}

void initWorkout(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("workout", eMesgNumWorkout);
    f->addField("sport", fitEnumType, eWorkoutSport, 0, 0, "");
    f->addField("capabilities", fitEnumType, eWorkoutCapabilities, 0, 0, "");
    f->addField("num_valid_steps", fitUint16Type, eWorkoutNumValidSteps, 0, 0, "");
    f->addField("wkt_name", fitStringType, eWorkoutWktName, 0, 0, "");
    profiles.insert(eMesgNumWorkout, f);
}

void initWorkoutStep(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("workout_step", eMesgNumWorkoutStep);
    f->addField("message_index", fitEnumType, eWorkoutStepMessageIndex, 0, 0, "");
    f->addField("wkt_step_name", fitStringType, eWorkoutStepWktStepName, 0, 0, "");
    f->addField("duration_type", fitEnumType, eWorkoutStepDurationType, 0, 0, "");
    f->addField("duration_value", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "");
    f->addSubfield("duration_time", fitUint32Type, eWorkoutStepDurationValue, 1000, 0, "s", eWorkoutStepDurationType, eWktStepDurationTime);
    f->addSubfield("duration_time", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepetitionTime);
    f->addSubfield("duration_distance", fitUint32Type, eWorkoutStepDurationValue, 100, 0, "m", eWorkoutStepDurationType, eWktStepDurationDistance);
    f->addSubfield("duration_hr", fitEnumType, eWorkoutStepDurationValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationHrLessThan);
    f->addSubfield("duration_hr", fitEnumType, eWorkoutStepDurationValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationHrGreaterThan);
    f->addSubfield("duration_calories", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "calories", eWorkoutStepDurationType, eWktStepDurationCalories);
    f->addSubfield("duration_step", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilStepsCmplt);
    f->addSubfield("duration_step", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilTime);
    f->addSubfield("duration_step", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilDistance);
    f->addSubfield("duration_step", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilCalories);
    f->addSubfield("duration_step", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrLessThan);
    f->addSubfield("duration_step", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrGreaterThan);
    f->addSubfield("duration_step", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerLessThan);
    f->addSubfield("duration_step", fitUint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerGreaterThan);
    f->addSubfield("duration_power", fitEnumType, eWorkoutStepDurationValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationPowerLessThan);
    f->addSubfield("duration_power", fitEnumType, eWorkoutStepDurationValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationPowerGreaterThan);
    f->addField("target_type", fitEnumType, eWorkoutStepTargetType, 0, 0, "");
    f->addField("target_value", fitUint32Type, eWorkoutStepTargetValue, 0, 0, "");
    f->addSubfield("target_hr_zone", fitUint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("target_power_zone", fitUint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addSubfield("repeat_steps", fitUint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilStepsCmplt);
    f->addSubfield("repeat_time", fitUint32Type, eWorkoutStepTargetValue, 1000, 0, "s", eWorkoutStepDurationType, eWktStepDurationRepeatUntilTime);
    f->addSubfield("repeat_distance", fitUint32Type, eWorkoutStepTargetValue, 100, 0, "m", eWorkoutStepDurationType, eWktStepDurationRepeatUntilDistance);
    f->addSubfield("repeat_calories", fitUint32Type, eWorkoutStepTargetValue, 0, 0, "calories", eWorkoutStepDurationType, eWktStepDurationRepeatUntilCalories);
    f->addSubfield("repeat_hr", fitEnumType, eWorkoutStepTargetValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrLessThan);
    f->addSubfield("repeat_hr", fitEnumType, eWorkoutStepTargetValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrGreaterThan);
    f->addSubfield("repeat_power", fitEnumType, eWorkoutStepTargetValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerLessThan);
    f->addSubfield("repeat_power", fitEnumType, eWorkoutStepTargetValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerGreaterThan);
    f->addField("custom_target_value_low", fitUint32Type, eWorkoutStepCustomTargetValueLow, 0, 0, "");
    f->addSubfield("custom_target_speed_low", fitUint32Type, eWorkoutStepCustomTargetValueLow, 1000, 0, "m/s", eWorkoutStepTargetType, eWktStepTargetSpeed);
    f->addSubfield("custom_target_heart_rate_low", fitEnumType, eWorkoutStepCustomTargetValueLow, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("custom_target_cadence_low", fitUint32Type, eWorkoutStepCustomTargetValueLow, 0, 0, "rpm", eWorkoutStepTargetType, eWktStepTargetCadence);
    f->addSubfield("custom_target_power_low", fitEnumType, eWorkoutStepCustomTargetValueLow, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addField("custom_target_value_high", fitUint32Type, eWorkoutStepCustomTargetValueHigh, 0, 0, "");
    f->addSubfield("custom_target_speed_high", fitUint32Type, eWorkoutStepCustomTargetValueHigh, 1000, 0, "m/s", eWorkoutStepTargetType, eWktStepTargetSpeed);
    f->addSubfield("custom_target_heart_rate_high", fitEnumType, eWorkoutStepCustomTargetValueHigh, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("custom_target_cadence_high", fitUint32Type, eWorkoutStepCustomTargetValueHigh, 0, 0, "rpm", eWorkoutStepTargetType, eWktStepTargetCadence);
    f->addSubfield("custom_target_power_high", fitEnumType, eWorkoutStepCustomTargetValueHigh, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addField("intensity", fitEnumType, eWorkoutStepIntensity, 0, 0, "");
    profiles.insert(eMesgNumWorkoutStep, f);
}

void initSchedule(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("schedule", eMesgNumSchedule);
    f->addField("manufacturer", fitEnumType, eScheduleManufacturer, 0, 0, "");
    f->addField("product", fitUint16Type, eScheduleProduct, 0, 0, "");
    f->addSubfield("garmin_product", fitEnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fitEnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fitEnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", fitUint32zType, eScheduleSerialNumber, 0, 0, "");
    f->addField("time_created", fitEnumType, eScheduleTimeCreated, 0, 0, "");
    f->addField("completed", fitEnumType, eScheduleCompleted, 0, 0, "");
    f->addField("type", fitEnumType, eScheduleType, 0, 0, "");
    f->addField("scheduled_time", fitEnumType, eScheduleScheduledTime, 0, 0, "");
    profiles.insert(eMesgNumSchedule, f);
}

void initTotals(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("totals", eMesgNumTotals);
    f->addField("message_index", fitEnumType, eTotalsMessageIndex, 0, 0, "");
    f->addField("timestamp", fitEnumType, eTotalsTimestamp, 0, 0, "s");
    f->addField("timer_time", fitUint32Type, eTotalsTimerTime, 0, 0, "s");
    f->addField("distance", fitUint32Type, eTotalsDistance, 0, 0, "m");
    f->addField("calories", fitUint32Type, eTotalsCalories, 0, 0, "kcal");
    f->addField("sport", fitEnumType, eTotalsSport, 0, 0, "");
    f->addField("elapsed_time", fitUint32Type, eTotalsElapsedTime, 0, 0, "s");
    f->addField("sessions", fitUint16Type, eTotalsSessions, 0, 0, "");
    f->addField("active_time", fitUint32Type, eTotalsActiveTime, 0, 0, "s");
    f->addField("sport_index", fitUint8Type, eTotalsSportIndex, 0, 0, "");
    profiles.insert(eMesgNumTotals, f);
}

void initWeightScale(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("weight_scale", eMesgNumWeightScale);
    f->addField("timestamp", fitEnumType, eWeightScaleTimestamp, 0, 0, "s");
    f->addField("weight", fitEnumType, eWeightScaleWeight, 100, 0, "kg");
    f->addField("percent_fat", fitUint16Type, eWeightScalePercentFat, 100, 0, "%");
    f->addField("percent_hydration", fitUint16Type, eWeightScalePercentHydration, 100, 0, "%");
    f->addField("visceral_fat_mass", fitUint16Type, eWeightScaleVisceralFatMass, 100, 0, "kg");
    f->addField("bone_mass", fitUint16Type, eWeightScaleBoneMass, 100, 0, "kg");
    f->addField("muscle_mass", fitUint16Type, eWeightScaleMuscleMass, 100, 0, "kg");
    f->addField("basal_met", fitUint16Type, eWeightScaleBasalMet, 4, 0, "kcal/day");
    f->addField("physique_rating", fitUint8Type, eWeightScalePhysiqueRating, 0, 0, "");
    f->addField("active_met", fitUint16Type, eWeightScaleActiveMet, 4, 0, "kcal/day");
    f->addField("metabolic_age", fitUint8Type, eWeightScaleMetabolicAge, 0, 0, "years");
    f->addField("visceral_fat_rating", fitUint8Type, eWeightScaleVisceralFatRating, 0, 0, "");
    f->addField("user_profile_index", fitEnumType, eWeightScaleUserProfileIndex, 0, 0, "");
    profiles.insert(eMesgNumWeightScale, f);
}

void initBloodPressure(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("blood_pressure", eMesgNumBloodPressure);
    f->addField("timestamp", fitEnumType, eBloodPressureTimestamp, 0, 0, "s");
    f->addField("systolic_pressure", fitUint16Type, eBloodPressureSystolicPressure, 0, 0, "mmHg");
    f->addField("diastolic_pressure", fitUint16Type, eBloodPressureDiastolicPressure, 0, 0, "mmHg");
    f->addField("mean_arterial_pressure", fitUint16Type, eBloodPressureMeanArterialPressure, 0, 0, "mmHg");
    f->addField("map_3_sample_mean", fitUint16Type, eBloodPressureMap3SampleMean, 0, 0, "mmHg");
    f->addField("map_morning_values", fitUint16Type, eBloodPressureMapMorningValues, 0, 0, "mmHg");
    f->addField("map_evening_values", fitUint16Type, eBloodPressureMapEveningValues, 0, 0, "mmHg");
    f->addField("heart_rate", fitUint8Type, eBloodPressureHeartRate, 0, 0, "bpm");
    f->addField("heart_rate_type", fitEnumType, eBloodPressureHeartRateType, 0, 0, "");
    f->addField("status", fitEnumType, eBloodPressureStatus, 0, 0, "");
    f->addField("user_profile_index", fitEnumType, eBloodPressureUserProfileIndex, 0, 0, "");
    profiles.insert(eMesgNumBloodPressure, f);
}

void initMonitoringInfo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("monitoring_info", eMesgNumMonitoringInfo);
    f->addField("timestamp", fitEnumType, eMonitoringInfoTimestamp, 0, 0, "s");
    f->addField("local_timestamp", fitEnumType, eMonitoringInfoLocalTimestamp, 0, 0, "s");
    f->addField("activity_type", fitEnumType, eMonitoringInfoActivityType, 0, 0, "");
    f->addField("cycles_to_distance", fitUint16Type, eMonitoringInfoCyclesToDistance, 5000, 0, "m/cycle");
    f->addField("cycles_to_calories", fitUint16Type, eMonitoringInfoCyclesToCalories, 5000, 0, "kcal/cycle");
    f->addField("resting_metabolic_rate", fitUint16Type, eMonitoringInfoRestingMetabolicRate, 0, 0, "kcal / day");
    profiles.insert(eMesgNumMonitoringInfo, f);
}

void initMonitoring(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("monitoring", eMesgNumMonitoring);
    f->addField("timestamp", fitEnumType, eMonitoringTimestamp, 0, 0, "s");
    f->addField("device_index", fitEnumType, eMonitoringDeviceIndex, 0, 0, "");
    f->addField("calories", fitUint16Type, eMonitoringCalories, 0, 0, "kcal");
    f->addField("distance", fitUint32Type, eMonitoringDistance, 100, 0, "m");
    f->addField("cycles", fitUint32Type, eMonitoringCycles, 2, 0, "cycles");
    f->addSubfield("steps", fitUint32Type, eMonitoringCycles, 1, 0, "steps", eMonitoringActivityType, eActivityTypeWalking);
    f->addSubfield("steps", fitUint32Type, eMonitoringCycles, 0, 0, "", eMonitoringActivityType, eActivityTypeRunning);
    f->addSubfield("strokes", fitUint32Type, eMonitoringCycles, 2, 0, "strokes", eMonitoringActivityType, eActivityTypeCycling);
    f->addSubfield("strokes", fitUint32Type, eMonitoringCycles, 0, 0, "", eMonitoringActivityType, eActivityTypeSwimming);
    f->addField("active_time", fitUint32Type, eMonitoringActiveTime, 1000, 0, "s");
    f->addField("activity_type", fitEnumType, eMonitoringActivityType, 0, 0, "");
    f->addField("activity_subtype", fitEnumType, eMonitoringActivitySubtype, 0, 0, "");
    f->addField("activity_level", fitEnumType, eMonitoringActivityLevel, 0, 0, "");
    f->addField("distance_16", fitUint16Type, eMonitoringDistance16, 0, 0, "100 * m");
    f->addField("cycles_16", fitUint16Type, eMonitoringCycles16, 0, 0, "2 * cycles (steps)");
    f->addField("active_time_16", fitUint16Type, eMonitoringActiveTime16, 0, 0, "s");
    f->addField("local_timestamp", fitEnumType, eMonitoringLocalTimestamp, 0, 0, "");
    f->addField("temperature", fitSint16Type, eMonitoringTemperature, 100, 0, "C");
    f->addField("temperature_min", fitSint16Type, eMonitoringTemperatureMin, 100, 0, "C");
    f->addField("temperature_max", fitSint16Type, eMonitoringTemperatureMax, 100, 0, "C");
    f->addField("activity_time", fitUint16Type, eMonitoringActivityTime, 0, 0, "minutes");
    f->addField("active_calories", fitUint16Type, eMonitoringActiveCalories, 0, 0, "kcal");
    f->addField("current_activity_type_intensity", fitByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "");
    f->addComponent("current_activity_type_intensity", fitByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "", eMonitoringActivityType, 5);
    f->addComponent("current_activity_type_intensity", fitByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "", eMonitoringIntensity, 3);
    f->addField("timestamp_min_8", fitUint8Type, eMonitoringTimestampMin8, 0, 0, "min");
    f->addField("timestamp_16", fitUint16Type, eMonitoringTimestamp16, 0, 0, "s");
    f->addField("heart_rate", fitUint8Type, eMonitoringHeartRate, 0, 0, "bpm");
    f->addField("intensity", fitUint8Type, eMonitoringIntensity, 10, 0, "");
    f->addField("duration_min", fitUint16Type, eMonitoringDurationMin, 0, 0, "min");
    f->addField("duration", fitUint32Type, eMonitoringDuration, 0, 0, "s");
    profiles.insert(eMesgNumMonitoring, f);
}

void initMemoGlob(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("memo_glob", eMesgNumMemoGlob);
    f->addField("part_index", fitUint32Type, eMemoGlobPartIndex, 0, 0, "");
    f->addField("memo", fitByteType, eMemoGlobMemo, 0, 0, "");
    f->addField("message_number", fitUint16Type, eMemoGlobMessageNumber, 0, 0, "");
    f->addField("message_index", fitEnumType, eMemoGlobMessageIndex, 0, 0, "");
    profiles.insert(eMesgNumMemoGlob, f);
}

// ----------- end generated code -----------


void initProfiles(QMap<quint16, CFitProfile*>& allProfiles)
{
    initFileId(allProfiles);
    initFileCreator(allProfiles);
    initTimestampCorrelation(allProfiles);
    initSoftware(allProfiles);
    initSlaveDevice(allProfiles);
    initCapabilities(allProfiles);
    initFileCapabilities(allProfiles);
    initMesgCapabilities(allProfiles);
    initFieldCapabilities(allProfiles);
    initDeviceSettings(allProfiles);
    initUserProfile(allProfiles);
    initHrmProfile(allProfiles);
    initSdmProfile(allProfiles);
    initBikeProfile(allProfiles);
    initZonesTarget(allProfiles);
    initSport(allProfiles);
    initHrZone(allProfiles);
    initSpeedZone(allProfiles);
    initCadenceZone(allProfiles);
    initPowerZone(allProfiles);
    initMetZone(allProfiles);
    initGoal(allProfiles);
    initActivity(allProfiles);
    initSession(allProfiles);
    initLap(allProfiles);
    initLength(allProfiles);
    initRecord(allProfiles);
    initEvent(allProfiles);
    initDeviceInfo(allProfiles);
    initTrainingFile(allProfiles);
    initHrv(allProfiles);
    initCameraEvent(allProfiles);
    initGyroscopeData(allProfiles);
    initAccelerometerData(allProfiles);
    initThreeDSensorCalibration(allProfiles);
    initVideoFrame(allProfiles);
    initObdiiData(allProfiles);
    initNmeaSentence(allProfiles);
    initAviationAttitude(allProfiles);
    initVideo(allProfiles);
    initVideoTitle(allProfiles);
    initVideoDescription(allProfiles);
    initVideoClip(allProfiles);
    initCourse(allProfiles);
    initCoursePoint(allProfiles);
    initSegmentId(allProfiles);
    initSegmentLeaderboardEntry(allProfiles);
    initSegmentPoint(allProfiles);
    initSegmentLap(allProfiles);
    initSegmentFile(allProfiles);
    initWorkout(allProfiles);
    initWorkoutStep(allProfiles);
    initSchedule(allProfiles);
    initTotals(allProfiles);
    initWeightScale(allProfiles);
    initBloodPressure(allProfiles);
    initMonitoringInfo(allProfiles);
    initMonitoring(allProfiles);
    initMemoGlob(allProfiles);

    // invalid profile
    allProfiles.insert(fitGlobalMesgNrInvalid, new CFitProfile());
}

CFitProfileLookup * fitLookupInstance = nullptr;

CFitProfileLookup::CFitProfileLookup()
{
    initProfiles(allProfiles);
    connect(qApp, &QApplication::aboutToQuit, this, &CFitProfileLookup::slotCleanup);
}


CFitProfileLookup::~CFitProfileLookup()
{
    qDeleteAll(allProfiles);
}

void CFitProfileLookup::slotCleanup()
{
    fitLookupInstance = nullptr;
    delete this;
}

const CFitProfile*CFitProfileLookup::getProfile(quint16 globalMesgNr)
{
    if(fitLookupInstance == nullptr)
    {
        fitLookupInstance = new CFitProfileLookup();
    }

    if (fitLookupInstance->allProfiles.contains(globalMesgNr))
    {
        return fitLookupInstance->allProfiles[globalMesgNr];
    }
    return fitLookupInstance->allProfiles[fitGlobalMesgNrInvalid];
}

const CFitFieldProfile*CFitProfileLookup::getFieldForProfile(quint16 globalMesgNr, quint8 fieldDefNr)
{
    if(fitLookupInstance == nullptr)
    {
        fitLookupInstance = new CFitProfileLookup();
    }

    if (fitLookupInstance->allProfiles.contains(globalMesgNr))
    {
        return fitLookupInstance->allProfiles[globalMesgNr]->getField(fieldDefNr);
    }
    return fitLookupInstance->allProfiles[fitGlobalMesgNrInvalid]->getField(fitFieldDefNrInvalid);
}