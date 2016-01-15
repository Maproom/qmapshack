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
    f->addField("type", EnumType, eFileIdType, 0, 0, "");
    f->addField("manufacturer", EnumType, eFileIdManufacturer, 0, 0, "");
    f->addField("product", Uint16Type, eFileIdProduct, 0, 0, "");
    f->addSubfield("garmin_product", EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", Uint32zType, eFileIdSerialNumber, 0, 0, "");
    f->addField("time_created", EnumType, eFileIdTimeCreated, 0, 0, "");
    f->addField("number", Uint16Type, eFileIdNumber, 0, 0, "");
    f->addField("product_name", StringType, eFileIdProductName, 0, 0, "");
    profiles.insert(eMesgNumFileId, f);
}

void initFileCreator(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("file_creator", eMesgNumFileCreator);
    f->addField("software_version", Uint16Type, eFileCreatorSoftwareVersion, 0, 0, "");
    f->addField("hardware_version", Uint8Type, eFileCreatorHardwareVersion, 0, 0, "");
    profiles.insert(eMesgNumFileCreator, f);
}

void initTimestampCorrelation(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("timestamp_correlation", eMesgNumTimestampCorrelation);
    f->addField("timestamp", EnumType, eTimestampCorrelationTimestamp, 0, 0, "s");
    f->addField("fractional_timestamp", Uint16Type, eTimestampCorrelationFractionalTimestamp, 32768, 0, "s");
    f->addField("system_timestamp", EnumType, eTimestampCorrelationSystemTimestamp, 0, 0, "s");
    f->addField("fractional_system_timestamp", Uint16Type, eTimestampCorrelationFractionalSystemTimestamp, 32768, 0, "s");
    f->addField("local_timestamp", EnumType, eTimestampCorrelationLocalTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", Uint16Type, eTimestampCorrelationTimestampMs, 0, 0, "ms");
    f->addField("system_timestamp_ms", Uint16Type, eTimestampCorrelationSystemTimestampMs, 0, 0, "ms");
    profiles.insert(eMesgNumTimestampCorrelation, f);
}

void initSoftware(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("software", eMesgNumSoftware);
    f->addField("message_index", EnumType, eSoftwareMessageIndex, 0, 0, "");
    f->addField("version", Uint16Type, eSoftwareVersion, 100, 0, "");
    f->addField("part_number", StringType, eSoftwarePartNumber, 0, 0, "");
    profiles.insert(eMesgNumSoftware, f);
}

void initSlaveDevice(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("slave_device", eMesgNumSlaveDevice);
    f->addField("manufacturer", EnumType, eSlaveDeviceManufacturer, 0, 0, "");
    f->addField("product", Uint16Type, eSlaveDeviceProduct, 0, 0, "");
    f->addSubfield("garmin_product", EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerDynastreamOem);
    profiles.insert(eMesgNumSlaveDevice, f);
}

void initCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("capabilities", eMesgNumCapabilities);
    f->addField("languages", Uint8zType, eCapabilitiesLanguages, 0, 0, "");
    f->addField("sports", EnumType, eCapabilitiesSports, 0, 0, "");
    f->addField("workouts_supported", EnumType, eCapabilitiesWorkoutsSupported, 0, 0, "");
    f->addField("connectivity_supported", EnumType, eCapabilitiesConnectivitySupported, 0, 0, "");
    profiles.insert(eMesgNumCapabilities, f);
}

void initFileCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("file_capabilities", eMesgNumFileCapabilities);
    f->addField("message_index", EnumType, eFileCapabilitiesMessageIndex, 0, 0, "");
    f->addField("type", EnumType, eFileCapabilitiesType, 0, 0, "");
    f->addField("flags", EnumType, eFileCapabilitiesFlags, 0, 0, "");
    f->addField("directory", StringType, eFileCapabilitiesDirectory, 0, 0, "");
    f->addField("max_count", Uint16Type, eFileCapabilitiesMaxCount, 0, 0, "");
    f->addField("max_size", Uint32Type, eFileCapabilitiesMaxSize, 0, 0, "bytes");
    profiles.insert(eMesgNumFileCapabilities, f);
}

void initMesgCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("mesg_capabilities", eMesgNumMesgCapabilities);
    f->addField("message_index", EnumType, eMesgCapabilitiesMessageIndex, 0, 0, "");
    f->addField("file", EnumType, eMesgCapabilitiesFile, 0, 0, "");
    f->addField("mesg_num", EnumType, eMesgCapabilitiesMesgNum, 0, 0, "");
    f->addField("count_type", EnumType, eMesgCapabilitiesCountType, 0, 0, "");
    f->addField("count", Uint16Type, eMesgCapabilitiesCount, 0, 0, "");
    f->addSubfield("num_per_file", Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountNumPerFile);
    f->addSubfield("max_per_file", Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountMaxPerFile);
    f->addSubfield("max_per_file_type", Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountMaxPerFileType);
    profiles.insert(eMesgNumMesgCapabilities, f);
}

void initFieldCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("field_capabilities", eMesgNumFieldCapabilities);
    f->addField("message_index", EnumType, eFieldCapabilitiesMessageIndex, 0, 0, "");
    f->addField("file", EnumType, eFieldCapabilitiesFile, 0, 0, "");
    f->addField("mesg_num", EnumType, eFieldCapabilitiesMesgNum, 0, 0, "");
    f->addField("field_num", Uint8Type, eFieldCapabilitiesFieldNum, 0, 0, "");
    f->addField("count", Uint16Type, eFieldCapabilitiesCount, 0, 0, "");
    profiles.insert(eMesgNumFieldCapabilities, f);
}

void initDeviceSettings(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("device_settings", eMesgNumDeviceSettings);
    f->addField("active_time_zone", Uint8Type, eDeviceSettingsActiveTimeZone, 0, 0, "");
    f->addField("utc_offset", Uint32Type, eDeviceSettingsUtcOffset, 0, 0, "");
    f->addField("time_zone_offset", Sint8Type, eDeviceSettingsTimeZoneOffset, 4, 0, "hr");
    profiles.insert(eMesgNumDeviceSettings, f);
}

void initUserProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("user_profile", eMesgNumUserProfile);
    f->addField("message_index", EnumType, eUserProfileMessageIndex, 0, 0, "");
    f->addField("friendly_name", StringType, eUserProfileFriendlyName, 0, 0, "");
    f->addField("gender", EnumType, eUserProfileGender, 0, 0, "");
    f->addField("age", Uint8Type, eUserProfileAge, 0, 0, "years");
    f->addField("height", Uint8Type, eUserProfileHeight, 100, 0, "m");
    f->addField("weight", Uint16Type, eUserProfileWeight, 10, 0, "kg");
    f->addField("language", EnumType, eUserProfileLanguage, 0, 0, "");
    f->addField("elev_setting", EnumType, eUserProfileElevSetting, 0, 0, "");
    f->addField("weight_setting", EnumType, eUserProfileWeightSetting, 0, 0, "");
    f->addField("resting_heart_rate", Uint8Type, eUserProfileRestingHeartRate, 0, 0, "bpm");
    f->addField("default_max_running_heart_rate", Uint8Type, eUserProfileDefaultMaxRunningHeartRate, 0, 0, "bpm");
    f->addField("default_max_biking_heart_rate", Uint8Type, eUserProfileDefaultMaxBikingHeartRate, 0, 0, "bpm");
    f->addField("default_max_heart_rate", Uint8Type, eUserProfileDefaultMaxHeartRate, 0, 0, "bpm");
    f->addField("hr_setting", EnumType, eUserProfileHrSetting, 0, 0, "");
    f->addField("speed_setting", EnumType, eUserProfileSpeedSetting, 0, 0, "");
    f->addField("dist_setting", EnumType, eUserProfileDistSetting, 0, 0, "");
    f->addField("power_setting", EnumType, eUserProfilePowerSetting, 0, 0, "");
    f->addField("activity_class", EnumType, eUserProfileActivityClass, 0, 0, "");
    f->addField("position_setting", EnumType, eUserProfilePositionSetting, 0, 0, "");
    f->addField("temperature_setting", EnumType, eUserProfileTemperatureSetting, 0, 0, "");
    f->addField("local_id", EnumType, eUserProfileLocalId, 0, 0, "");
    f->addField("global_id", ByteType, eUserProfileGlobalId, 0, 0, "");
    f->addField("height_setting", EnumType, eUserProfileHeightSetting, 0, 0, "");
    profiles.insert(eMesgNumUserProfile, f);
}

void initHrmProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hrm_profile", eMesgNumHrmProfile);
    f->addField("message_index", EnumType, eHrmProfileMessageIndex, 0, 0, "");
    f->addField("enabled", EnumType, eHrmProfileEnabled, 0, 0, "");
    f->addField("hrm_ant_id", Uint16zType, eHrmProfileHrmAntId, 0, 0, "");
    f->addField("log_hrv", EnumType, eHrmProfileLogHrv, 0, 0, "");
    f->addField("hrm_ant_id_trans_type", Uint8zType, eHrmProfileHrmAntIdTransType, 0, 0, "");
    profiles.insert(eMesgNumHrmProfile, f);
}

void initSdmProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("sdm_profile", eMesgNumSdmProfile);
    f->addField("message_index", EnumType, eSdmProfileMessageIndex, 0, 0, "");
    f->addField("enabled", EnumType, eSdmProfileEnabled, 0, 0, "");
    f->addField("sdm_ant_id", Uint16zType, eSdmProfileSdmAntId, 0, 0, "");
    f->addField("sdm_cal_factor", Uint16Type, eSdmProfileSdmCalFactor, 10, 0, "%");
    f->addField("odometer", Uint32Type, eSdmProfileOdometer, 100, 0, "m");
    f->addField("speed_source", EnumType, eSdmProfileSpeedSource, 0, 0, "");
    f->addField("sdm_ant_id_trans_type", Uint8zType, eSdmProfileSdmAntIdTransType, 0, 0, "");
    f->addField("odometer_rollover", Uint8Type, eSdmProfileOdometerRollover, 0, 0, "");
    profiles.insert(eMesgNumSdmProfile, f);
}

void initBikeProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("bike_profile", eMesgNumBikeProfile);
    f->addField("message_index", EnumType, eBikeProfileMessageIndex, 0, 0, "");
    f->addField("name", StringType, eBikeProfileName, 0, 0, "");
    f->addField("sport", EnumType, eBikeProfileSport, 0, 0, "");
    f->addField("sub_sport", EnumType, eBikeProfileSubSport, 0, 0, "");
    f->addField("odometer", Uint32Type, eBikeProfileOdometer, 100, 0, "m");
    f->addField("bike_spd_ant_id", Uint16zType, eBikeProfileBikeSpdAntId, 0, 0, "");
    f->addField("bike_cad_ant_id", Uint16zType, eBikeProfileBikeCadAntId, 0, 0, "");
    f->addField("bike_spdcad_ant_id", Uint16zType, eBikeProfileBikeSpdcadAntId, 0, 0, "");
    f->addField("bike_power_ant_id", Uint16zType, eBikeProfileBikePowerAntId, 0, 0, "");
    f->addField("custom_wheelsize", Uint16Type, eBikeProfileCustomWheelsize, 1000, 0, "m");
    f->addField("auto_wheelsize", Uint16Type, eBikeProfileAutoWheelsize, 1000, 0, "m");
    f->addField("bike_weight", Uint16Type, eBikeProfileBikeWeight, 10, 0, "kg");
    f->addField("power_cal_factor", Uint16Type, eBikeProfilePowerCalFactor, 10, 0, "%");
    f->addField("auto_wheel_cal", EnumType, eBikeProfileAutoWheelCal, 0, 0, "");
    f->addField("auto_power_zero", EnumType, eBikeProfileAutoPowerZero, 0, 0, "");
    f->addField("id", Uint8Type, eBikeProfileId, 0, 0, "");
    f->addField("spd_enabled", EnumType, eBikeProfileSpdEnabled, 0, 0, "");
    f->addField("cad_enabled", EnumType, eBikeProfileCadEnabled, 0, 0, "");
    f->addField("spdcad_enabled", EnumType, eBikeProfileSpdcadEnabled, 0, 0, "");
    f->addField("power_enabled", EnumType, eBikeProfilePowerEnabled, 0, 0, "");
    f->addField("crank_length", Uint8Type, eBikeProfileCrankLength, 2, -110, "mm");
    f->addField("enabled", EnumType, eBikeProfileEnabled, 0, 0, "");
    f->addField("bike_spd_ant_id_trans_type", Uint8zType, eBikeProfileBikeSpdAntIdTransType, 0, 0, "");
    f->addField("bike_cad_ant_id_trans_type", Uint8zType, eBikeProfileBikeCadAntIdTransType, 0, 0, "");
    f->addField("bike_spdcad_ant_id_trans_type", Uint8zType, eBikeProfileBikeSpdcadAntIdTransType, 0, 0, "");
    f->addField("bike_power_ant_id_trans_type", Uint8zType, eBikeProfileBikePowerAntIdTransType, 0, 0, "");
    f->addField("odometer_rollover", Uint8Type, eBikeProfileOdometerRollover, 0, 0, "");
    f->addField("front_gear_num", Uint8zType, eBikeProfileFrontGearNum, 0, 0, "");
    f->addField("front_gear", Uint8zType, eBikeProfileFrontGear, 0, 0, "");
    f->addField("rear_gear_num", Uint8zType, eBikeProfileRearGearNum, 0, 0, "");
    f->addField("rear_gear", Uint8zType, eBikeProfileRearGear, 0, 0, "");
    f->addField("shimano_di2_enabled", EnumType, eBikeProfileShimanoDi2Enabled, 0, 0, "");
    profiles.insert(eMesgNumBikeProfile, f);
}

void initZonesTarget(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("zones_target", eMesgNumZonesTarget);
    f->addField("max_heart_rate", Uint8Type, eZonesTargetMaxHeartRate, 0, 0, "");
    f->addField("threshold_heart_rate", Uint8Type, eZonesTargetThresholdHeartRate, 0, 0, "");
    f->addField("functional_threshold_power", Uint16Type, eZonesTargetFunctionalThresholdPower, 0, 0, "");
    f->addField("hr_calc_type", EnumType, eZonesTargetHrCalcType, 0, 0, "");
    f->addField("pwr_calc_type", EnumType, eZonesTargetPwrCalcType, 0, 0, "");
    profiles.insert(eMesgNumZonesTarget, f);
}

void initSport(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("sport", eMesgNumSport);
    f->addField("sport", EnumType, eSportSport, 0, 0, "");
    f->addField("sub_sport", EnumType, eSportSubSport, 0, 0, "");
    f->addField("name", StringType, eSportName, 0, 0, "");
    profiles.insert(eMesgNumSport, f);
}

void initHrZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hr_zone", eMesgNumHrZone);
    f->addField("message_index", EnumType, eHrZoneMessageIndex, 0, 0, "");
    f->addField("high_bpm", Uint8Type, eHrZoneHighBpm, 0, 0, "bpm");
    f->addField("name", StringType, eHrZoneName, 0, 0, "");
    profiles.insert(eMesgNumHrZone, f);
}

void initSpeedZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("speed_zone", eMesgNumSpeedZone);
    f->addField("message_index", EnumType, eSpeedZoneMessageIndex, 0, 0, "");
    f->addField("high_value", Uint16Type, eSpeedZoneHighValue, 1000, 0, "m/s");
    f->addField("name", StringType, eSpeedZoneName, 0, 0, "");
    profiles.insert(eMesgNumSpeedZone, f);
}

void initCadenceZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("cadence_zone", eMesgNumCadenceZone);
    f->addField("message_index", EnumType, eCadenceZoneMessageIndex, 0, 0, "");
    f->addField("high_value", Uint8Type, eCadenceZoneHighValue, 0, 0, "rpm");
    f->addField("name", StringType, eCadenceZoneName, 0, 0, "");
    profiles.insert(eMesgNumCadenceZone, f);
}

void initPowerZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("power_zone", eMesgNumPowerZone);
    f->addField("message_index", EnumType, ePowerZoneMessageIndex, 0, 0, "");
    f->addField("high_value", Uint16Type, ePowerZoneHighValue, 0, 0, "watts");
    f->addField("name", StringType, ePowerZoneName, 0, 0, "");
    profiles.insert(eMesgNumPowerZone, f);
}

void initMetZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("met_zone", eMesgNumMetZone);
    f->addField("message_index", EnumType, eMetZoneMessageIndex, 0, 0, "");
    f->addField("high_bpm", Uint8Type, eMetZoneHighBpm, 0, 0, "");
    f->addField("calories", Uint16Type, eMetZoneCalories, 10, 0, "kcal / min");
    f->addField("fat_calories", Uint8Type, eMetZoneFatCalories, 10, 0, "kcal / min");
    profiles.insert(eMesgNumMetZone, f);
}

void initGoal(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("goal", eMesgNumGoal);
    f->addField("message_index", EnumType, eGoalMessageIndex, 0, 0, "");
    f->addField("sport", EnumType, eGoalSport, 0, 0, "");
    f->addField("sub_sport", EnumType, eGoalSubSport, 0, 0, "");
    f->addField("start_date", EnumType, eGoalStartDate, 0, 0, "");
    f->addField("end_date", EnumType, eGoalEndDate, 0, 0, "");
    f->addField("type", EnumType, eGoalType, 0, 0, "");
    f->addField("value", Uint32Type, eGoalValue, 0, 0, "");
    f->addField("repeat", EnumType, eGoalRepeat, 0, 0, "");
    f->addField("target_value", Uint32Type, eGoalTargetValue, 0, 0, "");
    f->addField("recurrence", EnumType, eGoalRecurrence, 0, 0, "");
    f->addField("recurrence_value", Uint16Type, eGoalRecurrenceValue, 0, 0, "");
    f->addField("enabled", EnumType, eGoalEnabled, 0, 0, "");
    profiles.insert(eMesgNumGoal, f);
}

void initActivity(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("activity", eMesgNumActivity);
    f->addField("timestamp", EnumType, eActivityTimestamp, 0, 0, "");
    f->addField("total_timer_time", Uint32Type, eActivityTotalTimerTime, 1000, 0, "s");
    f->addField("num_sessions", Uint16Type, eActivityNumSessions, 0, 0, "");
    f->addField("type", EnumType, eActivityType, 0, 0, "");
    f->addField("event", EnumType, eActivityEvent, 0, 0, "");
    f->addField("event_type", EnumType, eActivityEventType, 0, 0, "");
    f->addField("local_timestamp", EnumType, eActivityLocalTimestamp, 0, 0, "");
    f->addField("event_group", Uint8Type, eActivityEventGroup, 0, 0, "");
    profiles.insert(eMesgNumActivity, f);
}

void initSession(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("session", eMesgNumSession);
    f->addField("message_index", EnumType, eSessionMessageIndex, 0, 0, "");
    f->addField("timestamp", EnumType, eSessionTimestamp, 0, 0, "s");
    f->addField("event", EnumType, eSessionEvent, 0, 0, "");
    f->addField("event_type", EnumType, eSessionEventType, 0, 0, "");
    f->addField("start_time", EnumType, eSessionStartTime, 0, 0, "");
    f->addField("start_position_lat", Sint32Type, eSessionStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", Sint32Type, eSessionStartPositionLong, 0, 0, "semicircles");
    f->addField("sport", EnumType, eSessionSport, 0, 0, "");
    f->addField("sub_sport", EnumType, eSessionSubSport, 0, 0, "");
    f->addField("total_elapsed_time", Uint32Type, eSessionTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", Uint32Type, eSessionTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", Uint32Type, eSessionTotalDistance, 100, 0, "m");
    f->addField("total_cycles", Uint32Type, eSessionTotalCycles, 0, 0, "cycles");
    f->addSubfield("total_strides", Uint32Type, eSessionTotalCycles, 0, 0, "strides", eSessionSport, eSportRunning);
    f->addSubfield("total_strides", Uint32Type, eSessionTotalCycles, 0, 0, "", eSessionSport, eSportWalking);
    f->addField("total_calories", Uint16Type, eSessionTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", Uint16Type, eSessionTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", Uint16Type, eSessionAvgSpeed, 0, 0, "");
    f->addComponent("avg_speed", Uint16Type, eSessionAvgSpeed, 1000, 0, "m/s", eSessionEnhancedAvgSpeed, 16);
    f->addField("max_speed", Uint16Type, eSessionMaxSpeed, 0, 0, "");
    f->addComponent("max_speed", Uint16Type, eSessionMaxSpeed, 1000, 0, "m/s", eSessionEnhancedMaxSpeed, 16);
    f->addField("avg_heart_rate", Uint8Type, eSessionAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", Uint8Type, eSessionMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", Uint8Type, eSessionAvgCadence, 0, 0, "rpm");
    f->addSubfield("avg_running_cadence", Uint8Type, eSessionAvgCadence, 0, 0, "strides/min", eSessionSport, eSportRunning);
    f->addField("max_cadence", Uint8Type, eSessionMaxCadence, 0, 0, "rpm");
    f->addSubfield("max_running_cadence", Uint8Type, eSessionMaxCadence, 0, 0, "strides/min", eSessionSport, eSportRunning);
    f->addField("avg_power", Uint16Type, eSessionAvgPower, 0, 0, "watts");
    f->addField("max_power", Uint16Type, eSessionMaxPower, 0, 0, "watts");
    f->addField("total_ascent", Uint16Type, eSessionTotalAscent, 0, 0, "m");
    f->addField("total_descent", Uint16Type, eSessionTotalDescent, 0, 0, "m");
    f->addField("total_training_effect", Uint8Type, eSessionTotalTrainingEffect, 10, 0, "");
    f->addField("first_lap_index", Uint16Type, eSessionFirstLapIndex, 0, 0, "");
    f->addField("num_laps", Uint16Type, eSessionNumLaps, 0, 0, "");
    f->addField("event_group", Uint8Type, eSessionEventGroup, 0, 0, "");
    f->addField("trigger", EnumType, eSessionTrigger, 0, 0, "");
    f->addField("nec_lat", Sint32Type, eSessionNecLat, 0, 0, "semicircles");
    f->addField("nec_long", Sint32Type, eSessionNecLong, 0, 0, "semicircles");
    f->addField("swc_lat", Sint32Type, eSessionSwcLat, 0, 0, "semicircles");
    f->addField("swc_long", Sint32Type, eSessionSwcLong, 0, 0, "semicircles");
    f->addField("normalized_power", Uint16Type, eSessionNormalizedPower, 0, 0, "watts");
    f->addField("training_stress_score", Uint16Type, eSessionTrainingStressScore, 10, 0, "tss");
    f->addField("intensity_factor", Uint16Type, eSessionIntensityFactor, 1000, 0, "if");
    f->addField("left_right_balance", EnumType, eSessionLeftRightBalance, 0, 0, "");
    f->addField("avg_stroke_count", Uint32Type, eSessionAvgStrokeCount, 10, 0, "strokes/lap");
    f->addField("avg_stroke_distance", Uint16Type, eSessionAvgStrokeDistance, 100, 0, "m");
    f->addField("swim_stroke", EnumType, eSessionSwimStroke, 0, 0, "swim_stroke");
    f->addField("pool_length", Uint16Type, eSessionPoolLength, 100, 0, "m");
    f->addField("threshold_power", Uint16Type, eSessionThresholdPower, 0, 0, "watts");
    f->addField("pool_length_unit", EnumType, eSessionPoolLengthUnit, 0, 0, "");
    f->addField("num_active_lengths", Uint16Type, eSessionNumActiveLengths, 0, 0, "lengths");
    f->addField("total_work", Uint32Type, eSessionTotalWork, 0, 0, "J");
    f->addField("avg_altitude", Uint16Type, eSessionAvgAltitude, 0, 0, "");
    f->addComponent("avg_altitude", Uint16Type, eSessionAvgAltitude, 5, 500, "m", eSessionEnhancedAvgAltitude, 16);
    f->addField("max_altitude", Uint16Type, eSessionMaxAltitude, 0, 0, "");
    f->addComponent("max_altitude", Uint16Type, eSessionMaxAltitude, 5, 500, "m", eSessionEnhancedMaxAltitude, 16);
    f->addField("gps_accuracy", Uint8Type, eSessionGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", Sint16Type, eSessionAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", Sint16Type, eSessionAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", Sint16Type, eSessionAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", Sint16Type, eSessionMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", Sint16Type, eSessionMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", Sint8Type, eSessionAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", Sint8Type, eSessionMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", Uint32Type, eSessionTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", Sint16Type, eSessionAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", Sint16Type, eSessionAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", Sint16Type, eSessionMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", Sint16Type, eSessionMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("min_heart_rate", Uint8Type, eSessionMinHeartRate, 0, 0, "bpm");
    f->addField("time_in_hr_zone", Uint32Type, eSessionTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", Uint32Type, eSessionTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", Uint32Type, eSessionTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", Uint32Type, eSessionTimeInPowerZone, 1000, 0, "s");
    f->addField("avg_lap_time", Uint32Type, eSessionAvgLapTime, 1000, 0, "s");
    f->addField("best_lap_index", Uint16Type, eSessionBestLapIndex, 0, 0, "");
    f->addField("min_altitude", Uint16Type, eSessionMinAltitude, 0, 0, "");
    f->addComponent("min_altitude", Uint16Type, eSessionMinAltitude, 5, 500, "m", eSessionEnhancedMinAltitude, 16);
    f->addField("player_score", Uint16Type, eSessionPlayerScore, 0, 0, "");
    f->addField("opponent_score", Uint16Type, eSessionOpponentScore, 0, 0, "");
    f->addField("opponent_name", StringType, eSessionOpponentName, 0, 0, "");
    f->addField("stroke_count", Uint16Type, eSessionStrokeCount, 0, 0, "counts");
    f->addField("zone_count", Uint16Type, eSessionZoneCount, 0, 0, "counts");
    f->addField("max_ball_speed", Uint16Type, eSessionMaxBallSpeed, 100, 0, "m/s");
    f->addField("avg_ball_speed", Uint16Type, eSessionAvgBallSpeed, 100, 0, "m/s");
    f->addField("avg_vertical_oscillation", Uint16Type, eSessionAvgVerticalOscillation, 10, 0, "mm");
    f->addField("avg_stance_time_percent", Uint16Type, eSessionAvgStanceTimePercent, 100, 0, "percent");
    f->addField("avg_stance_time", Uint16Type, eSessionAvgStanceTime, 10, 0, "ms");
    f->addField("avg_fractional_cadence", Uint8Type, eSessionAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", Uint8Type, eSessionMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", Uint8Type, eSessionTotalFractionalCycles, 128, 0, "cycles");
    f->addField("avg_total_hemoglobin_conc", Uint16Type, eSessionAvgTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("min_total_hemoglobin_conc", Uint16Type, eSessionMinTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("max_total_hemoglobin_conc", Uint16Type, eSessionMaxTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("avg_saturated_hemoglobin_percent", Uint16Type, eSessionAvgSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("min_saturated_hemoglobin_percent", Uint16Type, eSessionMinSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("max_saturated_hemoglobin_percent", Uint16Type, eSessionMaxSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("avg_left_torque_effectiveness", Uint8Type, eSessionAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", Uint8Type, eSessionAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", Uint8Type, eSessionAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", Uint8Type, eSessionAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", Uint8Type, eSessionAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("sport_index", Uint8Type, eSessionSportIndex, 0, 0, "");
    f->addField("time_standing", Uint32Type, eSessionTimeStanding, 1000, 0, "s");
    f->addField("stand_count", Uint16Type, eSessionStandCount, 0, 0, "");
    f->addField("avg_left_pco", Sint8Type, eSessionAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", Sint8Type, eSessionAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", Uint8Type, eSessionAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", Uint8Type, eSessionAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", Uint8Type, eSessionAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", Uint8Type, eSessionAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", Uint16Type, eSessionAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", Uint16Type, eSessionMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", Uint8Type, eSessionAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", Uint8Type, eSessionMaxCadencePosition, 0, 0, "rpm");
    f->addField("enhanced_avg_speed", Uint32Type, eSessionEnhancedAvgSpeed, 1000, 0, "m/s");
    f->addField("enhanced_max_speed", Uint32Type, eSessionEnhancedMaxSpeed, 1000, 0, "m/s");
    f->addField("enhanced_avg_altitude", Uint32Type, eSessionEnhancedAvgAltitude, 5, 500, "m");
    f->addField("enhanced_min_altitude", Uint32Type, eSessionEnhancedMinAltitude, 5, 500, "m");
    f->addField("enhanced_max_altitude", Uint32Type, eSessionEnhancedMaxAltitude, 5, 500, "m");
    f->addField("avg_lev_motor_power", Uint16Type, eSessionAvgLevMotorPower, 0, 0, "watts");
    f->addField("max_lev_motor_power", Uint16Type, eSessionMaxLevMotorPower, 0, 0, "watts");
    f->addField("lev_battery_consumption", Uint8Type, eSessionLevBatteryConsumption, 2, 0, "percent");
    profiles.insert(eMesgNumSession, f);
}

void initLap(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("lap", eMesgNumLap);
    f->addField("message_index", EnumType, eLapMessageIndex, 0, 0, "");
    f->addField("timestamp", EnumType, eLapTimestamp, 0, 0, "s");
    f->addField("event", EnumType, eLapEvent, 0, 0, "");
    f->addField("event_type", EnumType, eLapEventType, 0, 0, "");
    f->addField("start_time", EnumType, eLapStartTime, 0, 0, "");
    f->addField("start_position_lat", Sint32Type, eLapStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", Sint32Type, eLapStartPositionLong, 0, 0, "semicircles");
    f->addField("end_position_lat", Sint32Type, eLapEndPositionLat, 0, 0, "semicircles");
    f->addField("end_position_long", Sint32Type, eLapEndPositionLong, 0, 0, "semicircles");
    f->addField("total_elapsed_time", Uint32Type, eLapTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", Uint32Type, eLapTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", Uint32Type, eLapTotalDistance, 100, 0, "m");
    f->addField("total_cycles", Uint32Type, eLapTotalCycles, 0, 0, "cycles");
    f->addSubfield("total_strides", Uint32Type, eLapTotalCycles, 0, 0, "strides", eLapSport, eSportRunning);
    f->addSubfield("total_strides", Uint32Type, eLapTotalCycles, 0, 0, "", eLapSport, eSportWalking);
    f->addField("total_calories", Uint16Type, eLapTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", Uint16Type, eLapTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", Uint16Type, eLapAvgSpeed, 0, 0, "");
    f->addComponent("avg_speed", Uint16Type, eLapAvgSpeed, 1000, 0, "m/s", eLapEnhancedAvgSpeed, 16);
    f->addField("max_speed", Uint16Type, eLapMaxSpeed, 0, 0, "");
    f->addComponent("max_speed", Uint16Type, eLapMaxSpeed, 1000, 0, "m/s", eLapEnhancedMaxSpeed, 16);
    f->addField("avg_heart_rate", Uint8Type, eLapAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", Uint8Type, eLapMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", Uint8Type, eLapAvgCadence, 0, 0, "rpm");
    f->addSubfield("avg_running_cadence", Uint8Type, eLapAvgCadence, 0, 0, "strides/min", eLapSport, eSportRunning);
    f->addField("max_cadence", Uint8Type, eLapMaxCadence, 0, 0, "rpm");
    f->addSubfield("max_running_cadence", Uint8Type, eLapMaxCadence, 0, 0, "strides/min", eLapSport, eSportRunning);
    f->addField("avg_power", Uint16Type, eLapAvgPower, 0, 0, "watts");
    f->addField("max_power", Uint16Type, eLapMaxPower, 0, 0, "watts");
    f->addField("total_ascent", Uint16Type, eLapTotalAscent, 0, 0, "m");
    f->addField("total_descent", Uint16Type, eLapTotalDescent, 0, 0, "m");
    f->addField("intensity", EnumType, eLapIntensity, 0, 0, "");
    f->addField("lap_trigger", EnumType, eLapLapTrigger, 0, 0, "");
    f->addField("sport", EnumType, eLapSport, 0, 0, "");
    f->addField("event_group", Uint8Type, eLapEventGroup, 0, 0, "");
    f->addField("num_lengths", Uint16Type, eLapNumLengths, 0, 0, "lengths");
    f->addField("normalized_power", Uint16Type, eLapNormalizedPower, 0, 0, "watts");
    f->addField("left_right_balance", EnumType, eLapLeftRightBalance, 0, 0, "");
    f->addField("first_length_index", Uint16Type, eLapFirstLengthIndex, 0, 0, "");
    f->addField("avg_stroke_distance", Uint16Type, eLapAvgStrokeDistance, 100, 0, "m");
    f->addField("swim_stroke", EnumType, eLapSwimStroke, 0, 0, "");
    f->addField("sub_sport", EnumType, eLapSubSport, 0, 0, "");
    f->addField("num_active_lengths", Uint16Type, eLapNumActiveLengths, 0, 0, "lengths");
    f->addField("total_work", Uint32Type, eLapTotalWork, 0, 0, "J");
    f->addField("avg_altitude", Uint16Type, eLapAvgAltitude, 0, 0, "");
    f->addComponent("avg_altitude", Uint16Type, eLapAvgAltitude, 5, 500, "m", eLapEnhancedAvgAltitude, 16);
    f->addField("max_altitude", Uint16Type, eLapMaxAltitude, 0, 0, "");
    f->addComponent("max_altitude", Uint16Type, eLapMaxAltitude, 5, 500, "m", eLapEnhancedMaxAltitude, 16);
    f->addField("gps_accuracy", Uint8Type, eLapGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", Sint16Type, eLapAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", Sint16Type, eLapAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", Sint16Type, eLapAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", Sint16Type, eLapMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", Sint16Type, eLapMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", Sint8Type, eLapAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", Sint8Type, eLapMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", Uint32Type, eLapTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", Sint16Type, eLapAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", Sint16Type, eLapAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", Sint16Type, eLapMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", Sint16Type, eLapMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("time_in_hr_zone", Uint32Type, eLapTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", Uint32Type, eLapTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", Uint32Type, eLapTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", Uint32Type, eLapTimeInPowerZone, 1000, 0, "s");
    f->addField("repetition_num", Uint16Type, eLapRepetitionNum, 0, 0, "");
    f->addField("min_altitude", Uint16Type, eLapMinAltitude, 0, 0, "");
    f->addComponent("min_altitude", Uint16Type, eLapMinAltitude, 5, 500, "m", eLapEnhancedMinAltitude, 16);
    f->addField("min_heart_rate", Uint8Type, eLapMinHeartRate, 0, 0, "bpm");
    f->addField("wkt_step_index", EnumType, eLapWktStepIndex, 0, 0, "");
    f->addField("opponent_score", Uint16Type, eLapOpponentScore, 0, 0, "");
    f->addField("stroke_count", Uint16Type, eLapStrokeCount, 0, 0, "counts");
    f->addField("zone_count", Uint16Type, eLapZoneCount, 0, 0, "counts");
    f->addField("avg_vertical_oscillation", Uint16Type, eLapAvgVerticalOscillation, 10, 0, "mm");
    f->addField("avg_stance_time_percent", Uint16Type, eLapAvgStanceTimePercent, 100, 0, "percent");
    f->addField("avg_stance_time", Uint16Type, eLapAvgStanceTime, 10, 0, "ms");
    f->addField("avg_fractional_cadence", Uint8Type, eLapAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", Uint8Type, eLapMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", Uint8Type, eLapTotalFractionalCycles, 128, 0, "cycles");
    f->addField("player_score", Uint16Type, eLapPlayerScore, 0, 0, "");
    f->addField("avg_total_hemoglobin_conc", Uint16Type, eLapAvgTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("min_total_hemoglobin_conc", Uint16Type, eLapMinTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("max_total_hemoglobin_conc", Uint16Type, eLapMaxTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("avg_saturated_hemoglobin_percent", Uint16Type, eLapAvgSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("min_saturated_hemoglobin_percent", Uint16Type, eLapMinSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("max_saturated_hemoglobin_percent", Uint16Type, eLapMaxSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("avg_left_torque_effectiveness", Uint8Type, eLapAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", Uint8Type, eLapAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", Uint8Type, eLapAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", Uint8Type, eLapAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", Uint8Type, eLapAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("time_standing", Uint32Type, eLapTimeStanding, 1000, 0, "s");
    f->addField("stand_count", Uint16Type, eLapStandCount, 0, 0, "");
    f->addField("avg_left_pco", Sint8Type, eLapAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", Sint8Type, eLapAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", Uint8Type, eLapAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", Uint8Type, eLapAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", Uint8Type, eLapAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", Uint8Type, eLapAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", Uint16Type, eLapAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", Uint16Type, eLapMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", Uint8Type, eLapAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", Uint8Type, eLapMaxCadencePosition, 0, 0, "rpm");
    f->addField("enhanced_avg_speed", Uint32Type, eLapEnhancedAvgSpeed, 1000, 0, "m/s");
    f->addField("enhanced_max_speed", Uint32Type, eLapEnhancedMaxSpeed, 1000, 0, "m/s");
    f->addField("enhanced_avg_altitude", Uint32Type, eLapEnhancedAvgAltitude, 5, 500, "m");
    f->addField("enhanced_min_altitude", Uint32Type, eLapEnhancedMinAltitude, 5, 500, "m");
    f->addField("enhanced_max_altitude", Uint32Type, eLapEnhancedMaxAltitude, 5, 500, "m");
    f->addField("avg_lev_motor_power", Uint16Type, eLapAvgLevMotorPower, 0, 0, "watts");
    f->addField("max_lev_motor_power", Uint16Type, eLapMaxLevMotorPower, 0, 0, "watts");
    f->addField("lev_battery_consumption", Uint8Type, eLapLevBatteryConsumption, 2, 0, "percent");
    profiles.insert(eMesgNumLap, f);
}

void initLength(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("length", eMesgNumLength);
    f->addField("message_index", EnumType, eLengthMessageIndex, 0, 0, "");
    f->addField("timestamp", EnumType, eLengthTimestamp, 0, 0, "");
    f->addField("event", EnumType, eLengthEvent, 0, 0, "");
    f->addField("event_type", EnumType, eLengthEventType, 0, 0, "");
    f->addField("start_time", EnumType, eLengthStartTime, 0, 0, "");
    f->addField("total_elapsed_time", Uint32Type, eLengthTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", Uint32Type, eLengthTotalTimerTime, 1000, 0, "s");
    f->addField("total_strokes", Uint16Type, eLengthTotalStrokes, 0, 0, "strokes");
    f->addField("avg_speed", Uint16Type, eLengthAvgSpeed, 1000, 0, "m/s");
    f->addField("swim_stroke", EnumType, eLengthSwimStroke, 0, 0, "swim_stroke");
    f->addField("avg_swimming_cadence", Uint8Type, eLengthAvgSwimmingCadence, 0, 0, "strokes/min");
    f->addField("event_group", Uint8Type, eLengthEventGroup, 0, 0, "");
    f->addField("total_calories", Uint16Type, eLengthTotalCalories, 0, 0, "kcal");
    f->addField("length_type", EnumType, eLengthLengthType, 0, 0, "");
    f->addField("player_score", Uint16Type, eLengthPlayerScore, 0, 0, "");
    f->addField("opponent_score", Uint16Type, eLengthOpponentScore, 0, 0, "");
    f->addField("stroke_count", Uint16Type, eLengthStrokeCount, 0, 0, "counts");
    f->addField("zone_count", Uint16Type, eLengthZoneCount, 0, 0, "counts");
    profiles.insert(eMesgNumLength, f);
}

void initRecord(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("record", eMesgNumRecord);
    f->addField("timestamp", EnumType, eRecordTimestamp, 0, 0, "s");
    f->addField("position_lat", Sint32Type, eRecordPositionLat, 0, 0, "semicircles");
    f->addField("position_long", Sint32Type, eRecordPositionLong, 0, 0, "semicircles");
    f->addField("altitude", Uint16Type, eRecordAltitude, 0, 0, "");
    f->addComponent("altitude", Uint16Type, eRecordAltitude, 5, 500, "m", eRecordEnhancedAltitude, 16);
    f->addField("heart_rate", Uint8Type, eRecordHeartRate, 0, 0, "bpm");
    f->addField("cadence", Uint8Type, eRecordCadence, 0, 0, "rpm");
    f->addField("distance", Uint32Type, eRecordDistance, 100, 0, "m");
    f->addField("speed", Uint16Type, eRecordSpeed, 0, 0, "");
    f->addComponent("speed", Uint16Type, eRecordSpeed, 1000, 0, "m/s", eRecordEnhancedSpeed, 16);
    f->addField("power", Uint16Type, eRecordPower, 0, 0, "watts");
    f->addField("compressed_speed_distance", ByteType, eRecordCompressedSpeedDistance, 0, 0, "");
    f->addComponent("compressed_speed_distance", ByteType, eRecordCompressedSpeedDistance, 100, 0, "m/s", eRecordSpeed, 12);
    f->addComponent("compressed_speed_distance", ByteType, eRecordCompressedSpeedDistance, 16, 0, "m", eRecordDistance, 12);
    f->addField("grade", Sint16Type, eRecordGrade, 100, 0, "%");
    f->addField("resistance", Uint8Type, eRecordResistance, 0, 0, "");
    f->addField("time_from_course", Sint32Type, eRecordTimeFromCourse, 1000, 0, "s");
    f->addField("cycle_length", Uint8Type, eRecordCycleLength, 100, 0, "m");
    f->addField("temperature", Sint8Type, eRecordTemperature, 0, 0, "C");
    f->addField("speed_1s", Uint8Type, eRecordSpeed1s, 16, 0, "m/s");
    f->addField("cycles", Uint8Type, eRecordCycles, 0, 0, "");
    f->addComponent("cycles", Uint8Type, eRecordCycles, 0, 0, "cycles", eRecordTotalCycles, 8);
    f->addField("total_cycles", Uint32Type, eRecordTotalCycles, 0, 0, "cycles");
    f->addField("compressed_accumulated_power", Uint16Type, eRecordCompressedAccumulatedPower, 0, 0, "");
    f->addComponent("compressed_accumulated_power", Uint16Type, eRecordCompressedAccumulatedPower, 0, 0, "watts", eRecordAccumulatedPower, 16);
    f->addField("accumulated_power", Uint32Type, eRecordAccumulatedPower, 0, 0, "watts");
    f->addField("left_right_balance", EnumType, eRecordLeftRightBalance, 0, 0, "");
    f->addField("gps_accuracy", Uint8Type, eRecordGpsAccuracy, 0, 0, "m");
    f->addField("vertical_speed", Sint16Type, eRecordVerticalSpeed, 1000, 0, "m/s");
    f->addField("calories", Uint16Type, eRecordCalories, 0, 0, "kcal");
    f->addField("vertical_oscillation", Uint16Type, eRecordVerticalOscillation, 10, 0, "mm");
    f->addField("stance_time_percent", Uint16Type, eRecordStanceTimePercent, 100, 0, "percent");
    f->addField("stance_time", Uint16Type, eRecordStanceTime, 10, 0, "ms");
    f->addField("activity_type", EnumType, eRecordActivityType, 0, 0, "");
    f->addField("left_torque_effectiveness", Uint8Type, eRecordLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("right_torque_effectiveness", Uint8Type, eRecordRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("left_pedal_smoothness", Uint8Type, eRecordLeftPedalSmoothness, 2, 0, "percent");
    f->addField("right_pedal_smoothness", Uint8Type, eRecordRightPedalSmoothness, 2, 0, "percent");
    f->addField("combined_pedal_smoothness", Uint8Type, eRecordCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("time128", Uint8Type, eRecordTime128, 128, 0, "s");
    f->addField("stroke_type", EnumType, eRecordStrokeType, 0, 0, "");
    f->addField("zone", Uint8Type, eRecordZone, 0, 0, "");
    f->addField("ball_speed", Uint16Type, eRecordBallSpeed, 100, 0, "m/s");
    f->addField("cadence256", Uint16Type, eRecordCadence256, 256, 0, "rpm");
    f->addField("fractional_cadence", Uint8Type, eRecordFractionalCadence, 128, 0, "rpm");
    f->addField("total_hemoglobin_conc", Uint16Type, eRecordTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("total_hemoglobin_conc_min", Uint16Type, eRecordTotalHemoglobinConcMin, 100, 0, "g/dL");
    f->addField("total_hemoglobin_conc_max", Uint16Type, eRecordTotalHemoglobinConcMax, 100, 0, "g/dL");
    f->addField("saturated_hemoglobin_percent", Uint16Type, eRecordSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("saturated_hemoglobin_percent_min", Uint16Type, eRecordSaturatedHemoglobinPercentMin, 10, 0, "%");
    f->addField("saturated_hemoglobin_percent_max", Uint16Type, eRecordSaturatedHemoglobinPercentMax, 10, 0, "%");
    f->addField("device_index", EnumType, eRecordDeviceIndex, 0, 0, "");
    f->addField("left_pco", Sint8Type, eRecordLeftPco, 0, 0, "mm");
    f->addField("right_pco", Sint8Type, eRecordRightPco, 0, 0, "mm");
    f->addField("left_power_phase", Uint8Type, eRecordLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("left_power_phase_peak", Uint8Type, eRecordLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("right_power_phase", Uint8Type, eRecordRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("right_power_phase_peak", Uint8Type, eRecordRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("enhanced_speed", Uint32Type, eRecordEnhancedSpeed, 1000, 0, "m/s");
    f->addField("enhanced_altitude", Uint32Type, eRecordEnhancedAltitude, 5, 500, "m");
    f->addField("battery_soc", Uint8Type, eRecordBatterySoc, 2, 0, "percent");
    f->addField("motor_power", Uint16Type, eRecordMotorPower, 0, 0, "watts");
    profiles.insert(eMesgNumRecord, f);
}

void initEvent(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("event", eMesgNumEvent);
    f->addField("timestamp", EnumType, eEventTimestamp, 0, 0, "s");
    f->addField("event", EnumType, eEventEvent, 0, 0, "");
    f->addField("event_type", EnumType, eEventEventType, 0, 0, "");
    f->addField("data16", Uint16Type, eEventData16, 0, 0, "");
    f->addComponent("data16", Uint16Type, eEventData16, 0, 0, "", eEventData, 16);
    f->addField("data", Uint32Type, eEventData, 0, 0, "");
    f->addSubfield("timer_trigger", EnumType, eEventData, 0, 0, "", eEventEvent, eEventTimer);
    f->addSubfield("course_point_index", EnumType, eEventData, 0, 0, "", eEventEvent, eEventCoursePoint);
    f->addSubfield("battery_level", Uint16Type, eEventData, 1000, 0, "V", eEventEvent, eEventBattery);
    f->addSubfield("virtual_partner_speed", Uint16Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventVirtualPartnerPace);
    f->addSubfield("hr_high_alert", Uint8Type, eEventData, 0, 0, "bpm", eEventEvent, eEventHrHighAlert);
    f->addSubfield("hr_low_alert", Uint8Type, eEventData, 0, 0, "bpm", eEventEvent, eEventHrLowAlert);
    f->addSubfield("speed_high_alert", Uint32Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventSpeedHighAlert);
    f->addSubfield("speed_low_alert", Uint32Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventSpeedLowAlert);
    f->addSubfield("cad_high_alert", Uint16Type, eEventData, 0, 0, "rpm", eEventEvent, eEventCadHighAlert);
    f->addSubfield("cad_low_alert", Uint16Type, eEventData, 0, 0, "rpm", eEventEvent, eEventCadLowAlert);
    f->addSubfield("power_high_alert", Uint16Type, eEventData, 0, 0, "watts", eEventEvent, eEventPowerHighAlert);
    f->addSubfield("power_low_alert", Uint16Type, eEventData, 0, 0, "watts", eEventEvent, eEventPowerLowAlert);
    f->addSubfield("time_duration_alert", Uint32Type, eEventData, 1000, 0, "s", eEventEvent, eEventTimeDurationAlert);
    f->addSubfield("distance_duration_alert", Uint32Type, eEventData, 100, 0, "m", eEventEvent, eEventDistanceDurationAlert);
    f->addSubfield("calorie_duration_alert", Uint32Type, eEventData, 0, 0, "calories", eEventEvent, eEventCalorieDurationAlert);
    f->addSubfield("fitness_equipment_state", EnumType, eEventData, 0, 0, "", eEventEvent, eEventFitnessEquipment);
    f->addSubfield("sport_point", Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventSportPoint);
    f->addComponent(16, "sport_point", Uint32Type, eEventData, 1, 0, "", eEventScore, 16);
    f->addComponent(16, "sport_point", Uint32Type, eEventData, 1, 0, "", eEventOpponentScore, 16);
    f->addSubfield("gear_change_data", Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventFrontGearChange);
    f->addSubfield("gear_change_data", Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventRearGearChange);
    f->addComponent(18, "gear_change_data", Uint32Type, eEventData, 1, 0, "", eEventRearGearNum, 8);
    f->addComponent(18, "gear_change_data", Uint32Type, eEventData, 1, 0, "", eEventRearGear, 8);
    f->addComponent(18, "gear_change_data", Uint32Type, eEventData, 1, 0, "", eEventFrontGearNum, 8);
    f->addComponent(18, "gear_change_data", Uint32Type, eEventData, 1, 0, "", eEventFrontGear, 8);
    f->addSubfield("rider_position", EnumType, eEventData, 0, 0, "", eEventEvent, eEventRiderPositionChange);
    f->addSubfield("comm_timeout", EnumType, eEventData, 0, 0, "", eEventEvent, eEventCommTimeout);
    f->addField("event_group", Uint8Type, eEventEventGroup, 0, 0, "");
    f->addField("score", Uint16Type, eEventScore, 0, 0, "");
    f->addField("opponent_score", Uint16Type, eEventOpponentScore, 0, 0, "");
    f->addField("front_gear_num", Uint8zType, eEventFrontGearNum, 0, 0, "");
    f->addField("front_gear", Uint8zType, eEventFrontGear, 0, 0, "");
    f->addField("rear_gear_num", Uint8zType, eEventRearGearNum, 0, 0, "");
    f->addField("rear_gear", Uint8zType, eEventRearGear, 0, 0, "");
    f->addField("device_index", EnumType, eEventDeviceIndex, 0, 0, "");
    profiles.insert(eMesgNumEvent, f);
}

void initDeviceInfo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("device_info", eMesgNumDeviceInfo);
    f->addField("timestamp", EnumType, eDeviceInfoTimestamp, 0, 0, "s");
    f->addField("device_index", EnumType, eDeviceInfoDeviceIndex, 0, 0, "");
    f->addField("device_type", Uint8Type, eDeviceInfoDeviceType, 0, 0, "");
    f->addSubfield("antplus_device_type", EnumType, eDeviceInfoDeviceType, 0, 0, "", eDeviceInfoSourceType, eSourceTypeAntplus);
    f->addSubfield("ant_device_type", Uint8Type, eDeviceInfoDeviceType, 0, 0, "", eDeviceInfoSourceType, eSourceTypeAnt);
    f->addField("manufacturer", EnumType, eDeviceInfoManufacturer, 0, 0, "");
    f->addField("serial_number", Uint32zType, eDeviceInfoSerialNumber, 0, 0, "");
    f->addField("product", Uint16Type, eDeviceInfoProduct, 0, 0, "");
    f->addSubfield("garmin_product", EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerDynastreamOem);
    f->addField("software_version", Uint16Type, eDeviceInfoSoftwareVersion, 100, 0, "");
    f->addField("hardware_version", Uint8Type, eDeviceInfoHardwareVersion, 0, 0, "");
    f->addField("cum_operating_time", Uint32Type, eDeviceInfoCumOperatingTime, 0, 0, "s");
    f->addField("battery_voltage", Uint16Type, eDeviceInfoBatteryVoltage, 256, 0, "V");
    f->addField("battery_status", EnumType, eDeviceInfoBatteryStatus, 0, 0, "");
    f->addField("sensor_position", EnumType, eDeviceInfoSensorPosition, 0, 0, "");
    f->addField("descriptor", StringType, eDeviceInfoDescriptor, 0, 0, "");
    f->addField("ant_transmission_type", Uint8zType, eDeviceInfoAntTransmissionType, 0, 0, "");
    f->addField("ant_device_number", Uint16zType, eDeviceInfoAntDeviceNumber, 0, 0, "");
    f->addField("ant_network", EnumType, eDeviceInfoAntNetwork, 0, 0, "");
    f->addField("source_type", EnumType, eDeviceInfoSourceType, 0, 0, "");
    f->addField("product_name", StringType, eDeviceInfoProductName, 0, 0, "");
    profiles.insert(eMesgNumDeviceInfo, f);
}

void initTrainingFile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("training_file", eMesgNumTrainingFile);
    f->addField("timestamp", EnumType, eTrainingFileTimestamp, 0, 0, "");
    f->addField("type", EnumType, eTrainingFileType, 0, 0, "");
    f->addField("manufacturer", EnumType, eTrainingFileManufacturer, 0, 0, "");
    f->addField("product", Uint16Type, eTrainingFileProduct, 0, 0, "");
    f->addSubfield("garmin_product", EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", Uint32zType, eTrainingFileSerialNumber, 0, 0, "");
    f->addField("time_created", EnumType, eTrainingFileTimeCreated, 0, 0, "");
    profiles.insert(eMesgNumTrainingFile, f);
}

void initHrv(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hrv", eMesgNumHrv);
    f->addField("time", Uint16Type, eHrvTime, 1000, 0, "s");
    profiles.insert(eMesgNumHrv, f);
}

void initCameraEvent(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("camera_event", eMesgNumCameraEvent);
    f->addField("timestamp", EnumType, eCameraEventTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", Uint16Type, eCameraEventTimestampMs, 0, 0, "ms");
    f->addField("camera_event_type", EnumType, eCameraEventCameraEventType, 0, 0, "");
    f->addField("camera_file_uuid", StringType, eCameraEventCameraFileUuid, 0, 0, "");
    f->addField("camera_orientation", EnumType, eCameraEventCameraOrientation, 0, 0, "");
    profiles.insert(eMesgNumCameraEvent, f);
}

void initGyroscopeData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("gyroscope_data", eMesgNumGyroscopeData);
    f->addField("timestamp", EnumType, eGyroscopeDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", Uint16Type, eGyroscopeDataTimestampMs, 0, 0, "ms");
    f->addField("sample_time_offset", Uint16Type, eGyroscopeDataSampleTimeOffset, 0, 0, "ms");
    f->addField("gyro_x", Uint16Type, eGyroscopeDataGyroX, 0, 0, "counts");
    f->addField("gyro_y", Uint16Type, eGyroscopeDataGyroY, 0, 0, "counts");
    f->addField("gyro_z", Uint16Type, eGyroscopeDataGyroZ, 0, 0, "counts");
    f->addField("calibrated_gyro_x", Float32Type, eGyroscopeDataCalibratedGyroX, 0, 0, "deg/s");
    f->addField("calibrated_gyro_y", Float32Type, eGyroscopeDataCalibratedGyroY, 0, 0, "deg/s");
    f->addField("calibrated_gyro_z", Float32Type, eGyroscopeDataCalibratedGyroZ, 0, 0, "deg/s");
    profiles.insert(eMesgNumGyroscopeData, f);
}

void initAccelerometerData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("accelerometer_data", eMesgNumAccelerometerData);
    f->addField("timestamp", EnumType, eAccelerometerDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", Uint16Type, eAccelerometerDataTimestampMs, 0, 0, "ms");
    f->addField("sample_time_offset", Uint16Type, eAccelerometerDataSampleTimeOffset, 0, 0, "ms");
    f->addField("accel_x", Uint16Type, eAccelerometerDataAccelX, 0, 0, "counts");
    f->addField("accel_y", Uint16Type, eAccelerometerDataAccelY, 0, 0, "counts");
    f->addField("accel_z", Uint16Type, eAccelerometerDataAccelZ, 0, 0, "counts");
    f->addField("calibrated_accel_x", Float32Type, eAccelerometerDataCalibratedAccelX, 0, 0, "g");
    f->addField("calibrated_accel_y", Float32Type, eAccelerometerDataCalibratedAccelY, 0, 0, "g");
    f->addField("calibrated_accel_z", Float32Type, eAccelerometerDataCalibratedAccelZ, 0, 0, "g");
    profiles.insert(eMesgNumAccelerometerData, f);
}

void initThreeDSensorCalibration(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("three_d_sensor_calibration", eMesgNumThreeDSensorCalibration);
    f->addField("timestamp", EnumType, eThreeDSensorCalibrationTimestamp, 0, 0, "s");
    f->addField("sensor_type", EnumType, eThreeDSensorCalibrationSensorType, 0, 0, "");
    f->addField("calibration_factor", Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "");
    f->addSubfield("accel_cal_factor", Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "g", eThreeDSensorCalibrationSensorType, eSensorTypeAccelerometer);
    f->addSubfield("gyro_cal_factor", Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "deg/s", eThreeDSensorCalibrationSensorType, eSensorTypeGyroscope);
    f->addField("calibration_divisor", Uint32Type, eThreeDSensorCalibrationCalibrationDivisor, 0, 0, "counts");
    f->addField("level_shift", Uint32Type, eThreeDSensorCalibrationLevelShift, 0, 0, "");
    f->addField("offset_cal", Sint32Type, eThreeDSensorCalibrationOffsetCal, 0, 0, "");
    f->addField("orientation_matrix", Sint32Type, eThreeDSensorCalibrationOrientationMatrix, 65535, 0, "");
    profiles.insert(eMesgNumThreeDSensorCalibration, f);
}

void initVideoFrame(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_frame", eMesgNumVideoFrame);
    f->addField("timestamp", EnumType, eVideoFrameTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", Uint16Type, eVideoFrameTimestampMs, 0, 0, "ms");
    f->addField("frame_number", Uint32Type, eVideoFrameFrameNumber, 0, 0, "");
    profiles.insert(eMesgNumVideoFrame, f);
}

void initObdiiData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("obdii_data", eMesgNumObdiiData);
    f->addField("timestamp", EnumType, eObdiiDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", Uint16Type, eObdiiDataTimestampMs, 0, 0, "ms");
    f->addField("time_offset", Uint16Type, eObdiiDataTimeOffset, 0, 0, "ms");
    f->addField("pid", ByteType, eObdiiDataPid, 0, 0, "");
    f->addField("raw_data", ByteType, eObdiiDataRawData, 0, 0, "");
    f->addField("pid_data_size", Uint8Type, eObdiiDataPidDataSize, 0, 0, "");
    f->addField("system_time", Uint32Type, eObdiiDataSystemTime, 0, 0, "");
    f->addField("start_timestamp", EnumType, eObdiiDataStartTimestamp, 0, 0, "");
    f->addField("start_timestamp_ms", Uint16Type, eObdiiDataStartTimestampMs, 0, 0, "ms");
    profiles.insert(eMesgNumObdiiData, f);
}

void initNmeaSentence(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("nmea_sentence", eMesgNumNmeaSentence);
    f->addField("timestamp", EnumType, eNmeaSentenceTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", Uint16Type, eNmeaSentenceTimestampMs, 0, 0, "ms");
    f->addField("sentence", StringType, eNmeaSentenceSentence, 0, 0, "");
    profiles.insert(eMesgNumNmeaSentence, f);
}

void initAviationAttitude(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("aviation_attitude", eMesgNumAviationAttitude);
    f->addField("timestamp", EnumType, eAviationAttitudeTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", Uint16Type, eAviationAttitudeTimestampMs, 0, 0, "ms");
    f->addField("system_time", Uint32Type, eAviationAttitudeSystemTime, 0, 0, "ms");
    f->addField("pitch", Sint16Type, eAviationAttitudePitch, 10430.38, 0, "radians");
    f->addField("roll", Sint16Type, eAviationAttitudeRoll, 10430.38, 0, "radians");
    f->addField("accel_lateral", Sint16Type, eAviationAttitudeAccelLateral, 100, 0, "m/s^2");
    f->addField("accel_normal", Sint16Type, eAviationAttitudeAccelNormal, 100, 0, "m/s^2");
    f->addField("turn_rate", Sint16Type, eAviationAttitudeTurnRate, 1024, 0, "radians/second");
    f->addField("stage", EnumType, eAviationAttitudeStage, 0, 0, "");
    f->addField("attitude_stage_complete", Uint8Type, eAviationAttitudeAttitudeStageComplete, 0, 0, "%");
    f->addField("track", Uint16Type, eAviationAttitudeTrack, 10430.38, 0, "radians");
    f->addField("validity", EnumType, eAviationAttitudeValidity, 0, 0, "");
    profiles.insert(eMesgNumAviationAttitude, f);
}

void initVideo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video", eMesgNumVideo);
    f->addField("url", StringType, eVideoUrl, 0, 0, "");
    f->addField("hosting_provider", StringType, eVideoHostingProvider, 0, 0, "");
    f->addField("duration", Uint32Type, eVideoDuration, 0, 0, "ms");
    profiles.insert(eMesgNumVideo, f);
}

void initVideoTitle(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_title", eMesgNumVideoTitle);
    f->addField("message_index", EnumType, eVideoTitleMessageIndex, 0, 0, "");
    f->addField("message_count", Uint16Type, eVideoTitleMessageCount, 0, 0, "");
    f->addField("text", StringType, eVideoTitleText, 0, 0, "");
    profiles.insert(eMesgNumVideoTitle, f);
}

void initVideoDescription(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_description", eMesgNumVideoDescription);
    f->addField("message_index", EnumType, eVideoDescriptionMessageIndex, 0, 0, "");
    f->addField("message_count", Uint16Type, eVideoDescriptionMessageCount, 0, 0, "");
    f->addField("text", StringType, eVideoDescriptionText, 0, 0, "");
    profiles.insert(eMesgNumVideoDescription, f);
}

void initVideoClip(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_clip", eMesgNumVideoClip);
    f->addField("clip_number", Uint16Type, eVideoClipClipNumber, 0, 0, "");
    f->addField("start_timestamp", EnumType, eVideoClipStartTimestamp, 0, 0, "");
    f->addField("start_timestamp_ms", Uint16Type, eVideoClipStartTimestampMs, 0, 0, "");
    f->addField("end_timestamp", EnumType, eVideoClipEndTimestamp, 0, 0, "");
    f->addField("end_timestamp_ms", Uint16Type, eVideoClipEndTimestampMs, 0, 0, "");
    f->addField("clip_start", Uint32Type, eVideoClipClipStart, 0, 0, "ms");
    f->addField("clip_end", Uint32Type, eVideoClipClipEnd, 0, 0, "ms");
    profiles.insert(eMesgNumVideoClip, f);
}

void initCourse(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("course", eMesgNumCourse);
    f->addField("sport", EnumType, eCourseSport, 0, 0, "");
    f->addField("name", StringType, eCourseName, 0, 0, "");
    f->addField("capabilities", EnumType, eCourseCapabilities, 0, 0, "");
    profiles.insert(eMesgNumCourse, f);
}

void initCoursePoint(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("course_point", eMesgNumCoursePoint);
    f->addField("message_index", EnumType, eCoursePointMessageIndex, 0, 0, "");
    f->addField("timestamp", EnumType, eCoursePointTimestamp, 0, 0, "");
    f->addField("position_lat", Sint32Type, eCoursePointPositionLat, 0, 0, "semicircles");
    f->addField("position_long", Sint32Type, eCoursePointPositionLong, 0, 0, "semicircles");
    f->addField("distance", Uint32Type, eCoursePointDistance, 100, 0, "m");
    f->addField("type", EnumType, eCoursePointType, 0, 0, "");
    f->addField("name", StringType, eCoursePointName, 0, 0, "");
    f->addField("favorite", EnumType, eCoursePointFavorite, 0, 0, "");
    profiles.insert(eMesgNumCoursePoint, f);
}

void initSegmentId(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_id", eMesgNumSegmentId);
    f->addField("name", StringType, eSegmentIdName, 0, 0, "");
    f->addField("uuid", StringType, eSegmentIdUuid, 0, 0, "");
    f->addField("sport", EnumType, eSegmentIdSport, 0, 0, "");
    f->addField("enabled", EnumType, eSegmentIdEnabled, 0, 0, "");
    f->addField("user_profile_primary_key", Uint32Type, eSegmentIdUserProfilePrimaryKey, 0, 0, "");
    f->addField("device_id", Uint32Type, eSegmentIdDeviceId, 0, 0, "");
    f->addField("default_race_leader", Uint8Type, eSegmentIdDefaultRaceLeader, 0, 0, "");
    f->addField("delete_status", EnumType, eSegmentIdDeleteStatus, 0, 0, "");
    f->addField("selection_type", EnumType, eSegmentIdSelectionType, 0, 0, "");
    profiles.insert(eMesgNumSegmentId, f);
}

void initSegmentLeaderboardEntry(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_leaderboard_entry", eMesgNumSegmentLeaderboardEntry);
    f->addField("message_index", EnumType, eSegmentLeaderboardEntryMessageIndex, 0, 0, "");
    f->addField("name", StringType, eSegmentLeaderboardEntryName, 0, 0, "");
    f->addField("type", EnumType, eSegmentLeaderboardEntryType, 0, 0, "");
    f->addField("group_primary_key", Uint32Type, eSegmentLeaderboardEntryGroupPrimaryKey, 0, 0, "");
    f->addField("activity_id", Uint32Type, eSegmentLeaderboardEntryActivityId, 0, 0, "");
    f->addField("segment_time", Uint32Type, eSegmentLeaderboardEntrySegmentTime, 1000, 0, "s");
    f->addField("activity_id_string", StringType, eSegmentLeaderboardEntryActivityIdString, 0, 0, "");
    profiles.insert(eMesgNumSegmentLeaderboardEntry, f);
}

void initSegmentPoint(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_point", eMesgNumSegmentPoint);
    f->addField("message_index", EnumType, eSegmentPointMessageIndex, 0, 0, "");
    f->addField("position_lat", Sint32Type, eSegmentPointPositionLat, 0, 0, "semicircles");
    f->addField("position_long", Sint32Type, eSegmentPointPositionLong, 0, 0, "semicircles");
    f->addField("distance", Uint32Type, eSegmentPointDistance, 100, 0, "m");
    f->addField("altitude", Uint16Type, eSegmentPointAltitude, 5, 500, "m");
    f->addField("leader_time", Uint32Type, eSegmentPointLeaderTime, 1000, 0, "s");
    profiles.insert(eMesgNumSegmentPoint, f);
}

void initSegmentLap(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_lap", eMesgNumSegmentLap);
    f->addField("message_index", EnumType, eSegmentLapMessageIndex, 0, 0, "");
    f->addField("timestamp", EnumType, eSegmentLapTimestamp, 0, 0, "s");
    f->addField("event", EnumType, eSegmentLapEvent, 0, 0, "");
    f->addField("event_type", EnumType, eSegmentLapEventType, 0, 0, "");
    f->addField("start_time", EnumType, eSegmentLapStartTime, 0, 0, "");
    f->addField("start_position_lat", Sint32Type, eSegmentLapStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", Sint32Type, eSegmentLapStartPositionLong, 0, 0, "semicircles");
    f->addField("end_position_lat", Sint32Type, eSegmentLapEndPositionLat, 0, 0, "semicircles");
    f->addField("end_position_long", Sint32Type, eSegmentLapEndPositionLong, 0, 0, "semicircles");
    f->addField("total_elapsed_time", Uint32Type, eSegmentLapTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", Uint32Type, eSegmentLapTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", Uint32Type, eSegmentLapTotalDistance, 100, 0, "m");
    f->addField("total_cycles", Uint32Type, eSegmentLapTotalCycles, 0, 0, "cycles");
    f->addSubfield("total_strokes", Uint32Type, eSegmentLapTotalCycles, 0, 0, "strokes", eSegmentLapSport, eSportCycling);
    f->addField("total_calories", Uint16Type, eSegmentLapTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", Uint16Type, eSegmentLapTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", Uint16Type, eSegmentLapAvgSpeed, 1000, 0, "m/s");
    f->addField("max_speed", Uint16Type, eSegmentLapMaxSpeed, 1000, 0, "m/s");
    f->addField("avg_heart_rate", Uint8Type, eSegmentLapAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", Uint8Type, eSegmentLapMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", Uint8Type, eSegmentLapAvgCadence, 0, 0, "rpm");
    f->addField("max_cadence", Uint8Type, eSegmentLapMaxCadence, 0, 0, "rpm");
    f->addField("avg_power", Uint16Type, eSegmentLapAvgPower, 0, 0, "watts");
    f->addField("max_power", Uint16Type, eSegmentLapMaxPower, 0, 0, "watts");
    f->addField("total_ascent", Uint16Type, eSegmentLapTotalAscent, 0, 0, "m");
    f->addField("total_descent", Uint16Type, eSegmentLapTotalDescent, 0, 0, "m");
    f->addField("sport", EnumType, eSegmentLapSport, 0, 0, "");
    f->addField("event_group", Uint8Type, eSegmentLapEventGroup, 0, 0, "");
    f->addField("nec_lat", Sint32Type, eSegmentLapNecLat, 0, 0, "semicircles");
    f->addField("nec_long", Sint32Type, eSegmentLapNecLong, 0, 0, "semicircles");
    f->addField("swc_lat", Sint32Type, eSegmentLapSwcLat, 0, 0, "semicircles");
    f->addField("swc_long", Sint32Type, eSegmentLapSwcLong, 0, 0, "semicircles");
    f->addField("name", StringType, eSegmentLapName, 0, 0, "");
    f->addField("normalized_power", Uint16Type, eSegmentLapNormalizedPower, 0, 0, "watts");
    f->addField("left_right_balance", EnumType, eSegmentLapLeftRightBalance, 0, 0, "");
    f->addField("sub_sport", EnumType, eSegmentLapSubSport, 0, 0, "");
    f->addField("total_work", Uint32Type, eSegmentLapTotalWork, 0, 0, "J");
    f->addField("avg_altitude", Uint16Type, eSegmentLapAvgAltitude, 5, 500, "m");
    f->addField("max_altitude", Uint16Type, eSegmentLapMaxAltitude, 5, 500, "m");
    f->addField("gps_accuracy", Uint8Type, eSegmentLapGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", Sint16Type, eSegmentLapAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", Sint16Type, eSegmentLapAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", Sint16Type, eSegmentLapAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", Sint16Type, eSegmentLapMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", Sint16Type, eSegmentLapMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", Sint8Type, eSegmentLapAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", Sint8Type, eSegmentLapMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", Uint32Type, eSegmentLapTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", Sint16Type, eSegmentLapAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", Sint16Type, eSegmentLapAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", Sint16Type, eSegmentLapMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", Sint16Type, eSegmentLapMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("time_in_hr_zone", Uint32Type, eSegmentLapTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", Uint32Type, eSegmentLapTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", Uint32Type, eSegmentLapTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", Uint32Type, eSegmentLapTimeInPowerZone, 1000, 0, "s");
    f->addField("repetition_num", Uint16Type, eSegmentLapRepetitionNum, 0, 0, "");
    f->addField("min_altitude", Uint16Type, eSegmentLapMinAltitude, 5, 500, "m");
    f->addField("min_heart_rate", Uint8Type, eSegmentLapMinHeartRate, 0, 0, "bpm");
    f->addField("active_time", Uint32Type, eSegmentLapActiveTime, 1000, 0, "s");
    f->addField("wkt_step_index", EnumType, eSegmentLapWktStepIndex, 0, 0, "");
    f->addField("sport_event", EnumType, eSegmentLapSportEvent, 0, 0, "");
    f->addField("avg_left_torque_effectiveness", Uint8Type, eSegmentLapAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", Uint8Type, eSegmentLapAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", Uint8Type, eSegmentLapAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", Uint8Type, eSegmentLapAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", Uint8Type, eSegmentLapAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("status", EnumType, eSegmentLapStatus, 0, 0, "");
    f->addField("uuid", StringType, eSegmentLapUuid, 0, 0, "");
    f->addField("avg_fractional_cadence", Uint8Type, eSegmentLapAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", Uint8Type, eSegmentLapMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", Uint8Type, eSegmentLapTotalFractionalCycles, 128, 0, "cycles");
    f->addField("front_gear_shift_count", Uint16Type, eSegmentLapFrontGearShiftCount, 0, 0, "");
    f->addField("rear_gear_shift_count", Uint16Type, eSegmentLapRearGearShiftCount, 0, 0, "");
    f->addField("time_standing", Uint32Type, eSegmentLapTimeStanding, 1000, 0, "s");
    f->addField("stand_count", Uint16Type, eSegmentLapStandCount, 0, 0, "");
    f->addField("avg_left_pco", Sint8Type, eSegmentLapAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", Sint8Type, eSegmentLapAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", Uint8Type, eSegmentLapAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", Uint8Type, eSegmentLapAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", Uint8Type, eSegmentLapAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", Uint8Type, eSegmentLapAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", Uint16Type, eSegmentLapAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", Uint16Type, eSegmentLapMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", Uint8Type, eSegmentLapAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", Uint8Type, eSegmentLapMaxCadencePosition, 0, 0, "rpm");
    profiles.insert(eMesgNumSegmentLap, f);
}

void initSegmentFile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_file", eMesgNumSegmentFile);
    f->addField("message_index", EnumType, eSegmentFileMessageIndex, 0, 0, "");
    f->addField("file_uuid", StringType, eSegmentFileFileUuid, 0, 0, "");
    f->addField("enabled", EnumType, eSegmentFileEnabled, 0, 0, "");
    f->addField("user_profile_primary_key", Uint32Type, eSegmentFileUserProfilePrimaryKey, 0, 0, "");
    f->addField("leader_type", EnumType, eSegmentFileLeaderType, 0, 0, "");
    f->addField("leader_group_primary_key", Uint32Type, eSegmentFileLeaderGroupPrimaryKey, 0, 0, "");
    f->addField("leader_activity_id", Uint32Type, eSegmentFileLeaderActivityId, 0, 0, "");
    f->addField("leader_activity_id_string", StringType, eSegmentFileLeaderActivityIdString, 0, 0, "");
    profiles.insert(eMesgNumSegmentFile, f);
}

void initWorkout(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("workout", eMesgNumWorkout);
    f->addField("sport", EnumType, eWorkoutSport, 0, 0, "");
    f->addField("capabilities", EnumType, eWorkoutCapabilities, 0, 0, "");
    f->addField("num_valid_steps", Uint16Type, eWorkoutNumValidSteps, 0, 0, "");
    f->addField("wkt_name", StringType, eWorkoutWktName, 0, 0, "");
    profiles.insert(eMesgNumWorkout, f);
}

void initWorkoutStep(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("workout_step", eMesgNumWorkoutStep);
    f->addField("message_index", EnumType, eWorkoutStepMessageIndex, 0, 0, "");
    f->addField("wkt_step_name", StringType, eWorkoutStepWktStepName, 0, 0, "");
    f->addField("duration_type", EnumType, eWorkoutStepDurationType, 0, 0, "");
    f->addField("duration_value", Uint32Type, eWorkoutStepDurationValue, 0, 0, "");
    f->addSubfield("duration_time", Uint32Type, eWorkoutStepDurationValue, 1000, 0, "s", eWorkoutStepDurationType, eWktStepDurationTime);
    f->addSubfield("duration_time", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepetitionTime);
    f->addSubfield("duration_distance", Uint32Type, eWorkoutStepDurationValue, 100, 0, "m", eWorkoutStepDurationType, eWktStepDurationDistance);
    f->addSubfield("duration_hr", EnumType, eWorkoutStepDurationValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationHrLessThan);
    f->addSubfield("duration_hr", EnumType, eWorkoutStepDurationValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationHrGreaterThan);
    f->addSubfield("duration_calories", Uint32Type, eWorkoutStepDurationValue, 0, 0, "calories", eWorkoutStepDurationType, eWktStepDurationCalories);
    f->addSubfield("duration_step", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilStepsCmplt);
    f->addSubfield("duration_step", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilTime);
    f->addSubfield("duration_step", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilDistance);
    f->addSubfield("duration_step", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilCalories);
    f->addSubfield("duration_step", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrLessThan);
    f->addSubfield("duration_step", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrGreaterThan);
    f->addSubfield("duration_step", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerLessThan);
    f->addSubfield("duration_step", Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerGreaterThan);
    f->addSubfield("duration_power", EnumType, eWorkoutStepDurationValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationPowerLessThan);
    f->addSubfield("duration_power", EnumType, eWorkoutStepDurationValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationPowerGreaterThan);
    f->addField("target_type", EnumType, eWorkoutStepTargetType, 0, 0, "");
    f->addField("target_value", Uint32Type, eWorkoutStepTargetValue, 0, 0, "");
    f->addSubfield("target_hr_zone", Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("target_power_zone", Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addSubfield("repeat_steps", Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilStepsCmplt);
    f->addSubfield("repeat_time", Uint32Type, eWorkoutStepTargetValue, 1000, 0, "s", eWorkoutStepDurationType, eWktStepDurationRepeatUntilTime);
    f->addSubfield("repeat_distance", Uint32Type, eWorkoutStepTargetValue, 100, 0, "m", eWorkoutStepDurationType, eWktStepDurationRepeatUntilDistance);
    f->addSubfield("repeat_calories", Uint32Type, eWorkoutStepTargetValue, 0, 0, "calories", eWorkoutStepDurationType, eWktStepDurationRepeatUntilCalories);
    f->addSubfield("repeat_hr", EnumType, eWorkoutStepTargetValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrLessThan);
    f->addSubfield("repeat_hr", EnumType, eWorkoutStepTargetValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrGreaterThan);
    f->addSubfield("repeat_power", EnumType, eWorkoutStepTargetValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerLessThan);
    f->addSubfield("repeat_power", EnumType, eWorkoutStepTargetValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerGreaterThan);
    f->addField("custom_target_value_low", Uint32Type, eWorkoutStepCustomTargetValueLow, 0, 0, "");
    f->addSubfield("custom_target_speed_low", Uint32Type, eWorkoutStepCustomTargetValueLow, 1000, 0, "m/s", eWorkoutStepTargetType, eWktStepTargetSpeed);
    f->addSubfield("custom_target_heart_rate_low", EnumType, eWorkoutStepCustomTargetValueLow, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("custom_target_cadence_low", Uint32Type, eWorkoutStepCustomTargetValueLow, 0, 0, "rpm", eWorkoutStepTargetType, eWktStepTargetCadence);
    f->addSubfield("custom_target_power_low", EnumType, eWorkoutStepCustomTargetValueLow, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addField("custom_target_value_high", Uint32Type, eWorkoutStepCustomTargetValueHigh, 0, 0, "");
    f->addSubfield("custom_target_speed_high", Uint32Type, eWorkoutStepCustomTargetValueHigh, 1000, 0, "m/s", eWorkoutStepTargetType, eWktStepTargetSpeed);
    f->addSubfield("custom_target_heart_rate_high", EnumType, eWorkoutStepCustomTargetValueHigh, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("custom_target_cadence_high", Uint32Type, eWorkoutStepCustomTargetValueHigh, 0, 0, "rpm", eWorkoutStepTargetType, eWktStepTargetCadence);
    f->addSubfield("custom_target_power_high", EnumType, eWorkoutStepCustomTargetValueHigh, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addField("intensity", EnumType, eWorkoutStepIntensity, 0, 0, "");
    profiles.insert(eMesgNumWorkoutStep, f);
}

void initSchedule(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("schedule", eMesgNumSchedule);
    f->addField("manufacturer", EnumType, eScheduleManufacturer, 0, 0, "");
    f->addField("product", Uint16Type, eScheduleProduct, 0, 0, "");
    f->addSubfield("garmin_product", EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", Uint32zType, eScheduleSerialNumber, 0, 0, "");
    f->addField("time_created", EnumType, eScheduleTimeCreated, 0, 0, "");
    f->addField("completed", EnumType, eScheduleCompleted, 0, 0, "");
    f->addField("type", EnumType, eScheduleType, 0, 0, "");
    f->addField("scheduled_time", EnumType, eScheduleScheduledTime, 0, 0, "");
    profiles.insert(eMesgNumSchedule, f);
}

void initTotals(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("totals", eMesgNumTotals);
    f->addField("message_index", EnumType, eTotalsMessageIndex, 0, 0, "");
    f->addField("timestamp", EnumType, eTotalsTimestamp, 0, 0, "s");
    f->addField("timer_time", Uint32Type, eTotalsTimerTime, 0, 0, "s");
    f->addField("distance", Uint32Type, eTotalsDistance, 0, 0, "m");
    f->addField("calories", Uint32Type, eTotalsCalories, 0, 0, "kcal");
    f->addField("sport", EnumType, eTotalsSport, 0, 0, "");
    f->addField("elapsed_time", Uint32Type, eTotalsElapsedTime, 0, 0, "s");
    f->addField("sessions", Uint16Type, eTotalsSessions, 0, 0, "");
    f->addField("active_time", Uint32Type, eTotalsActiveTime, 0, 0, "s");
    f->addField("sport_index", Uint8Type, eTotalsSportIndex, 0, 0, "");
    profiles.insert(eMesgNumTotals, f);
}

void initWeightScale(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("weight_scale", eMesgNumWeightScale);
    f->addField("timestamp", EnumType, eWeightScaleTimestamp, 0, 0, "s");
    f->addField("weight", EnumType, eWeightScaleWeight, 100, 0, "kg");
    f->addField("percent_fat", Uint16Type, eWeightScalePercentFat, 100, 0, "%");
    f->addField("percent_hydration", Uint16Type, eWeightScalePercentHydration, 100, 0, "%");
    f->addField("visceral_fat_mass", Uint16Type, eWeightScaleVisceralFatMass, 100, 0, "kg");
    f->addField("bone_mass", Uint16Type, eWeightScaleBoneMass, 100, 0, "kg");
    f->addField("muscle_mass", Uint16Type, eWeightScaleMuscleMass, 100, 0, "kg");
    f->addField("basal_met", Uint16Type, eWeightScaleBasalMet, 4, 0, "kcal/day");
    f->addField("physique_rating", Uint8Type, eWeightScalePhysiqueRating, 0, 0, "");
    f->addField("active_met", Uint16Type, eWeightScaleActiveMet, 4, 0, "kcal/day");
    f->addField("metabolic_age", Uint8Type, eWeightScaleMetabolicAge, 0, 0, "years");
    f->addField("visceral_fat_rating", Uint8Type, eWeightScaleVisceralFatRating, 0, 0, "");
    f->addField("user_profile_index", EnumType, eWeightScaleUserProfileIndex, 0, 0, "");
    profiles.insert(eMesgNumWeightScale, f);
}

void initBloodPressure(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("blood_pressure", eMesgNumBloodPressure);
    f->addField("timestamp", EnumType, eBloodPressureTimestamp, 0, 0, "s");
    f->addField("systolic_pressure", Uint16Type, eBloodPressureSystolicPressure, 0, 0, "mmHg");
    f->addField("diastolic_pressure", Uint16Type, eBloodPressureDiastolicPressure, 0, 0, "mmHg");
    f->addField("mean_arterial_pressure", Uint16Type, eBloodPressureMeanArterialPressure, 0, 0, "mmHg");
    f->addField("map_3_sample_mean", Uint16Type, eBloodPressureMap3SampleMean, 0, 0, "mmHg");
    f->addField("map_morning_values", Uint16Type, eBloodPressureMapMorningValues, 0, 0, "mmHg");
    f->addField("map_evening_values", Uint16Type, eBloodPressureMapEveningValues, 0, 0, "mmHg");
    f->addField("heart_rate", Uint8Type, eBloodPressureHeartRate, 0, 0, "bpm");
    f->addField("heart_rate_type", EnumType, eBloodPressureHeartRateType, 0, 0, "");
    f->addField("status", EnumType, eBloodPressureStatus, 0, 0, "");
    f->addField("user_profile_index", EnumType, eBloodPressureUserProfileIndex, 0, 0, "");
    profiles.insert(eMesgNumBloodPressure, f);
}

void initMonitoringInfo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("monitoring_info", eMesgNumMonitoringInfo);
    f->addField("timestamp", EnumType, eMonitoringInfoTimestamp, 0, 0, "s");
    f->addField("local_timestamp", EnumType, eMonitoringInfoLocalTimestamp, 0, 0, "s");
    f->addField("activity_type", EnumType, eMonitoringInfoActivityType, 0, 0, "");
    f->addField("cycles_to_distance", Uint16Type, eMonitoringInfoCyclesToDistance, 5000, 0, "m/cycle");
    f->addField("cycles_to_calories", Uint16Type, eMonitoringInfoCyclesToCalories, 5000, 0, "kcal/cycle");
    f->addField("resting_metabolic_rate", Uint16Type, eMonitoringInfoRestingMetabolicRate, 0, 0, "kcal / day");
    profiles.insert(eMesgNumMonitoringInfo, f);
}

void initMonitoring(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("monitoring", eMesgNumMonitoring);
    f->addField("timestamp", EnumType, eMonitoringTimestamp, 0, 0, "s");
    f->addField("device_index", EnumType, eMonitoringDeviceIndex, 0, 0, "");
    f->addField("calories", Uint16Type, eMonitoringCalories, 0, 0, "kcal");
    f->addField("distance", Uint32Type, eMonitoringDistance, 100, 0, "m");
    f->addField("cycles", Uint32Type, eMonitoringCycles, 2, 0, "cycles");
    f->addSubfield("steps", Uint32Type, eMonitoringCycles, 1, 0, "steps", eMonitoringActivityType, eActivityTypeWalking);
    f->addSubfield("steps", Uint32Type, eMonitoringCycles, 0, 0, "", eMonitoringActivityType, eActivityTypeRunning);
    f->addSubfield("strokes", Uint32Type, eMonitoringCycles, 2, 0, "strokes", eMonitoringActivityType, eActivityTypeCycling);
    f->addSubfield("strokes", Uint32Type, eMonitoringCycles, 0, 0, "", eMonitoringActivityType, eActivityTypeSwimming);
    f->addField("active_time", Uint32Type, eMonitoringActiveTime, 1000, 0, "s");
    f->addField("activity_type", EnumType, eMonitoringActivityType, 0, 0, "");
    f->addField("activity_subtype", EnumType, eMonitoringActivitySubtype, 0, 0, "");
    f->addField("activity_level", EnumType, eMonitoringActivityLevel, 0, 0, "");
    f->addField("distance_16", Uint16Type, eMonitoringDistance16, 0, 0, "100 * m");
    f->addField("cycles_16", Uint16Type, eMonitoringCycles16, 0, 0, "2 * cycles (steps)");
    f->addField("active_time_16", Uint16Type, eMonitoringActiveTime16, 0, 0, "s");
    f->addField("local_timestamp", EnumType, eMonitoringLocalTimestamp, 0, 0, "");
    f->addField("temperature", Sint16Type, eMonitoringTemperature, 100, 0, "C");
    f->addField("temperature_min", Sint16Type, eMonitoringTemperatureMin, 100, 0, "C");
    f->addField("temperature_max", Sint16Type, eMonitoringTemperatureMax, 100, 0, "C");
    f->addField("activity_time", Uint16Type, eMonitoringActivityTime, 0, 0, "minutes");
    f->addField("active_calories", Uint16Type, eMonitoringActiveCalories, 0, 0, "kcal");
    f->addField("current_activity_type_intensity", ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "");
    f->addComponent("current_activity_type_intensity", ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "", eMonitoringActivityType, 5);
    f->addComponent("current_activity_type_intensity", ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "", eMonitoringIntensity, 3);
    f->addField("timestamp_min_8", Uint8Type, eMonitoringTimestampMin8, 0, 0, "min");
    f->addField("timestamp_16", Uint16Type, eMonitoringTimestamp16, 0, 0, "s");
    f->addField("heart_rate", Uint8Type, eMonitoringHeartRate, 0, 0, "bpm");
    f->addField("intensity", Uint8Type, eMonitoringIntensity, 10, 0, "");
    f->addField("duration_min", Uint16Type, eMonitoringDurationMin, 0, 0, "min");
    f->addField("duration", Uint32Type, eMonitoringDuration, 0, 0, "s");
    profiles.insert(eMesgNumMonitoring, f);
}

void initMemoGlob(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("memo_glob", eMesgNumMemoGlob);
    f->addField("part_index", Uint32Type, eMemoGlobPartIndex, 0, 0, "");
    f->addField("memo", ByteType, eMemoGlobMemo, 0, 0, "");
    f->addField("message_number", Uint16Type, eMemoGlobMessageNumber, 0, 0, "");
    f->addField("message_index", EnumType, eMemoGlobMessageIndex, 0, 0, "");
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