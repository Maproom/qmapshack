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

#include "CFitProfileLookup.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/CFitFieldProfile.h"
#include "gis/fit/defs/CFitProfile.h"
#include "gis/fit/defs/fit_const.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_fields.h"

#include <QtWidgets>

// ----------- start generated code -----------
void initFileId(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("file_id", eMesgNumFileId);
    f->addField("type", fit::EnumType, eFileIdType, 0, 0, "");
    f->addField("manufacturer", fit::EnumType, eFileIdManufacturer, 0, 0, "");
    f->addField("product", fit::Uint16Type, eFileIdProduct, 0, 0, "");
    f->addSubfield("garmin_product", fit::EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fit::EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fit::EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", fit::Uint32zType, eFileIdSerialNumber, 0, 0, "");
    f->addField("time_created", fit::EnumType, eFileIdTimeCreated, 0, 0, "");
    f->addField("number", fit::Uint16Type, eFileIdNumber, 0, 0, "");
    f->addField("product_name", fit::StringType, eFileIdProductName, 0, 0, "");
    profiles.insert(eMesgNumFileId, f);
}

void initFileCreator(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("file_creator", eMesgNumFileCreator);
    f->addField("software_version", fit::Uint16Type, eFileCreatorSoftwareVersion, 0, 0, "");
    f->addField("hardware_version", fit::Uint8Type, eFileCreatorHardwareVersion, 0, 0, "");
    profiles.insert(eMesgNumFileCreator, f);
}

void initTimestampCorrelation(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("timestamp_correlation", eMesgNumTimestampCorrelation);
    f->addField("timestamp", fit::EnumType, eTimestampCorrelationTimestamp, 0, 0, "s");
    f->addField("fractional_timestamp", fit::Uint16Type, eTimestampCorrelationFractionalTimestamp, 32768, 0, "s");
    f->addField("system_timestamp", fit::EnumType, eTimestampCorrelationSystemTimestamp, 0, 0, "s");
    f->addField("fractional_system_timestamp", fit::Uint16Type, eTimestampCorrelationFractionalSystemTimestamp, 32768, 0, "s");
    f->addField("local_timestamp", fit::EnumType, eTimestampCorrelationLocalTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eTimestampCorrelationTimestampMs, 0, 0, "ms");
    f->addField("system_timestamp_ms", fit::Uint16Type, eTimestampCorrelationSystemTimestampMs, 0, 0, "ms");
    profiles.insert(eMesgNumTimestampCorrelation, f);
}

void initSoftware(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("software", eMesgNumSoftware);
    f->addField("message_index", fit::EnumType, eSoftwareMessageIndex, 0, 0, "");
    f->addField("version", fit::Uint16Type, eSoftwareVersion, 100, 0, "");
    f->addField("part_number", fit::StringType, eSoftwarePartNumber, 0, 0, "");
    profiles.insert(eMesgNumSoftware, f);
}

void initSlaveDevice(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("slave_device", eMesgNumSlaveDevice);
    f->addField("manufacturer", fit::EnumType, eSlaveDeviceManufacturer, 0, 0, "");
    f->addField("product", fit::Uint16Type, eSlaveDeviceProduct, 0, 0, "");
    f->addSubfield("garmin_product", fit::EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fit::EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fit::EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerDynastreamOem);
    profiles.insert(eMesgNumSlaveDevice, f);
}

void initCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("capabilities", eMesgNumCapabilities);
    f->addField("languages", fit::Uint8zType, eCapabilitiesLanguages, 0, 0, "");
    f->addField("sports", fit::EnumType, eCapabilitiesSports, 0, 0, "");
    f->addField("workouts_supported", fit::EnumType, eCapabilitiesWorkoutsSupported, 0, 0, "");
    f->addField("connectivity_supported", fit::EnumType, eCapabilitiesConnectivitySupported, 0, 0, "");
    profiles.insert(eMesgNumCapabilities, f);
}

void initFileCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("file_capabilities", eMesgNumFileCapabilities);
    f->addField("message_index", fit::EnumType, eFileCapabilitiesMessageIndex, 0, 0, "");
    f->addField("type", fit::EnumType, eFileCapabilitiesType, 0, 0, "");
    f->addField("flags", fit::EnumType, eFileCapabilitiesFlags, 0, 0, "");
    f->addField("directory", fit::StringType, eFileCapabilitiesDirectory, 0, 0, "");
    f->addField("max_count", fit::Uint16Type, eFileCapabilitiesMaxCount, 0, 0, "");
    f->addField("max_size", fit::Uint32Type, eFileCapabilitiesMaxSize, 0, 0, "bytes");
    profiles.insert(eMesgNumFileCapabilities, f);
}

void initMesgCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("mesg_capabilities", eMesgNumMesgCapabilities);
    f->addField("message_index", fit::EnumType, eMesgCapabilitiesMessageIndex, 0, 0, "");
    f->addField("file", fit::EnumType, eMesgCapabilitiesFile, 0, 0, "");
    f->addField("mesg_num", fit::EnumType, eMesgCapabilitiesMesgNum, 0, 0, "");
    f->addField("count_type", fit::EnumType, eMesgCapabilitiesCountType, 0, 0, "");
    f->addField("count", fit::Uint16Type, eMesgCapabilitiesCount, 0, 0, "");
    f->addSubfield("num_per_file", fit::Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountNumPerFile);
    f->addSubfield("max_per_file", fit::Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountMaxPerFile);
    f->addSubfield("max_per_file_type", fit::Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountMaxPerFileType);
    profiles.insert(eMesgNumMesgCapabilities, f);
}

void initFieldCapabilities(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("field_capabilities", eMesgNumFieldCapabilities);
    f->addField("message_index", fit::EnumType, eFieldCapabilitiesMessageIndex, 0, 0, "");
    f->addField("file", fit::EnumType, eFieldCapabilitiesFile, 0, 0, "");
    f->addField("mesg_num", fit::EnumType, eFieldCapabilitiesMesgNum, 0, 0, "");
    f->addField("field_num", fit::Uint8Type, eFieldCapabilitiesFieldNum, 0, 0, "");
    f->addField("count", fit::Uint16Type, eFieldCapabilitiesCount, 0, 0, "");
    profiles.insert(eMesgNumFieldCapabilities, f);
}

void initDeviceSettings(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("device_settings", eMesgNumDeviceSettings);
    f->addField("active_time_zone", fit::Uint8Type, eDeviceSettingsActiveTimeZone, 0, 0, "");
    f->addField("utc_offset", fit::Uint32Type, eDeviceSettingsUtcOffset, 0, 0, "");
    f->addField("time_offset", fit::Uint32Type, eDeviceSettingsTimeOffset, 0, 0, "s");
    f->addField("time_mode", fit::EnumType, eDeviceSettingsTimeMode, 0, 0, "");
    f->addField("time_zone_offset", fit::Sint8Type, eDeviceSettingsTimeZoneOffset, 4, 0, "hr");
    f->addField("backlight_mode", fit::EnumType, eDeviceSettingsBacklightMode, 0, 0, "");
    f->addField("activity_tracker_enabled", fit::EnumType, eDeviceSettingsActivityTrackerEnabled, 0, 0, "");
    f->addField("clock_time", fit::EnumType, eDeviceSettingsClockTime, 0, 0, "");
    f->addField("pages_enabled", fit::Uint16Type, eDeviceSettingsPagesEnabled, 0, 0, "");
    f->addField("move_alert_enabled", fit::EnumType, eDeviceSettingsMoveAlertEnabled, 0, 0, "");
    f->addField("date_mode", fit::EnumType, eDeviceSettingsDateMode, 0, 0, "");
    f->addField("display_orientation", fit::EnumType, eDeviceSettingsDisplayOrientation, 0, 0, "");
    f->addField("mounting_side", fit::EnumType, eDeviceSettingsMountingSide, 0, 0, "");
    f->addField("default_page", fit::Uint16Type, eDeviceSettingsDefaultPage, 0, 0, "");
    f->addField("autosync_min_steps", fit::Uint16Type, eDeviceSettingsAutosyncMinSteps, 0, 0, "steps");
    f->addField("autosync_min_time", fit::Uint16Type, eDeviceSettingsAutosyncMinTime, 0, 0, "minutes");
    f->addField("lactate_threshold_autodetect_enabled", fit::EnumType, eDeviceSettingsLactateThresholdAutodetectEnabled, 0, 0, "");
    f->addField("ble_auto_upload_enabled", fit::EnumType, eDeviceSettingsBleAutoUploadEnabled, 0, 0, "");
    f->addField("auto_sync_frequency", fit::EnumType, eDeviceSettingsAutoSyncFrequency, 0, 0, "");
    f->addField("auto_activity_detect", fit::EnumType, eDeviceSettingsAutoActivityDetect, 0, 0, "");
    f->addField("number_of_screens", fit::Uint8Type, eDeviceSettingsNumberOfScreens, 0, 0, "");
    f->addField("smart_notification_display_orientation", fit::EnumType, eDeviceSettingsSmartNotificationDisplayOrientation, 0, 0, "");
    profiles.insert(eMesgNumDeviceSettings, f);
}

void initUserProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("user_profile", eMesgNumUserProfile);
    f->addField("message_index", fit::EnumType, eUserProfileMessageIndex, 0, 0, "");
    f->addField("friendly_name", fit::StringType, eUserProfileFriendlyName, 0, 0, "");
    f->addField("gender", fit::EnumType, eUserProfileGender, 0, 0, "");
    f->addField("age", fit::Uint8Type, eUserProfileAge, 0, 0, "years");
    f->addField("height", fit::Uint8Type, eUserProfileHeight, 100, 0, "m");
    f->addField("weight", fit::Uint16Type, eUserProfileWeight, 10, 0, "kg");
    f->addField("language", fit::EnumType, eUserProfileLanguage, 0, 0, "");
    f->addField("elev_setting", fit::EnumType, eUserProfileElevSetting, 0, 0, "");
    f->addField("weight_setting", fit::EnumType, eUserProfileWeightSetting, 0, 0, "");
    f->addField("resting_heart_rate", fit::Uint8Type, eUserProfileRestingHeartRate, 0, 0, "bpm");
    f->addField("default_max_running_heart_rate", fit::Uint8Type, eUserProfileDefaultMaxRunningHeartRate, 0, 0, "bpm");
    f->addField("default_max_biking_heart_rate", fit::Uint8Type, eUserProfileDefaultMaxBikingHeartRate, 0, 0, "bpm");
    f->addField("default_max_heart_rate", fit::Uint8Type, eUserProfileDefaultMaxHeartRate, 0, 0, "bpm");
    f->addField("hr_setting", fit::EnumType, eUserProfileHrSetting, 0, 0, "");
    f->addField("speed_setting", fit::EnumType, eUserProfileSpeedSetting, 0, 0, "");
    f->addField("dist_setting", fit::EnumType, eUserProfileDistSetting, 0, 0, "");
    f->addField("power_setting", fit::EnumType, eUserProfilePowerSetting, 0, 0, "");
    f->addField("activity_class", fit::EnumType, eUserProfileActivityClass, 0, 0, "");
    f->addField("position_setting", fit::EnumType, eUserProfilePositionSetting, 0, 0, "");
    f->addField("temperature_setting", fit::EnumType, eUserProfileTemperatureSetting, 0, 0, "");
    f->addField("local_id", fit::EnumType, eUserProfileLocalId, 0, 0, "");
    f->addField("global_id", fit::ByteType, eUserProfileGlobalId, 0, 0, "");
    f->addField("wake_time", fit::EnumType, eUserProfileWakeTime, 0, 0, "");
    f->addField("sleep_time", fit::EnumType, eUserProfileSleepTime, 0, 0, "");
    f->addField("height_setting", fit::EnumType, eUserProfileHeightSetting, 0, 0, "");
    f->addField("user_running_step_length", fit::Uint16Type, eUserProfileUserRunningStepLength, 1000, 0, "m");
    f->addField("user_walking_step_length", fit::Uint16Type, eUserProfileUserWalkingStepLength, 1000, 0, "m");
    profiles.insert(eMesgNumUserProfile, f);
}

void initHrmProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hrm_profile", eMesgNumHrmProfile);
    f->addField("message_index", fit::EnumType, eHrmProfileMessageIndex, 0, 0, "");
    f->addField("enabled", fit::EnumType, eHrmProfileEnabled, 0, 0, "");
    f->addField("hrm_ant_id", fit::Uint16zType, eHrmProfileHrmAntId, 0, 0, "");
    f->addField("log_hrv", fit::EnumType, eHrmProfileLogHrv, 0, 0, "");
    f->addField("hrm_ant_id_trans_type", fit::Uint8zType, eHrmProfileHrmAntIdTransType, 0, 0, "");
    profiles.insert(eMesgNumHrmProfile, f);
}

void initSdmProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("sdm_profile", eMesgNumSdmProfile);
    f->addField("message_index", fit::EnumType, eSdmProfileMessageIndex, 0, 0, "");
    f->addField("enabled", fit::EnumType, eSdmProfileEnabled, 0, 0, "");
    f->addField("sdm_ant_id", fit::Uint16zType, eSdmProfileSdmAntId, 0, 0, "");
    f->addField("sdm_cal_factor", fit::Uint16Type, eSdmProfileSdmCalFactor, 10, 0, "%");
    f->addField("odometer", fit::Uint32Type, eSdmProfileOdometer, 100, 0, "m");
    f->addField("speed_source", fit::EnumType, eSdmProfileSpeedSource, 0, 0, "");
    f->addField("sdm_ant_id_trans_type", fit::Uint8zType, eSdmProfileSdmAntIdTransType, 0, 0, "");
    f->addField("odometer_rollover", fit::Uint8Type, eSdmProfileOdometerRollover, 0, 0, "");
    profiles.insert(eMesgNumSdmProfile, f);
}

void initBikeProfile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("bike_profile", eMesgNumBikeProfile);
    f->addField("message_index", fit::EnumType, eBikeProfileMessageIndex, 0, 0, "");
    f->addField("name", fit::StringType, eBikeProfileName, 0, 0, "");
    f->addField("sport", fit::EnumType, eBikeProfileSport, 0, 0, "");
    f->addField("sub_sport", fit::EnumType, eBikeProfileSubSport, 0, 0, "");
    f->addField("odometer", fit::Uint32Type, eBikeProfileOdometer, 100, 0, "m");
    f->addField("bike_spd_ant_id", fit::Uint16zType, eBikeProfileBikeSpdAntId, 0, 0, "");
    f->addField("bike_cad_ant_id", fit::Uint16zType, eBikeProfileBikeCadAntId, 0, 0, "");
    f->addField("bike_spdcad_ant_id", fit::Uint16zType, eBikeProfileBikeSpdcadAntId, 0, 0, "");
    f->addField("bike_power_ant_id", fit::Uint16zType, eBikeProfileBikePowerAntId, 0, 0, "");
    f->addField("custom_wheelsize", fit::Uint16Type, eBikeProfileCustomWheelsize, 1000, 0, "m");
    f->addField("auto_wheelsize", fit::Uint16Type, eBikeProfileAutoWheelsize, 1000, 0, "m");
    f->addField("bike_weight", fit::Uint16Type, eBikeProfileBikeWeight, 10, 0, "kg");
    f->addField("power_cal_factor", fit::Uint16Type, eBikeProfilePowerCalFactor, 10, 0, "%");
    f->addField("auto_wheel_cal", fit::EnumType, eBikeProfileAutoWheelCal, 0, 0, "");
    f->addField("auto_power_zero", fit::EnumType, eBikeProfileAutoPowerZero, 0, 0, "");
    f->addField("id", fit::Uint8Type, eBikeProfileId, 0, 0, "");
    f->addField("spd_enabled", fit::EnumType, eBikeProfileSpdEnabled, 0, 0, "");
    f->addField("cad_enabled", fit::EnumType, eBikeProfileCadEnabled, 0, 0, "");
    f->addField("spdcad_enabled", fit::EnumType, eBikeProfileSpdcadEnabled, 0, 0, "");
    f->addField("power_enabled", fit::EnumType, eBikeProfilePowerEnabled, 0, 0, "");
    f->addField("crank_length", fit::Uint8Type, eBikeProfileCrankLength, 2, -110, "mm");
    f->addField("enabled", fit::EnumType, eBikeProfileEnabled, 0, 0, "");
    f->addField("bike_spd_ant_id_trans_type", fit::Uint8zType, eBikeProfileBikeSpdAntIdTransType, 0, 0, "");
    f->addField("bike_cad_ant_id_trans_type", fit::Uint8zType, eBikeProfileBikeCadAntIdTransType, 0, 0, "");
    f->addField("bike_spdcad_ant_id_trans_type", fit::Uint8zType, eBikeProfileBikeSpdcadAntIdTransType, 0, 0, "");
    f->addField("bike_power_ant_id_trans_type", fit::Uint8zType, eBikeProfileBikePowerAntIdTransType, 0, 0, "");
    f->addField("odometer_rollover", fit::Uint8Type, eBikeProfileOdometerRollover, 0, 0, "");
    f->addField("front_gear_num", fit::Uint8zType, eBikeProfileFrontGearNum, 0, 0, "");
    f->addField("front_gear", fit::Uint8zType, eBikeProfileFrontGear, 0, 0, "");
    f->addField("rear_gear_num", fit::Uint8zType, eBikeProfileRearGearNum, 0, 0, "");
    f->addField("rear_gear", fit::Uint8zType, eBikeProfileRearGear, 0, 0, "");
    f->addField("shimano_di2_enabled", fit::EnumType, eBikeProfileShimanoDi2Enabled, 0, 0, "");
    profiles.insert(eMesgNumBikeProfile, f);
}

void initConnectivity(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("connectivity", eMesgNumConnectivity);
    f->addField("bluetooth_enabled", fit::EnumType, eConnectivityBluetoothEnabled, 0, 0, "");
    f->addField("bluetooth_le_enabled", fit::EnumType, eConnectivityBluetoothLeEnabled, 0, 0, "");
    f->addField("ant_enabled", fit::EnumType, eConnectivityAntEnabled, 0, 0, "");
    f->addField("name", fit::StringType, eConnectivityName, 0, 0, "");
    f->addField("live_tracking_enabled", fit::EnumType, eConnectivityLiveTrackingEnabled, 0, 0, "");
    f->addField("weather_conditions_enabled", fit::EnumType, eConnectivityWeatherConditionsEnabled, 0, 0, "");
    f->addField("weather_alerts_enabled", fit::EnumType, eConnectivityWeatherAlertsEnabled, 0, 0, "");
    f->addField("auto_activity_upload_enabled", fit::EnumType, eConnectivityAutoActivityUploadEnabled, 0, 0, "");
    f->addField("course_download_enabled", fit::EnumType, eConnectivityCourseDownloadEnabled, 0, 0, "");
    f->addField("workout_download_enabled", fit::EnumType, eConnectivityWorkoutDownloadEnabled, 0, 0, "");
    f->addField("gps_ephemeris_download_enabled", fit::EnumType, eConnectivityGpsEphemerisDownloadEnabled, 0, 0, "");
    f->addField("incident_detection_enabled", fit::EnumType, eConnectivityIncidentDetectionEnabled, 0, 0, "");
    f->addField("grouptrack_enabled", fit::EnumType, eConnectivityGrouptrackEnabled, 0, 0, "");
    profiles.insert(eMesgNumConnectivity, f);
}

void initWatchfaceSettings(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("watchface_settings", eMesgNumWatchfaceSettings);
    f->addField("message_index", fit::EnumType, eWatchfaceSettingsMessageIndex, 0, 0, "");
    f->addField("mode", fit::EnumType, eWatchfaceSettingsMode, 0, 0, "");
    f->addField("layout", fit::ByteType, eWatchfaceSettingsLayout, 0, 0, "");
    f->addSubfield("digital_layout", fit::EnumType, eWatchfaceSettingsLayout, 0, 0, "", eWatchfaceSettingsMode, eWatchfaceModeDigital);
    f->addSubfield("analog_layout", fit::EnumType, eWatchfaceSettingsLayout, 0, 0, "", eWatchfaceSettingsMode, eWatchfaceModeAnalog);
    profiles.insert(eMesgNumWatchfaceSettings, f);
}

void initOhrSettings(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("ohr_settings", eMesgNumOhrSettings);
    f->addField("enabled", fit::EnumType, eOhrSettingsEnabled, 0, 0, "");
    profiles.insert(eMesgNumOhrSettings, f);
}

void initZonesTarget(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("zones_target", eMesgNumZonesTarget);
    f->addField("max_heart_rate", fit::Uint8Type, eZonesTargetMaxHeartRate, 0, 0, "");
    f->addField("threshold_heart_rate", fit::Uint8Type, eZonesTargetThresholdHeartRate, 0, 0, "");
    f->addField("functional_threshold_power", fit::Uint16Type, eZonesTargetFunctionalThresholdPower, 0, 0, "");
    f->addField("hr_calc_type", fit::EnumType, eZonesTargetHrCalcType, 0, 0, "");
    f->addField("pwr_calc_type", fit::EnumType, eZonesTargetPwrCalcType, 0, 0, "");
    profiles.insert(eMesgNumZonesTarget, f);
}

void initSport(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("sport", eMesgNumSport);
    f->addField("sport", fit::EnumType, eSportSport, 0, 0, "");
    f->addField("sub_sport", fit::EnumType, eSportSubSport, 0, 0, "");
    f->addField("name", fit::StringType, eSportName, 0, 0, "");
    profiles.insert(eMesgNumSport, f);
}

void initHrZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hr_zone", eMesgNumHrZone);
    f->addField("message_index", fit::EnumType, eHrZoneMessageIndex, 0, 0, "");
    f->addField("high_bpm", fit::Uint8Type, eHrZoneHighBpm, 0, 0, "bpm");
    f->addField("name", fit::StringType, eHrZoneName, 0, 0, "");
    profiles.insert(eMesgNumHrZone, f);
}

void initSpeedZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("speed_zone", eMesgNumSpeedZone);
    f->addField("message_index", fit::EnumType, eSpeedZoneMessageIndex, 0, 0, "");
    f->addField("high_value", fit::Uint16Type, eSpeedZoneHighValue, 1000, 0, "m/s");
    f->addField("name", fit::StringType, eSpeedZoneName, 0, 0, "");
    profiles.insert(eMesgNumSpeedZone, f);
}

void initCadenceZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("cadence_zone", eMesgNumCadenceZone);
    f->addField("message_index", fit::EnumType, eCadenceZoneMessageIndex, 0, 0, "");
    f->addField("high_value", fit::Uint8Type, eCadenceZoneHighValue, 0, 0, "rpm");
    f->addField("name", fit::StringType, eCadenceZoneName, 0, 0, "");
    profiles.insert(eMesgNumCadenceZone, f);
}

void initPowerZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("power_zone", eMesgNumPowerZone);
    f->addField("message_index", fit::EnumType, ePowerZoneMessageIndex, 0, 0, "");
    f->addField("high_value", fit::Uint16Type, ePowerZoneHighValue, 0, 0, "watts");
    f->addField("name", fit::StringType, ePowerZoneName, 0, 0, "");
    profiles.insert(eMesgNumPowerZone, f);
}

void initMetZone(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("met_zone", eMesgNumMetZone);
    f->addField("message_index", fit::EnumType, eMetZoneMessageIndex, 0, 0, "");
    f->addField("high_bpm", fit::Uint8Type, eMetZoneHighBpm, 0, 0, "");
    f->addField("calories", fit::Uint16Type, eMetZoneCalories, 10, 0, "kcal / min");
    f->addField("fat_calories", fit::Uint8Type, eMetZoneFatCalories, 10, 0, "kcal / min");
    profiles.insert(eMesgNumMetZone, f);
}

void initGoal(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("goal", eMesgNumGoal);
    f->addField("message_index", fit::EnumType, eGoalMessageIndex, 0, 0, "");
    f->addField("sport", fit::EnumType, eGoalSport, 0, 0, "");
    f->addField("sub_sport", fit::EnumType, eGoalSubSport, 0, 0, "");
    f->addField("start_date", fit::EnumType, eGoalStartDate, 0, 0, "");
    f->addField("end_date", fit::EnumType, eGoalEndDate, 0, 0, "");
    f->addField("type", fit::EnumType, eGoalType, 0, 0, "");
    f->addField("value", fit::Uint32Type, eGoalValue, 0, 0, "");
    f->addField("repeat", fit::EnumType, eGoalRepeat, 0, 0, "");
    f->addField("target_value", fit::Uint32Type, eGoalTargetValue, 0, 0, "");
    f->addField("recurrence", fit::EnumType, eGoalRecurrence, 0, 0, "");
    f->addField("recurrence_value", fit::Uint16Type, eGoalRecurrenceValue, 0, 0, "");
    f->addField("enabled", fit::EnumType, eGoalEnabled, 0, 0, "");
    f->addField("source", fit::EnumType, eGoalSource, 0, 0, "");
    profiles.insert(eMesgNumGoal, f);
}

void initActivity(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("activity", eMesgNumActivity);
    f->addField("timestamp", fit::EnumType, eActivityTimestamp, 0, 0, "");
    f->addField("total_timer_time", fit::Uint32Type, eActivityTotalTimerTime, 1000, 0, "s");
    f->addField("num_sessions", fit::Uint16Type, eActivityNumSessions, 0, 0, "");
    f->addField("type", fit::EnumType, eActivityType, 0, 0, "");
    f->addField("event", fit::EnumType, eActivityEvent, 0, 0, "");
    f->addField("event_type", fit::EnumType, eActivityEventType, 0, 0, "");
    f->addField("local_timestamp", fit::EnumType, eActivityLocalTimestamp, 0, 0, "");
    f->addField("event_group", fit::Uint8Type, eActivityEventGroup, 0, 0, "");
    profiles.insert(eMesgNumActivity, f);
}

void initSession(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("session", eMesgNumSession);
    f->addField("message_index", fit::EnumType, eSessionMessageIndex, 0, 0, "");
    f->addField("timestamp", fit::EnumType, eSessionTimestamp, 0, 0, "s");
    f->addField("event", fit::EnumType, eSessionEvent, 0, 0, "");
    f->addField("event_type", fit::EnumType, eSessionEventType, 0, 0, "");
    f->addField("start_time", fit::EnumType, eSessionStartTime, 0, 0, "");
    f->addField("start_position_lat", fit::Sint32Type, eSessionStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", fit::Sint32Type, eSessionStartPositionLong, 0, 0, "semicircles");
    f->addField("sport", fit::EnumType, eSessionSport, 0, 0, "");
    f->addField("sub_sport", fit::EnumType, eSessionSubSport, 0, 0, "");
    f->addField("total_elapsed_time", fit::Uint32Type, eSessionTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", fit::Uint32Type, eSessionTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", fit::Uint32Type, eSessionTotalDistance, 100, 0, "m");
    f->addField("total_cycles", fit::Uint32Type, eSessionTotalCycles, 0, 0, "cycles");
    f->addField("total_calories", fit::Uint16Type, eSessionTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", fit::Uint16Type, eSessionTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", fit::Uint16Type, eSessionAvgSpeed, 0, 0, "");
    f->addComponent("avg_speed", fit::Uint16Type, eSessionAvgSpeed, 1000, 0, "m/s", eSessionEnhancedAvgSpeed, 16);
    f->addField("max_speed", fit::Uint16Type, eSessionMaxSpeed, 0, 0, "");
    f->addComponent("max_speed", fit::Uint16Type, eSessionMaxSpeed, 1000, 0, "m/s", eSessionEnhancedMaxSpeed, 16);
    f->addField("avg_heart_rate", fit::Uint8Type, eSessionAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", fit::Uint8Type, eSessionMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", fit::Uint8Type, eSessionAvgCadence, 0, 0, "rpm");
    f->addSubfield("avg_running_cadence", fit::Uint8Type, eSessionAvgCadence, 0, 0, "strides/min", eSessionSport, eSportRunning);
    f->addField("max_cadence", fit::Uint8Type, eSessionMaxCadence, 0, 0, "rpm");
    f->addSubfield("max_running_cadence", fit::Uint8Type, eSessionMaxCadence, 0, 0, "strides/min", eSessionSport, eSportRunning);
    f->addField("avg_power", fit::Uint16Type, eSessionAvgPower, 0, 0, "watts");
    f->addField("max_power", fit::Uint16Type, eSessionMaxPower, 0, 0, "watts");
    f->addField("total_ascent", fit::Uint16Type, eSessionTotalAscent, 0, 0, "m");
    f->addField("total_descent", fit::Uint16Type, eSessionTotalDescent, 0, 0, "m");
    f->addField("total_training_effect", fit::Uint8Type, eSessionTotalTrainingEffect, 10, 0, "");
    f->addField("first_lap_index", fit::Uint16Type, eSessionFirstLapIndex, 0, 0, "");
    f->addField("num_laps", fit::Uint16Type, eSessionNumLaps, 0, 0, "");
    f->addField("event_group", fit::Uint8Type, eSessionEventGroup, 0, 0, "");
    f->addField("trigger", fit::EnumType, eSessionTrigger, 0, 0, "");
    f->addField("nec_lat", fit::Sint32Type, eSessionNecLat, 0, 0, "semicircles");
    f->addField("nec_long", fit::Sint32Type, eSessionNecLong, 0, 0, "semicircles");
    f->addField("swc_lat", fit::Sint32Type, eSessionSwcLat, 0, 0, "semicircles");
    f->addField("swc_long", fit::Sint32Type, eSessionSwcLong, 0, 0, "semicircles");
    f->addField("normalized_power", fit::Uint16Type, eSessionNormalizedPower, 0, 0, "watts");
    f->addField("training_stress_score", fit::Uint16Type, eSessionTrainingStressScore, 10, 0, "tss");
    f->addField("intensity_factor", fit::Uint16Type, eSessionIntensityFactor, 1000, 0, "if");
    f->addField("left_right_balance", fit::EnumType, eSessionLeftRightBalance, 0, 0, "");
    f->addField("avg_stroke_count", fit::Uint32Type, eSessionAvgStrokeCount, 10, 0, "strokes/lap");
    f->addField("avg_stroke_distance", fit::Uint16Type, eSessionAvgStrokeDistance, 100, 0, "m");
    f->addField("swim_stroke", fit::EnumType, eSessionSwimStroke, 0, 0, "swim_stroke");
    f->addField("pool_length", fit::Uint16Type, eSessionPoolLength, 100, 0, "m");
    f->addField("threshold_power", fit::Uint16Type, eSessionThresholdPower, 0, 0, "watts");
    f->addField("pool_length_unit", fit::EnumType, eSessionPoolLengthUnit, 0, 0, "");
    f->addField("num_active_lengths", fit::Uint16Type, eSessionNumActiveLengths, 0, 0, "lengths");
    f->addField("total_work", fit::Uint32Type, eSessionTotalWork, 0, 0, "J");
    f->addField("avg_altitude", fit::Uint16Type, eSessionAvgAltitude, 0, 0, "");
    f->addComponent("avg_altitude", fit::Uint16Type, eSessionAvgAltitude, 5, 500, "m", eSessionEnhancedAvgAltitude, 16);
    f->addField("max_altitude", fit::Uint16Type, eSessionMaxAltitude, 0, 0, "");
    f->addComponent("max_altitude", fit::Uint16Type, eSessionMaxAltitude, 5, 500, "m", eSessionEnhancedMaxAltitude, 16);
    f->addField("gps_accuracy", fit::Uint8Type, eSessionGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", fit::Sint16Type, eSessionAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", fit::Sint16Type, eSessionAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", fit::Sint16Type, eSessionAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", fit::Sint16Type, eSessionMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", fit::Sint16Type, eSessionMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", fit::Sint8Type, eSessionAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", fit::Sint8Type, eSessionMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", fit::Uint32Type, eSessionTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", fit::Sint16Type, eSessionAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", fit::Sint16Type, eSessionAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", fit::Sint16Type, eSessionMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", fit::Sint16Type, eSessionMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("min_heart_rate", fit::Uint8Type, eSessionMinHeartRate, 0, 0, "bpm");
    f->addField("time_in_hr_zone", fit::Uint32Type, eSessionTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", fit::Uint32Type, eSessionTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", fit::Uint32Type, eSessionTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", fit::Uint32Type, eSessionTimeInPowerZone, 1000, 0, "s");
    f->addField("avg_lap_time", fit::Uint32Type, eSessionAvgLapTime, 1000, 0, "s");
    f->addField("best_lap_index", fit::Uint16Type, eSessionBestLapIndex, 0, 0, "");
    f->addField("min_altitude", fit::Uint16Type, eSessionMinAltitude, 0, 0, "");
    f->addComponent("min_altitude", fit::Uint16Type, eSessionMinAltitude, 5, 500, "m", eSessionEnhancedMinAltitude, 16);
    f->addField("player_score", fit::Uint16Type, eSessionPlayerScore, 0, 0, "");
    f->addField("opponent_score", fit::Uint16Type, eSessionOpponentScore, 0, 0, "");
    f->addField("opponent_name", fit::StringType, eSessionOpponentName, 0, 0, "");
    f->addField("stroke_count", fit::Uint16Type, eSessionStrokeCount, 0, 0, "counts");
    f->addField("zone_count", fit::Uint16Type, eSessionZoneCount, 0, 0, "counts");
    f->addField("max_ball_speed", fit::Uint16Type, eSessionMaxBallSpeed, 100, 0, "m/s");
    f->addField("avg_ball_speed", fit::Uint16Type, eSessionAvgBallSpeed, 100, 0, "m/s");
    f->addField("avg_vertical_oscillation", fit::Uint16Type, eSessionAvgVerticalOscillation, 10, 0, "mm");
    f->addField("avg_stance_time_percent", fit::Uint16Type, eSessionAvgStanceTimePercent, 100, 0, "percent");
    f->addField("avg_stance_time", fit::Uint16Type, eSessionAvgStanceTime, 10, 0, "ms");
    f->addField("avg_fractional_cadence", fit::Uint8Type, eSessionAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", fit::Uint8Type, eSessionMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", fit::Uint8Type, eSessionTotalFractionalCycles, 128, 0, "cycles");
    f->addField("avg_total_hemoglobin_conc", fit::Uint16Type, eSessionAvgTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("min_total_hemoglobin_conc", fit::Uint16Type, eSessionMinTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("max_total_hemoglobin_conc", fit::Uint16Type, eSessionMaxTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("avg_saturated_hemoglobin_percent", fit::Uint16Type, eSessionAvgSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("min_saturated_hemoglobin_percent", fit::Uint16Type, eSessionMinSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("max_saturated_hemoglobin_percent", fit::Uint16Type, eSessionMaxSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("avg_left_torque_effectiveness", fit::Uint8Type, eSessionAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", fit::Uint8Type, eSessionAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", fit::Uint8Type, eSessionAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", fit::Uint8Type, eSessionAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", fit::Uint8Type, eSessionAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("sport_index", fit::Uint8Type, eSessionSportIndex, 0, 0, "");
    f->addField("time_standing", fit::Uint32Type, eSessionTimeStanding, 1000, 0, "s");
    f->addField("stand_count", fit::Uint16Type, eSessionStandCount, 0, 0, "");
    f->addField("avg_left_pco", fit::Sint8Type, eSessionAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", fit::Sint8Type, eSessionAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", fit::Uint8Type, eSessionAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", fit::Uint8Type, eSessionAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", fit::Uint8Type, eSessionAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", fit::Uint8Type, eSessionAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", fit::Uint16Type, eSessionAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", fit::Uint16Type, eSessionMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", fit::Uint8Type, eSessionAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", fit::Uint8Type, eSessionMaxCadencePosition, 0, 0, "rpm");
    f->addField("enhanced_avg_speed", fit::Uint32Type, eSessionEnhancedAvgSpeed, 1000, 0, "m/s");
    f->addField("enhanced_max_speed", fit::Uint32Type, eSessionEnhancedMaxSpeed, 1000, 0, "m/s");
    f->addField("enhanced_avg_altitude", fit::Uint32Type, eSessionEnhancedAvgAltitude, 5, 500, "m");
    f->addField("enhanced_min_altitude", fit::Uint32Type, eSessionEnhancedMinAltitude, 5, 500, "m");
    f->addField("enhanced_max_altitude", fit::Uint32Type, eSessionEnhancedMaxAltitude, 5, 500, "m");
    f->addField("avg_lev_motor_power", fit::Uint16Type, eSessionAvgLevMotorPower, 0, 0, "watts");
    f->addField("max_lev_motor_power", fit::Uint16Type, eSessionMaxLevMotorPower, 0, 0, "watts");
    f->addField("lev_battery_consumption", fit::Uint8Type, eSessionLevBatteryConsumption, 2, 0, "percent");
    f->addField("avg_vertical_ratio", fit::Uint16Type, eSessionAvgVerticalRatio, 100, 0, "percent");
    f->addField("avg_stance_time_balance", fit::Uint16Type, eSessionAvgStanceTimeBalance, 100, 0, "percent");
    f->addField("avg_step_length", fit::Uint16Type, eSessionAvgStepLength, 10, 0, "mm");
    f->addField("total_anaerobic_training_effect", fit::Uint8Type, eSessionTotalAnaerobicTrainingEffect, 10, 0, "");
    f->addField("avg_vam", fit::Uint16Type, eSessionAvgVam, 1000, 0, "m/s");
    profiles.insert(eMesgNumSession, f);
}

void initLap(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("lap", eMesgNumLap);
    f->addField("message_index", fit::EnumType, eLapMessageIndex, 0, 0, "");
    f->addField("timestamp", fit::EnumType, eLapTimestamp, 0, 0, "s");
    f->addField("event", fit::EnumType, eLapEvent, 0, 0, "");
    f->addField("event_type", fit::EnumType, eLapEventType, 0, 0, "");
    f->addField("start_time", fit::EnumType, eLapStartTime, 0, 0, "");
    f->addField("start_position_lat", fit::Sint32Type, eLapStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", fit::Sint32Type, eLapStartPositionLong, 0, 0, "semicircles");
    f->addField("end_position_lat", fit::Sint32Type, eLapEndPositionLat, 0, 0, "semicircles");
    f->addField("end_position_long", fit::Sint32Type, eLapEndPositionLong, 0, 0, "semicircles");
    f->addField("total_elapsed_time", fit::Uint32Type, eLapTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", fit::Uint32Type, eLapTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", fit::Uint32Type, eLapTotalDistance, 100, 0, "m");
    f->addField("total_cycles", fit::Uint32Type, eLapTotalCycles, 0, 0, "cycles");
    f->addField("total_calories", fit::Uint16Type, eLapTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", fit::Uint16Type, eLapTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", fit::Uint16Type, eLapAvgSpeed, 0, 0, "");
    f->addComponent("avg_speed", fit::Uint16Type, eLapAvgSpeed, 1000, 0, "m/s", eLapEnhancedAvgSpeed, 16);
    f->addField("max_speed", fit::Uint16Type, eLapMaxSpeed, 0, 0, "");
    f->addComponent("max_speed", fit::Uint16Type, eLapMaxSpeed, 1000, 0, "m/s", eLapEnhancedMaxSpeed, 16);
    f->addField("avg_heart_rate", fit::Uint8Type, eLapAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", fit::Uint8Type, eLapMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", fit::Uint8Type, eLapAvgCadence, 0, 0, "rpm");
    f->addSubfield("avg_running_cadence", fit::Uint8Type, eLapAvgCadence, 0, 0, "strides/min", eLapSport, eSportRunning);
    f->addField("max_cadence", fit::Uint8Type, eLapMaxCadence, 0, 0, "rpm");
    f->addSubfield("max_running_cadence", fit::Uint8Type, eLapMaxCadence, 0, 0, "strides/min", eLapSport, eSportRunning);
    f->addField("avg_power", fit::Uint16Type, eLapAvgPower, 0, 0, "watts");
    f->addField("max_power", fit::Uint16Type, eLapMaxPower, 0, 0, "watts");
    f->addField("total_ascent", fit::Uint16Type, eLapTotalAscent, 0, 0, "m");
    f->addField("total_descent", fit::Uint16Type, eLapTotalDescent, 0, 0, "m");
    f->addField("intensity", fit::EnumType, eLapIntensity, 0, 0, "");
    f->addField("lap_trigger", fit::EnumType, eLapLapTrigger, 0, 0, "");
    f->addField("sport", fit::EnumType, eLapSport, 0, 0, "");
    f->addField("event_group", fit::Uint8Type, eLapEventGroup, 0, 0, "");
    f->addField("num_lengths", fit::Uint16Type, eLapNumLengths, 0, 0, "lengths");
    f->addField("normalized_power", fit::Uint16Type, eLapNormalizedPower, 0, 0, "watts");
    f->addField("left_right_balance", fit::EnumType, eLapLeftRightBalance, 0, 0, "");
    f->addField("first_length_index", fit::Uint16Type, eLapFirstLengthIndex, 0, 0, "");
    f->addField("avg_stroke_distance", fit::Uint16Type, eLapAvgStrokeDistance, 100, 0, "m");
    f->addField("swim_stroke", fit::EnumType, eLapSwimStroke, 0, 0, "");
    f->addField("sub_sport", fit::EnumType, eLapSubSport, 0, 0, "");
    f->addField("num_active_lengths", fit::Uint16Type, eLapNumActiveLengths, 0, 0, "lengths");
    f->addField("total_work", fit::Uint32Type, eLapTotalWork, 0, 0, "J");
    f->addField("avg_altitude", fit::Uint16Type, eLapAvgAltitude, 0, 0, "");
    f->addComponent("avg_altitude", fit::Uint16Type, eLapAvgAltitude, 5, 500, "m", eLapEnhancedAvgAltitude, 16);
    f->addField("max_altitude", fit::Uint16Type, eLapMaxAltitude, 0, 0, "");
    f->addComponent("max_altitude", fit::Uint16Type, eLapMaxAltitude, 5, 500, "m", eLapEnhancedMaxAltitude, 16);
    f->addField("gps_accuracy", fit::Uint8Type, eLapGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", fit::Sint16Type, eLapAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", fit::Sint16Type, eLapAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", fit::Sint16Type, eLapAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", fit::Sint16Type, eLapMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", fit::Sint16Type, eLapMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", fit::Sint8Type, eLapAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", fit::Sint8Type, eLapMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", fit::Uint32Type, eLapTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", fit::Sint16Type, eLapAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", fit::Sint16Type, eLapAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", fit::Sint16Type, eLapMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", fit::Sint16Type, eLapMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("time_in_hr_zone", fit::Uint32Type, eLapTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", fit::Uint32Type, eLapTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", fit::Uint32Type, eLapTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", fit::Uint32Type, eLapTimeInPowerZone, 1000, 0, "s");
    f->addField("repetition_num", fit::Uint16Type, eLapRepetitionNum, 0, 0, "");
    f->addField("min_altitude", fit::Uint16Type, eLapMinAltitude, 0, 0, "");
    f->addComponent("min_altitude", fit::Uint16Type, eLapMinAltitude, 5, 500, "m", eLapEnhancedMinAltitude, 16);
    f->addField("min_heart_rate", fit::Uint8Type, eLapMinHeartRate, 0, 0, "bpm");
    f->addField("wkt_step_index", fit::EnumType, eLapWktStepIndex, 0, 0, "");
    f->addField("opponent_score", fit::Uint16Type, eLapOpponentScore, 0, 0, "");
    f->addField("stroke_count", fit::Uint16Type, eLapStrokeCount, 0, 0, "counts");
    f->addField("zone_count", fit::Uint16Type, eLapZoneCount, 0, 0, "counts");
    f->addField("avg_vertical_oscillation", fit::Uint16Type, eLapAvgVerticalOscillation, 10, 0, "mm");
    f->addField("avg_stance_time_percent", fit::Uint16Type, eLapAvgStanceTimePercent, 100, 0, "percent");
    f->addField("avg_stance_time", fit::Uint16Type, eLapAvgStanceTime, 10, 0, "ms");
    f->addField("avg_fractional_cadence", fit::Uint8Type, eLapAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", fit::Uint8Type, eLapMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", fit::Uint8Type, eLapTotalFractionalCycles, 128, 0, "cycles");
    f->addField("player_score", fit::Uint16Type, eLapPlayerScore, 0, 0, "");
    f->addField("avg_total_hemoglobin_conc", fit::Uint16Type, eLapAvgTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("min_total_hemoglobin_conc", fit::Uint16Type, eLapMinTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("max_total_hemoglobin_conc", fit::Uint16Type, eLapMaxTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("avg_saturated_hemoglobin_percent", fit::Uint16Type, eLapAvgSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("min_saturated_hemoglobin_percent", fit::Uint16Type, eLapMinSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("max_saturated_hemoglobin_percent", fit::Uint16Type, eLapMaxSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("avg_left_torque_effectiveness", fit::Uint8Type, eLapAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", fit::Uint8Type, eLapAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", fit::Uint8Type, eLapAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", fit::Uint8Type, eLapAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", fit::Uint8Type, eLapAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("time_standing", fit::Uint32Type, eLapTimeStanding, 1000, 0, "s");
    f->addField("stand_count", fit::Uint16Type, eLapStandCount, 0, 0, "");
    f->addField("avg_left_pco", fit::Sint8Type, eLapAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", fit::Sint8Type, eLapAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", fit::Uint8Type, eLapAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", fit::Uint8Type, eLapAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", fit::Uint8Type, eLapAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", fit::Uint8Type, eLapAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", fit::Uint16Type, eLapAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", fit::Uint16Type, eLapMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", fit::Uint8Type, eLapAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", fit::Uint8Type, eLapMaxCadencePosition, 0, 0, "rpm");
    f->addField("enhanced_avg_speed", fit::Uint32Type, eLapEnhancedAvgSpeed, 1000, 0, "m/s");
    f->addField("enhanced_max_speed", fit::Uint32Type, eLapEnhancedMaxSpeed, 1000, 0, "m/s");
    f->addField("enhanced_avg_altitude", fit::Uint32Type, eLapEnhancedAvgAltitude, 5, 500, "m");
    f->addField("enhanced_min_altitude", fit::Uint32Type, eLapEnhancedMinAltitude, 5, 500, "m");
    f->addField("enhanced_max_altitude", fit::Uint32Type, eLapEnhancedMaxAltitude, 5, 500, "m");
    f->addField("avg_lev_motor_power", fit::Uint16Type, eLapAvgLevMotorPower, 0, 0, "watts");
    f->addField("max_lev_motor_power", fit::Uint16Type, eLapMaxLevMotorPower, 0, 0, "watts");
    f->addField("lev_battery_consumption", fit::Uint8Type, eLapLevBatteryConsumption, 2, 0, "percent");
    f->addField("avg_vertical_ratio", fit::Uint16Type, eLapAvgVerticalRatio, 100, 0, "percent");
    f->addField("avg_stance_time_balance", fit::Uint16Type, eLapAvgStanceTimeBalance, 100, 0, "percent");
    f->addField("avg_step_length", fit::Uint16Type, eLapAvgStepLength, 10, 0, "mm");
    f->addField("avg_vam", fit::Uint16Type, eLapAvgVam, 1000, 0, "m/s");
    profiles.insert(eMesgNumLap, f);
}

void initLength(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("length", eMesgNumLength);
    f->addField("message_index", fit::EnumType, eLengthMessageIndex, 0, 0, "");
    f->addField("timestamp", fit::EnumType, eLengthTimestamp, 0, 0, "");
    f->addField("event", fit::EnumType, eLengthEvent, 0, 0, "");
    f->addField("event_type", fit::EnumType, eLengthEventType, 0, 0, "");
    f->addField("start_time", fit::EnumType, eLengthStartTime, 0, 0, "");
    f->addField("total_elapsed_time", fit::Uint32Type, eLengthTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", fit::Uint32Type, eLengthTotalTimerTime, 1000, 0, "s");
    f->addField("total_strokes", fit::Uint16Type, eLengthTotalStrokes, 0, 0, "strokes");
    f->addField("avg_speed", fit::Uint16Type, eLengthAvgSpeed, 1000, 0, "m/s");
    f->addField("swim_stroke", fit::EnumType, eLengthSwimStroke, 0, 0, "swim_stroke");
    f->addField("avg_swimming_cadence", fit::Uint8Type, eLengthAvgSwimmingCadence, 0, 0, "strokes/min");
    f->addField("event_group", fit::Uint8Type, eLengthEventGroup, 0, 0, "");
    f->addField("total_calories", fit::Uint16Type, eLengthTotalCalories, 0, 0, "kcal");
    f->addField("length_type", fit::EnumType, eLengthLengthType, 0, 0, "");
    f->addField("player_score", fit::Uint16Type, eLengthPlayerScore, 0, 0, "");
    f->addField("opponent_score", fit::Uint16Type, eLengthOpponentScore, 0, 0, "");
    f->addField("stroke_count", fit::Uint16Type, eLengthStrokeCount, 0, 0, "counts");
    f->addField("zone_count", fit::Uint16Type, eLengthZoneCount, 0, 0, "counts");
    profiles.insert(eMesgNumLength, f);
}

void initRecord(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("record", eMesgNumRecord);
    f->addField("timestamp", fit::EnumType, eRecordTimestamp, 0, 0, "s");
    f->addField("position_lat", fit::Sint32Type, eRecordPositionLat, 0, 0, "semicircles");
    f->addField("position_long", fit::Sint32Type, eRecordPositionLong, 0, 0, "semicircles");
    f->addField("altitude", fit::Uint16Type, eRecordAltitude, 0, 0, "");
    f->addComponent("altitude", fit::Uint16Type, eRecordAltitude, 5, 500, "m", eRecordEnhancedAltitude, 16);
    f->addField("heart_rate", fit::Uint8Type, eRecordHeartRate, 0, 0, "bpm");
    f->addField("cadence", fit::Uint8Type, eRecordCadence, 0, 0, "rpm");
    f->addField("distance", fit::Uint32Type, eRecordDistance, 100, 0, "m");
    f->addField("speed", fit::Uint16Type, eRecordSpeed, 0, 0, "");
    f->addComponent("speed", fit::Uint16Type, eRecordSpeed, 1000, 0, "m/s", eRecordEnhancedSpeed, 16);
    f->addField("power", fit::Uint16Type, eRecordPower, 0, 0, "watts");
    f->addField("compressed_speed_distance", fit::ByteType, eRecordCompressedSpeedDistance, 0, 0, "");
    f->addComponent("compressed_speed_distance", fit::ByteType, eRecordCompressedSpeedDistance, 100, 0, "m/s", eRecordSpeed, 12);
    f->addComponent("compressed_speed_distance", fit::ByteType, eRecordCompressedSpeedDistance, 16, 0, "m", eRecordDistance, 12);
    f->addField("grade", fit::Sint16Type, eRecordGrade, 100, 0, "%");
    f->addField("resistance", fit::Uint8Type, eRecordResistance, 0, 0, "");
    f->addField("time_from_course", fit::Sint32Type, eRecordTimeFromCourse, 1000, 0, "s");
    f->addField("cycle_length", fit::Uint8Type, eRecordCycleLength, 100, 0, "m");
    f->addField("temperature", fit::Sint8Type, eRecordTemperature, 0, 0, "C");
    f->addField("speed_1s", fit::Uint8Type, eRecordSpeed1S, 16, 0, "m/s");
    f->addField("cycles", fit::Uint8Type, eRecordCycles, 0, 0, "");
    f->addComponent("cycles", fit::Uint8Type, eRecordCycles, 0, 0, "cycles", eRecordTotalCycles, 8);
    f->addField("total_cycles", fit::Uint32Type, eRecordTotalCycles, 0, 0, "cycles");
    f->addField("compressed_accumulated_power", fit::Uint16Type, eRecordCompressedAccumulatedPower, 0, 0, "");
    f->addComponent("compressed_accumulated_power", fit::Uint16Type, eRecordCompressedAccumulatedPower, 0, 0, "watts", eRecordAccumulatedPower, 16);
    f->addField("accumulated_power", fit::Uint32Type, eRecordAccumulatedPower, 0, 0, "watts");
    f->addField("left_right_balance", fit::EnumType, eRecordLeftRightBalance, 0, 0, "");
    f->addField("gps_accuracy", fit::Uint8Type, eRecordGpsAccuracy, 0, 0, "m");
    f->addField("vertical_speed", fit::Sint16Type, eRecordVerticalSpeed, 1000, 0, "m/s");
    f->addField("calories", fit::Uint16Type, eRecordCalories, 0, 0, "kcal");
    f->addField("vertical_oscillation", fit::Uint16Type, eRecordVerticalOscillation, 10, 0, "mm");
    f->addField("stance_time_percent", fit::Uint16Type, eRecordStanceTimePercent, 100, 0, "percent");
    f->addField("stance_time", fit::Uint16Type, eRecordStanceTime, 10, 0, "ms");
    f->addField("activity_type", fit::EnumType, eRecordActivityType, 0, 0, "");
    f->addField("left_torque_effectiveness", fit::Uint8Type, eRecordLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("right_torque_effectiveness", fit::Uint8Type, eRecordRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("left_pedal_smoothness", fit::Uint8Type, eRecordLeftPedalSmoothness, 2, 0, "percent");
    f->addField("right_pedal_smoothness", fit::Uint8Type, eRecordRightPedalSmoothness, 2, 0, "percent");
    f->addField("combined_pedal_smoothness", fit::Uint8Type, eRecordCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("time128", fit::Uint8Type, eRecordTime128, 128, 0, "s");
    f->addField("stroke_type", fit::EnumType, eRecordStrokeType, 0, 0, "");
    f->addField("zone", fit::Uint8Type, eRecordZone, 0, 0, "");
    f->addField("ball_speed", fit::Uint16Type, eRecordBallSpeed, 100, 0, "m/s");
    f->addField("cadence256", fit::Uint16Type, eRecordCadence256, 256, 0, "rpm");
    f->addField("fractional_cadence", fit::Uint8Type, eRecordFractionalCadence, 128, 0, "rpm");
    f->addField("total_hemoglobin_conc", fit::Uint16Type, eRecordTotalHemoglobinConc, 100, 0, "g/dL");
    f->addField("total_hemoglobin_conc_min", fit::Uint16Type, eRecordTotalHemoglobinConcMin, 100, 0, "g/dL");
    f->addField("total_hemoglobin_conc_max", fit::Uint16Type, eRecordTotalHemoglobinConcMax, 100, 0, "g/dL");
    f->addField("saturated_hemoglobin_percent", fit::Uint16Type, eRecordSaturatedHemoglobinPercent, 10, 0, "%");
    f->addField("saturated_hemoglobin_percent_min", fit::Uint16Type, eRecordSaturatedHemoglobinPercentMin, 10, 0, "%");
    f->addField("saturated_hemoglobin_percent_max", fit::Uint16Type, eRecordSaturatedHemoglobinPercentMax, 10, 0, "%");
    f->addField("device_index", fit::EnumType, eRecordDeviceIndex, 0, 0, "");
    f->addField("left_pco", fit::Sint8Type, eRecordLeftPco, 0, 0, "mm");
    f->addField("right_pco", fit::Sint8Type, eRecordRightPco, 0, 0, "mm");
    f->addField("left_power_phase", fit::Uint8Type, eRecordLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("left_power_phase_peak", fit::Uint8Type, eRecordLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("right_power_phase", fit::Uint8Type, eRecordRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("right_power_phase_peak", fit::Uint8Type, eRecordRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("enhanced_speed", fit::Uint32Type, eRecordEnhancedSpeed, 1000, 0, "m/s");
    f->addField("enhanced_altitude", fit::Uint32Type, eRecordEnhancedAltitude, 5, 500, "m");
    f->addField("battery_soc", fit::Uint8Type, eRecordBatterySoc, 2, 0, "percent");
    f->addField("motor_power", fit::Uint16Type, eRecordMotorPower, 0, 0, "watts");
    f->addField("vertical_ratio", fit::Uint16Type, eRecordVerticalRatio, 100, 0, "percent");
    f->addField("stance_time_balance", fit::Uint16Type, eRecordStanceTimeBalance, 100, 0, "percent");
    f->addField("step_length", fit::Uint16Type, eRecordStepLength, 10, 0, "mm");
    profiles.insert(eMesgNumRecord, f);
}

void initEvent(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("event", eMesgNumEvent);
    f->addField("timestamp", fit::EnumType, eEventTimestamp, 0, 0, "s");
    f->addField("event", fit::EnumType, eEventEvent, 0, 0, "");
    f->addField("event_type", fit::EnumType, eEventEventType, 0, 0, "");
    f->addField("data16", fit::Uint16Type, eEventData16, 0, 0, "");
    f->addComponent("data16", fit::Uint16Type, eEventData16, 0, 0, "", eEventData, 16);
    f->addField("data", fit::Uint32Type, eEventData, 0, 0, "");
    f->addSubfield("timer_trigger", fit::EnumType, eEventData, 0, 0, "", eEventEvent, eEventTimer);
    f->addSubfield("course_point_index", fit::EnumType, eEventData, 0, 0, "", eEventEvent, eEventCoursePoint);
    f->addSubfield("battery_level", fit::Uint16Type, eEventData, 1000, 0, "V", eEventEvent, eEventBattery);
    f->addSubfield("virtual_partner_speed", fit::Uint16Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventVirtualPartnerPace);
    f->addSubfield("hr_high_alert", fit::Uint8Type, eEventData, 0, 0, "bpm", eEventEvent, eEventHrHighAlert);
    f->addSubfield("hr_low_alert", fit::Uint8Type, eEventData, 0, 0, "bpm", eEventEvent, eEventHrLowAlert);
    f->addSubfield("speed_high_alert", fit::Uint32Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventSpeedHighAlert);
    f->addSubfield("speed_low_alert", fit::Uint32Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventSpeedLowAlert);
    f->addSubfield("cad_high_alert", fit::Uint16Type, eEventData, 0, 0, "rpm", eEventEvent, eEventCadHighAlert);
    f->addSubfield("cad_low_alert", fit::Uint16Type, eEventData, 0, 0, "rpm", eEventEvent, eEventCadLowAlert);
    f->addSubfield("power_high_alert", fit::Uint16Type, eEventData, 0, 0, "watts", eEventEvent, eEventPowerHighAlert);
    f->addSubfield("power_low_alert", fit::Uint16Type, eEventData, 0, 0, "watts", eEventEvent, eEventPowerLowAlert);
    f->addSubfield("time_duration_alert", fit::Uint32Type, eEventData, 1000, 0, "s", eEventEvent, eEventTimeDurationAlert);
    f->addSubfield("distance_duration_alert", fit::Uint32Type, eEventData, 100, 0, "m", eEventEvent, eEventDistanceDurationAlert);
    f->addSubfield("calorie_duration_alert", fit::Uint32Type, eEventData, 0, 0, "calories", eEventEvent, eEventCalorieDurationAlert);
    f->addSubfield("fitness_equipment_state", fit::EnumType, eEventData, 0, 0, "", eEventEvent, eEventFitnessEquipment);
    f->addSubfield("sport_point", fit::Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventSportPoint);
    f->addComponent(16, "sport_point", fit::Uint32Type, eEventData, 11, 0, "", eEventScore, 1616);
    f->addComponent(16, "sport_point", fit::Uint32Type, eEventData, 0, 0, "", eEventOpponentScore, 0);
    f->addSubfield("gear_change_data", fit::Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventFrontGearChange);
    f->addSubfield("gear_change_data", fit::Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventRearGearChange);
    f->addComponent(18, "gear_change_data", fit::Uint32Type, eEventData, 1111, 0, "", eEventRearGearNum, 8);
    f->addComponent(18, "gear_change_data", fit::Uint32Type, eEventData, 0, 0, "", eEventRearGear, 8);
    f->addComponent(18, "gear_change_data", fit::Uint32Type, eEventData, 0, 0, "", eEventFrontGearNum, 8);
    f->addComponent(18, "gear_change_data", fit::Uint32Type, eEventData, 0, 0, "", eEventFrontGear, 8);
    f->addSubfield("rider_position", fit::EnumType, eEventData, 0, 0, "", eEventEvent, eEventRiderPositionChange);
    f->addSubfield("comm_timeout", fit::EnumType, eEventData, 0, 0, "", eEventEvent, eEventCommTimeout);
    f->addField("event_group", fit::Uint8Type, eEventEventGroup, 0, 0, "");
    f->addField("score", fit::Uint16Type, eEventScore, 0, 0, "");
    f->addField("opponent_score", fit::Uint16Type, eEventOpponentScore, 0, 0, "");
    f->addField("front_gear_num", fit::Uint8zType, eEventFrontGearNum, 0, 0, "");
    f->addField("front_gear", fit::Uint8zType, eEventFrontGear, 0, 0, "");
    f->addField("rear_gear_num", fit::Uint8zType, eEventRearGearNum, 0, 0, "");
    f->addField("rear_gear", fit::Uint8zType, eEventRearGear, 0, 0, "");
    f->addField("device_index", fit::EnumType, eEventDeviceIndex, 0, 0, "");
    profiles.insert(eMesgNumEvent, f);
}

void initDeviceInfo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("device_info", eMesgNumDeviceInfo);
    f->addField("timestamp", fit::EnumType, eDeviceInfoTimestamp, 0, 0, "s");
    f->addField("device_index", fit::EnumType, eDeviceInfoDeviceIndex, 0, 0, "");
    f->addField("device_type", fit::Uint8Type, eDeviceInfoDeviceType, 0, 0, "");
    f->addSubfield("antplus_device_type", fit::EnumType, eDeviceInfoDeviceType, 0, 0, "", eDeviceInfoSourceType, eSourceTypeAntplus);
    f->addSubfield("ant_device_type", fit::Uint8Type, eDeviceInfoDeviceType, 0, 0, "", eDeviceInfoSourceType, eSourceTypeAnt);
    f->addField("manufacturer", fit::EnumType, eDeviceInfoManufacturer, 0, 0, "");
    f->addField("serial_number", fit::Uint32zType, eDeviceInfoSerialNumber, 0, 0, "");
    f->addField("product", fit::Uint16Type, eDeviceInfoProduct, 0, 0, "");
    f->addSubfield("garmin_product", fit::EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fit::EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fit::EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerDynastreamOem);
    f->addField("software_version", fit::Uint16Type, eDeviceInfoSoftwareVersion, 100, 0, "");
    f->addField("hardware_version", fit::Uint8Type, eDeviceInfoHardwareVersion, 0, 0, "");
    f->addField("cum_operating_time", fit::Uint32Type, eDeviceInfoCumOperatingTime, 0, 0, "s");
    f->addField("battery_voltage", fit::Uint16Type, eDeviceInfoBatteryVoltage, 256, 0, "V");
    f->addField("battery_status", fit::EnumType, eDeviceInfoBatteryStatus, 0, 0, "");
    f->addField("sensor_position", fit::EnumType, eDeviceInfoSensorPosition, 0, 0, "");
    f->addField("descriptor", fit::StringType, eDeviceInfoDescriptor, 0, 0, "");
    f->addField("ant_transmission_type", fit::Uint8zType, eDeviceInfoAntTransmissionType, 0, 0, "");
    f->addField("ant_device_number", fit::Uint16zType, eDeviceInfoAntDeviceNumber, 0, 0, "");
    f->addField("ant_network", fit::EnumType, eDeviceInfoAntNetwork, 0, 0, "");
    f->addField("source_type", fit::EnumType, eDeviceInfoSourceType, 0, 0, "");
    f->addField("product_name", fit::StringType, eDeviceInfoProductName, 0, 0, "");
    profiles.insert(eMesgNumDeviceInfo, f);
}

void initTrainingFile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("training_file", eMesgNumTrainingFile);
    f->addField("timestamp", fit::EnumType, eTrainingFileTimestamp, 0, 0, "");
    f->addField("type", fit::EnumType, eTrainingFileType, 0, 0, "");
    f->addField("manufacturer", fit::EnumType, eTrainingFileManufacturer, 0, 0, "");
    f->addField("product", fit::Uint16Type, eTrainingFileProduct, 0, 0, "");
    f->addSubfield("garmin_product", fit::EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fit::EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fit::EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", fit::Uint32zType, eTrainingFileSerialNumber, 0, 0, "");
    f->addField("time_created", fit::EnumType, eTrainingFileTimeCreated, 0, 0, "");
    profiles.insert(eMesgNumTrainingFile, f);
}

void initHrv(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hrv", eMesgNumHrv);
    f->addField("time", fit::Uint16Type, eHrvTime, 1000, 0, "s");
    profiles.insert(eMesgNumHrv, f);
}

void initWeatherConditions(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("weather_conditions", eMesgNumWeatherConditions);
    f->addField("timestamp", fit::EnumType, eWeatherConditionsTimestamp, 0, 0, "");
    f->addField("weather_report", fit::EnumType, eWeatherConditionsWeatherReport, 0, 0, "");
    f->addField("temperature", fit::Sint8Type, eWeatherConditionsTemperature, 0, 0, "C");
    f->addField("condition", fit::EnumType, eWeatherConditionsCondition, 0, 0, "");
    f->addField("wind_direction", fit::Uint16Type, eWeatherConditionsWindDirection, 0, 0, "degrees");
    f->addField("wind_speed", fit::Uint16Type, eWeatherConditionsWindSpeed, 1000, 0, "m/s");
    f->addField("precipitation_probability", fit::Uint8Type, eWeatherConditionsPrecipitationProbability, 0, 0, "");
    f->addField("temperature_feels_like", fit::Sint8Type, eWeatherConditionsTemperatureFeelsLike, 0, 0, "C");
    f->addField("relative_humidity", fit::Uint8Type, eWeatherConditionsRelativeHumidity, 0, 0, "");
    f->addField("location", fit::StringType, eWeatherConditionsLocation, 0, 0, "");
    f->addField("observed_at_time", fit::EnumType, eWeatherConditionsObservedAtTime, 0, 0, "");
    f->addField("observed_location_lat", fit::Sint32Type, eWeatherConditionsObservedLocationLat, 0, 0, "semicircles");
    f->addField("observed_location_long", fit::Sint32Type, eWeatherConditionsObservedLocationLong, 0, 0, "semicircles");
    f->addField("day_of_week", fit::EnumType, eWeatherConditionsDayOfWeek, 0, 0, "");
    f->addField("high_temperature", fit::Sint8Type, eWeatherConditionsHighTemperature, 0, 0, "C");
    f->addField("low_temperature", fit::Sint8Type, eWeatherConditionsLowTemperature, 0, 0, "C");
    profiles.insert(eMesgNumWeatherConditions, f);
}

void initWeatherAlert(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("weather_alert", eMesgNumWeatherAlert);
    f->addField("timestamp", fit::EnumType, eWeatherAlertTimestamp, 0, 0, "");
    f->addField("report_id", fit::StringType, eWeatherAlertReportId, 0, 0, "");
    f->addField("issue_time", fit::EnumType, eWeatherAlertIssueTime, 0, 0, "");
    f->addField("expire_time", fit::EnumType, eWeatherAlertExpireTime, 0, 0, "");
    f->addField("severity", fit::EnumType, eWeatherAlertSeverity, 0, 0, "");
    f->addField("type", fit::EnumType, eWeatherAlertType, 0, 0, "");
    profiles.insert(eMesgNumWeatherAlert, f);
}

void initGpsMetadata(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("gps_metadata", eMesgNumGpsMetadata);
    f->addField("timestamp", fit::EnumType, eGpsMetadataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eGpsMetadataTimestampMs, 0, 0, "ms");
    f->addField("position_lat", fit::Sint32Type, eGpsMetadataPositionLat, 0, 0, "semicircles");
    f->addField("position_long", fit::Sint32Type, eGpsMetadataPositionLong, 0, 0, "semicircles");
    f->addField("enhanced_altitude", fit::Uint32Type, eGpsMetadataEnhancedAltitude, 5, 500, "m");
    f->addField("enhanced_speed", fit::Uint32Type, eGpsMetadataEnhancedSpeed, 1000, 0, "m/s");
    f->addField("heading", fit::Uint16Type, eGpsMetadataHeading, 100, 0, "degrees");
    f->addField("utc_timestamp", fit::EnumType, eGpsMetadataUtcTimestamp, 0, 0, "s");
    f->addField("velocity", fit::Sint16Type, eGpsMetadataVelocity, 100, 0, "m/s");
    profiles.insert(eMesgNumGpsMetadata, f);
}

void initCameraEvent(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("camera_event", eMesgNumCameraEvent);
    f->addField("timestamp", fit::EnumType, eCameraEventTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eCameraEventTimestampMs, 0, 0, "ms");
    f->addField("camera_event_type", fit::EnumType, eCameraEventCameraEventType, 0, 0, "");
    f->addField("camera_file_uuid", fit::StringType, eCameraEventCameraFileUuid, 0, 0, "");
    f->addField("camera_orientation", fit::EnumType, eCameraEventCameraOrientation, 0, 0, "");
    profiles.insert(eMesgNumCameraEvent, f);
}

void initGyroscopeData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("gyroscope_data", eMesgNumGyroscopeData);
    f->addField("timestamp", fit::EnumType, eGyroscopeDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eGyroscopeDataTimestampMs, 0, 0, "ms");
    f->addField("sample_time_offset", fit::Uint16Type, eGyroscopeDataSampleTimeOffset, 0, 0, "ms");
    f->addField("gyro_x", fit::Uint16Type, eGyroscopeDataGyroX, 0, 0, "counts");
    f->addField("gyro_y", fit::Uint16Type, eGyroscopeDataGyroY, 0, 0, "counts");
    f->addField("gyro_z", fit::Uint16Type, eGyroscopeDataGyroZ, 0, 0, "counts");
    f->addField("calibrated_gyro_x", fit::Float32Type, eGyroscopeDataCalibratedGyroX, 0, 0, "deg/s");
    f->addField("calibrated_gyro_y", fit::Float32Type, eGyroscopeDataCalibratedGyroY, 0, 0, "deg/s");
    f->addField("calibrated_gyro_z", fit::Float32Type, eGyroscopeDataCalibratedGyroZ, 0, 0, "deg/s");
    profiles.insert(eMesgNumGyroscopeData, f);
}

void initAccelerometerData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("accelerometer_data", eMesgNumAccelerometerData);
    f->addField("timestamp", fit::EnumType, eAccelerometerDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eAccelerometerDataTimestampMs, 0, 0, "ms");
    f->addField("sample_time_offset", fit::Uint16Type, eAccelerometerDataSampleTimeOffset, 0, 0, "ms");
    f->addField("accel_x", fit::Uint16Type, eAccelerometerDataAccelX, 0, 0, "counts");
    f->addField("accel_y", fit::Uint16Type, eAccelerometerDataAccelY, 0, 0, "counts");
    f->addField("accel_z", fit::Uint16Type, eAccelerometerDataAccelZ, 0, 0, "counts");
    f->addField("calibrated_accel_x", fit::Float32Type, eAccelerometerDataCalibratedAccelX, 0, 0, "g");
    f->addField("calibrated_accel_y", fit::Float32Type, eAccelerometerDataCalibratedAccelY, 0, 0, "g");
    f->addField("calibrated_accel_z", fit::Float32Type, eAccelerometerDataCalibratedAccelZ, 0, 0, "g");
    f->addField("compressed_calibrated_accel_x", fit::Sint16Type, eAccelerometerDataCompressedCalibratedAccelX, 0, 0, "mG");
    f->addField("compressed_calibrated_accel_y", fit::Sint16Type, eAccelerometerDataCompressedCalibratedAccelY, 0, 0, "mG");
    f->addField("compressed_calibrated_accel_z", fit::Sint16Type, eAccelerometerDataCompressedCalibratedAccelZ, 0, 0, "mG");
    profiles.insert(eMesgNumAccelerometerData, f);
}

void initMagnetometerData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("magnetometer_data", eMesgNumMagnetometerData);
    f->addField("timestamp", fit::EnumType, eMagnetometerDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eMagnetometerDataTimestampMs, 0, 0, "ms");
    f->addField("sample_time_offset", fit::Uint16Type, eMagnetometerDataSampleTimeOffset, 0, 0, "ms");
    f->addField("mag_x", fit::Uint16Type, eMagnetometerDataMagX, 0, 0, "counts");
    f->addField("mag_y", fit::Uint16Type, eMagnetometerDataMagY, 0, 0, "counts");
    f->addField("mag_z", fit::Uint16Type, eMagnetometerDataMagZ, 0, 0, "counts");
    f->addField("calibrated_mag_x", fit::Float32Type, eMagnetometerDataCalibratedMagX, 0, 0, "G");
    f->addField("calibrated_mag_y", fit::Float32Type, eMagnetometerDataCalibratedMagY, 0, 0, "G");
    f->addField("calibrated_mag_z", fit::Float32Type, eMagnetometerDataCalibratedMagZ, 0, 0, "G");
    profiles.insert(eMesgNumMagnetometerData, f);
}

void initThreeDSensorCalibration(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("three_d_sensor_calibration", eMesgNumThreeDSensorCalibration);
    f->addField("timestamp", fit::EnumType, eThreeDSensorCalibrationTimestamp, 0, 0, "s");
    f->addField("sensor_type", fit::EnumType, eThreeDSensorCalibrationSensorType, 0, 0, "");
    f->addField("calibration_factor", fit::Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "");
    f->addSubfield("accel_cal_factor", fit::Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "g", eThreeDSensorCalibrationSensorType, eSensorTypeAccelerometer);
    f->addSubfield("gyro_cal_factor", fit::Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "deg/s", eThreeDSensorCalibrationSensorType, eSensorTypeGyroscope);
    f->addField("calibration_divisor", fit::Uint32Type, eThreeDSensorCalibrationCalibrationDivisor, 0, 0, "counts");
    f->addField("level_shift", fit::Uint32Type, eThreeDSensorCalibrationLevelShift, 0, 0, "");
    f->addField("offset_cal", fit::Sint32Type, eThreeDSensorCalibrationOffsetCal, 0, 0, "");
    f->addField("orientation_matrix", fit::Sint32Type, eThreeDSensorCalibrationOrientationMatrix, 65535, 0, "");
    profiles.insert(eMesgNumThreeDSensorCalibration, f);
}

void initVideoFrame(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_frame", eMesgNumVideoFrame);
    f->addField("timestamp", fit::EnumType, eVideoFrameTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eVideoFrameTimestampMs, 0, 0, "ms");
    f->addField("frame_number", fit::Uint32Type, eVideoFrameFrameNumber, 0, 0, "");
    profiles.insert(eMesgNumVideoFrame, f);
}

void initObdiiData(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("obdii_data", eMesgNumObdiiData);
    f->addField("timestamp", fit::EnumType, eObdiiDataTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eObdiiDataTimestampMs, 0, 0, "ms");
    f->addField("time_offset", fit::Uint16Type, eObdiiDataTimeOffset, 0, 0, "ms");
    f->addField("pid", fit::ByteType, eObdiiDataPid, 0, 0, "");
    f->addField("raw_data", fit::ByteType, eObdiiDataRawData, 0, 0, "");
    f->addField("pid_data_size", fit::Uint8Type, eObdiiDataPidDataSize, 0, 0, "");
    f->addField("system_time", fit::Uint32Type, eObdiiDataSystemTime, 0, 0, "");
    f->addField("start_timestamp", fit::EnumType, eObdiiDataStartTimestamp, 0, 0, "");
    f->addField("start_timestamp_ms", fit::Uint16Type, eObdiiDataStartTimestampMs, 0, 0, "ms");
    profiles.insert(eMesgNumObdiiData, f);
}

void initNmeaSentence(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("nmea_sentence", eMesgNumNmeaSentence);
    f->addField("timestamp", fit::EnumType, eNmeaSentenceTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eNmeaSentenceTimestampMs, 0, 0, "ms");
    f->addField("sentence", fit::StringType, eNmeaSentenceSentence, 0, 0, "");
    profiles.insert(eMesgNumNmeaSentence, f);
}

void initAviationAttitude(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("aviation_attitude", eMesgNumAviationAttitude);
    f->addField("timestamp", fit::EnumType, eAviationAttitudeTimestamp, 0, 0, "s");
    f->addField("timestamp_ms", fit::Uint16Type, eAviationAttitudeTimestampMs, 0, 0, "ms");
    f->addField("system_time", fit::Uint32Type, eAviationAttitudeSystemTime, 0, 0, "ms");
    f->addField("pitch", fit::Sint16Type, eAviationAttitudePitch, 10430.38, 0, "radians");
    f->addField("roll", fit::Sint16Type, eAviationAttitudeRoll, 10430.38, 0, "radians");
    f->addField("accel_lateral", fit::Sint16Type, eAviationAttitudeAccelLateral, 100, 0, "m/s^2");
    f->addField("accel_normal", fit::Sint16Type, eAviationAttitudeAccelNormal, 100, 0, "m/s^2");
    f->addField("turn_rate", fit::Sint16Type, eAviationAttitudeTurnRate, 1024, 0, "radians/second");
    f->addField("stage", fit::EnumType, eAviationAttitudeStage, 0, 0, "");
    f->addField("attitude_stage_complete", fit::Uint8Type, eAviationAttitudeAttitudeStageComplete, 0, 0, "%");
    f->addField("track", fit::Uint16Type, eAviationAttitudeTrack, 10430.38, 0, "radians");
    f->addField("validity", fit::EnumType, eAviationAttitudeValidity, 0, 0, "");
    profiles.insert(eMesgNumAviationAttitude, f);
}

void initVideo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video", eMesgNumVideo);
    f->addField("url", fit::StringType, eVideoUrl, 0, 0, "");
    f->addField("hosting_provider", fit::StringType, eVideoHostingProvider, 0, 0, "");
    f->addField("duration", fit::Uint32Type, eVideoDuration, 0, 0, "ms");
    profiles.insert(eMesgNumVideo, f);
}

void initVideoTitle(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_title", eMesgNumVideoTitle);
    f->addField("message_index", fit::EnumType, eVideoTitleMessageIndex, 0, 0, "");
    f->addField("message_count", fit::Uint16Type, eVideoTitleMessageCount, 0, 0, "");
    f->addField("text", fit::StringType, eVideoTitleText, 0, 0, "");
    profiles.insert(eMesgNumVideoTitle, f);
}

void initVideoDescription(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_description", eMesgNumVideoDescription);
    f->addField("message_index", fit::EnumType, eVideoDescriptionMessageIndex, 0, 0, "");
    f->addField("message_count", fit::Uint16Type, eVideoDescriptionMessageCount, 0, 0, "");
    f->addField("text", fit::StringType, eVideoDescriptionText, 0, 0, "");
    profiles.insert(eMesgNumVideoDescription, f);
}

void initVideoClip(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("video_clip", eMesgNumVideoClip);
    f->addField("clip_number", fit::Uint16Type, eVideoClipClipNumber, 0, 0, "");
    f->addField("start_timestamp", fit::EnumType, eVideoClipStartTimestamp, 0, 0, "");
    f->addField("start_timestamp_ms", fit::Uint16Type, eVideoClipStartTimestampMs, 0, 0, "");
    f->addField("end_timestamp", fit::EnumType, eVideoClipEndTimestamp, 0, 0, "");
    f->addField("end_timestamp_ms", fit::Uint16Type, eVideoClipEndTimestampMs, 0, 0, "");
    f->addField("clip_start", fit::Uint32Type, eVideoClipClipStart, 0, 0, "ms");
    f->addField("clip_end", fit::Uint32Type, eVideoClipClipEnd, 0, 0, "ms");
    profiles.insert(eMesgNumVideoClip, f);
}

void initCourse(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("course", eMesgNumCourse);
    f->addField("sport", fit::EnumType, eCourseSport, 0, 0, "");
    f->addField("name", fit::StringType, eCourseName, 0, 0, "");
    f->addField("capabilities", fit::EnumType, eCourseCapabilities, 0, 0, "");
    f->addField("sub_sport", fit::EnumType, eCourseSubSport, 0, 0, "");
    profiles.insert(eMesgNumCourse, f);
}

void initCoursePoint(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("course_point", eMesgNumCoursePoint);
    f->addField("message_index", fit::EnumType, eCoursePointMessageIndex, 0, 0, "");
    f->addField("timestamp", fit::EnumType, eCoursePointTimestamp, 0, 0, "");
    f->addField("position_lat", fit::Sint32Type, eCoursePointPositionLat, 0, 0, "semicircles");
    f->addField("position_long", fit::Sint32Type, eCoursePointPositionLong, 0, 0, "semicircles");
    f->addField("distance", fit::Uint32Type, eCoursePointDistance, 100, 0, "m");
    f->addField("type", fit::EnumType, eCoursePointType, 0, 0, "");
    f->addField("name", fit::StringType, eCoursePointName, 0, 0, "");
    f->addField("favorite", fit::EnumType, eCoursePointFavorite, 0, 0, "");
    profiles.insert(eMesgNumCoursePoint, f);
}

void initSegmentId(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_id", eMesgNumSegmentId);
    f->addField("name", fit::StringType, eSegmentIdName, 0, 0, "");
    f->addField("uuid", fit::StringType, eSegmentIdUuid, 0, 0, "");
    f->addField("sport", fit::EnumType, eSegmentIdSport, 0, 0, "");
    f->addField("enabled", fit::EnumType, eSegmentIdEnabled, 0, 0, "");
    f->addField("user_profile_primary_key", fit::Uint32Type, eSegmentIdUserProfilePrimaryKey, 0, 0, "");
    f->addField("device_id", fit::Uint32Type, eSegmentIdDeviceId, 0, 0, "");
    f->addField("default_race_leader", fit::Uint8Type, eSegmentIdDefaultRaceLeader, 0, 0, "");
    f->addField("delete_status", fit::EnumType, eSegmentIdDeleteStatus, 0, 0, "");
    f->addField("selection_type", fit::EnumType, eSegmentIdSelectionType, 0, 0, "");
    profiles.insert(eMesgNumSegmentId, f);
}

void initSegmentLeaderboardEntry(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_leaderboard_entry", eMesgNumSegmentLeaderboardEntry);
    f->addField("message_index", fit::EnumType, eSegmentLeaderboardEntryMessageIndex, 0, 0, "");
    f->addField("name", fit::StringType, eSegmentLeaderboardEntryName, 0, 0, "");
    f->addField("type", fit::EnumType, eSegmentLeaderboardEntryType, 0, 0, "");
    f->addField("group_primary_key", fit::Uint32Type, eSegmentLeaderboardEntryGroupPrimaryKey, 0, 0, "");
    f->addField("activity_id", fit::Uint32Type, eSegmentLeaderboardEntryActivityId, 0, 0, "");
    f->addField("segment_time", fit::Uint32Type, eSegmentLeaderboardEntrySegmentTime, 1000, 0, "s");
    f->addField("activity_id_string", fit::StringType, eSegmentLeaderboardEntryActivityIdString, 0, 0, "");
    profiles.insert(eMesgNumSegmentLeaderboardEntry, f);
}

void initSegmentPoint(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_point", eMesgNumSegmentPoint);
    f->addField("message_index", fit::EnumType, eSegmentPointMessageIndex, 0, 0, "");
    f->addField("position_lat", fit::Sint32Type, eSegmentPointPositionLat, 0, 0, "semicircles");
    f->addField("position_long", fit::Sint32Type, eSegmentPointPositionLong, 0, 0, "semicircles");
    f->addField("distance", fit::Uint32Type, eSegmentPointDistance, 100, 0, "m");
    f->addField("altitude", fit::Uint16Type, eSegmentPointAltitude, 5, 500, "m");
    f->addField("leader_time", fit::Uint32Type, eSegmentPointLeaderTime, 1000, 0, "s");
    profiles.insert(eMesgNumSegmentPoint, f);
}

void initSegmentLap(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_lap", eMesgNumSegmentLap);
    f->addField("message_index", fit::EnumType, eSegmentLapMessageIndex, 0, 0, "");
    f->addField("timestamp", fit::EnumType, eSegmentLapTimestamp, 0, 0, "s");
    f->addField("event", fit::EnumType, eSegmentLapEvent, 0, 0, "");
    f->addField("event_type", fit::EnumType, eSegmentLapEventType, 0, 0, "");
    f->addField("start_time", fit::EnumType, eSegmentLapStartTime, 0, 0, "");
    f->addField("start_position_lat", fit::Sint32Type, eSegmentLapStartPositionLat, 0, 0, "semicircles");
    f->addField("start_position_long", fit::Sint32Type, eSegmentLapStartPositionLong, 0, 0, "semicircles");
    f->addField("end_position_lat", fit::Sint32Type, eSegmentLapEndPositionLat, 0, 0, "semicircles");
    f->addField("end_position_long", fit::Sint32Type, eSegmentLapEndPositionLong, 0, 0, "semicircles");
    f->addField("total_elapsed_time", fit::Uint32Type, eSegmentLapTotalElapsedTime, 1000, 0, "s");
    f->addField("total_timer_time", fit::Uint32Type, eSegmentLapTotalTimerTime, 1000, 0, "s");
    f->addField("total_distance", fit::Uint32Type, eSegmentLapTotalDistance, 100, 0, "m");
    f->addField("total_cycles", fit::Uint32Type, eSegmentLapTotalCycles, 0, 0, "cycles");
    f->addSubfield("total_strokes", fit::Uint32Type, eSegmentLapTotalCycles, 0, 0, "strokes", eSegmentLapSport, eSportCycling);
    f->addField("total_calories", fit::Uint16Type, eSegmentLapTotalCalories, 0, 0, "kcal");
    f->addField("total_fat_calories", fit::Uint16Type, eSegmentLapTotalFatCalories, 0, 0, "kcal");
    f->addField("avg_speed", fit::Uint16Type, eSegmentLapAvgSpeed, 1000, 0, "m/s");
    f->addField("max_speed", fit::Uint16Type, eSegmentLapMaxSpeed, 1000, 0, "m/s");
    f->addField("avg_heart_rate", fit::Uint8Type, eSegmentLapAvgHeartRate, 0, 0, "bpm");
    f->addField("max_heart_rate", fit::Uint8Type, eSegmentLapMaxHeartRate, 0, 0, "bpm");
    f->addField("avg_cadence", fit::Uint8Type, eSegmentLapAvgCadence, 0, 0, "rpm");
    f->addField("max_cadence", fit::Uint8Type, eSegmentLapMaxCadence, 0, 0, "rpm");
    f->addField("avg_power", fit::Uint16Type, eSegmentLapAvgPower, 0, 0, "watts");
    f->addField("max_power", fit::Uint16Type, eSegmentLapMaxPower, 0, 0, "watts");
    f->addField("total_ascent", fit::Uint16Type, eSegmentLapTotalAscent, 0, 0, "m");
    f->addField("total_descent", fit::Uint16Type, eSegmentLapTotalDescent, 0, 0, "m");
    f->addField("sport", fit::EnumType, eSegmentLapSport, 0, 0, "");
    f->addField("event_group", fit::Uint8Type, eSegmentLapEventGroup, 0, 0, "");
    f->addField("nec_lat", fit::Sint32Type, eSegmentLapNecLat, 0, 0, "semicircles");
    f->addField("nec_long", fit::Sint32Type, eSegmentLapNecLong, 0, 0, "semicircles");
    f->addField("swc_lat", fit::Sint32Type, eSegmentLapSwcLat, 0, 0, "semicircles");
    f->addField("swc_long", fit::Sint32Type, eSegmentLapSwcLong, 0, 0, "semicircles");
    f->addField("name", fit::StringType, eSegmentLapName, 0, 0, "");
    f->addField("normalized_power", fit::Uint16Type, eSegmentLapNormalizedPower, 0, 0, "watts");
    f->addField("left_right_balance", fit::EnumType, eSegmentLapLeftRightBalance, 0, 0, "");
    f->addField("sub_sport", fit::EnumType, eSegmentLapSubSport, 0, 0, "");
    f->addField("total_work", fit::Uint32Type, eSegmentLapTotalWork, 0, 0, "J");
    f->addField("avg_altitude", fit::Uint16Type, eSegmentLapAvgAltitude, 5, 500, "m");
    f->addField("max_altitude", fit::Uint16Type, eSegmentLapMaxAltitude, 5, 500, "m");
    f->addField("gps_accuracy", fit::Uint8Type, eSegmentLapGpsAccuracy, 0, 0, "m");
    f->addField("avg_grade", fit::Sint16Type, eSegmentLapAvgGrade, 100, 0, "%");
    f->addField("avg_pos_grade", fit::Sint16Type, eSegmentLapAvgPosGrade, 100, 0, "%");
    f->addField("avg_neg_grade", fit::Sint16Type, eSegmentLapAvgNegGrade, 100, 0, "%");
    f->addField("max_pos_grade", fit::Sint16Type, eSegmentLapMaxPosGrade, 100, 0, "%");
    f->addField("max_neg_grade", fit::Sint16Type, eSegmentLapMaxNegGrade, 100, 0, "%");
    f->addField("avg_temperature", fit::Sint8Type, eSegmentLapAvgTemperature, 0, 0, "C");
    f->addField("max_temperature", fit::Sint8Type, eSegmentLapMaxTemperature, 0, 0, "C");
    f->addField("total_moving_time", fit::Uint32Type, eSegmentLapTotalMovingTime, 1000, 0, "s");
    f->addField("avg_pos_vertical_speed", fit::Sint16Type, eSegmentLapAvgPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("avg_neg_vertical_speed", fit::Sint16Type, eSegmentLapAvgNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_pos_vertical_speed", fit::Sint16Type, eSegmentLapMaxPosVerticalSpeed, 1000, 0, "m/s");
    f->addField("max_neg_vertical_speed", fit::Sint16Type, eSegmentLapMaxNegVerticalSpeed, 1000, 0, "m/s");
    f->addField("time_in_hr_zone", fit::Uint32Type, eSegmentLapTimeInHrZone, 1000, 0, "s");
    f->addField("time_in_speed_zone", fit::Uint32Type, eSegmentLapTimeInSpeedZone, 1000, 0, "s");
    f->addField("time_in_cadence_zone", fit::Uint32Type, eSegmentLapTimeInCadenceZone, 1000, 0, "s");
    f->addField("time_in_power_zone", fit::Uint32Type, eSegmentLapTimeInPowerZone, 1000, 0, "s");
    f->addField("repetition_num", fit::Uint16Type, eSegmentLapRepetitionNum, 0, 0, "");
    f->addField("min_altitude", fit::Uint16Type, eSegmentLapMinAltitude, 5, 500, "m");
    f->addField("min_heart_rate", fit::Uint8Type, eSegmentLapMinHeartRate, 0, 0, "bpm");
    f->addField("active_time", fit::Uint32Type, eSegmentLapActiveTime, 1000, 0, "s");
    f->addField("wkt_step_index", fit::EnumType, eSegmentLapWktStepIndex, 0, 0, "");
    f->addField("sport_event", fit::EnumType, eSegmentLapSportEvent, 0, 0, "");
    f->addField("avg_left_torque_effectiveness", fit::Uint8Type, eSegmentLapAvgLeftTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_right_torque_effectiveness", fit::Uint8Type, eSegmentLapAvgRightTorqueEffectiveness, 2, 0, "percent");
    f->addField("avg_left_pedal_smoothness", fit::Uint8Type, eSegmentLapAvgLeftPedalSmoothness, 2, 0, "percent");
    f->addField("avg_right_pedal_smoothness", fit::Uint8Type, eSegmentLapAvgRightPedalSmoothness, 2, 0, "percent");
    f->addField("avg_combined_pedal_smoothness", fit::Uint8Type, eSegmentLapAvgCombinedPedalSmoothness, 2, 0, "percent");
    f->addField("status", fit::EnumType, eSegmentLapStatus, 0, 0, "");
    f->addField("uuid", fit::StringType, eSegmentLapUuid, 0, 0, "");
    f->addField("avg_fractional_cadence", fit::Uint8Type, eSegmentLapAvgFractionalCadence, 128, 0, "rpm");
    f->addField("max_fractional_cadence", fit::Uint8Type, eSegmentLapMaxFractionalCadence, 128, 0, "rpm");
    f->addField("total_fractional_cycles", fit::Uint8Type, eSegmentLapTotalFractionalCycles, 128, 0, "cycles");
    f->addField("front_gear_shift_count", fit::Uint16Type, eSegmentLapFrontGearShiftCount, 0, 0, "");
    f->addField("rear_gear_shift_count", fit::Uint16Type, eSegmentLapRearGearShiftCount, 0, 0, "");
    f->addField("time_standing", fit::Uint32Type, eSegmentLapTimeStanding, 1000, 0, "s");
    f->addField("stand_count", fit::Uint16Type, eSegmentLapStandCount, 0, 0, "");
    f->addField("avg_left_pco", fit::Sint8Type, eSegmentLapAvgLeftPco, 0, 0, "mm");
    f->addField("avg_right_pco", fit::Sint8Type, eSegmentLapAvgRightPco, 0, 0, "mm");
    f->addField("avg_left_power_phase", fit::Uint8Type, eSegmentLapAvgLeftPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_left_power_phase_peak", fit::Uint8Type, eSegmentLapAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase", fit::Uint8Type, eSegmentLapAvgRightPowerPhase, 0.7111111, 0, "degrees");
    f->addField("avg_right_power_phase_peak", fit::Uint8Type, eSegmentLapAvgRightPowerPhasePeak, 0.7111111, 0, "degrees");
    f->addField("avg_power_position", fit::Uint16Type, eSegmentLapAvgPowerPosition, 0, 0, "watts");
    f->addField("max_power_position", fit::Uint16Type, eSegmentLapMaxPowerPosition, 0, 0, "watts");
    f->addField("avg_cadence_position", fit::Uint8Type, eSegmentLapAvgCadencePosition, 0, 0, "rpm");
    f->addField("max_cadence_position", fit::Uint8Type, eSegmentLapMaxCadencePosition, 0, 0, "rpm");
    f->addField("manufacturer", fit::EnumType, eSegmentLapManufacturer, 0, 0, "");
    profiles.insert(eMesgNumSegmentLap, f);
}

void initSegmentFile(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("segment_file", eMesgNumSegmentFile);
    f->addField("message_index", fit::EnumType, eSegmentFileMessageIndex, 0, 0, "");
    f->addField("file_uuid", fit::StringType, eSegmentFileFileUuid, 0, 0, "");
    f->addField("enabled", fit::EnumType, eSegmentFileEnabled, 0, 0, "");
    f->addField("user_profile_primary_key", fit::Uint32Type, eSegmentFileUserProfilePrimaryKey, 0, 0, "");
    f->addField("leader_type", fit::EnumType, eSegmentFileLeaderType, 0, 0, "");
    f->addField("leader_group_primary_key", fit::Uint32Type, eSegmentFileLeaderGroupPrimaryKey, 0, 0, "");
    f->addField("leader_activity_id", fit::Uint32Type, eSegmentFileLeaderActivityId, 0, 0, "");
    f->addField("leader_activity_id_string", fit::StringType, eSegmentFileLeaderActivityIdString, 0, 0, "");
    f->addField("default_race_leader", fit::Uint8Type, eSegmentFileDefaultRaceLeader, 0, 0, "");
    profiles.insert(eMesgNumSegmentFile, f);
}

void initWorkout(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("workout", eMesgNumWorkout);
    f->addField("sport", fit::EnumType, eWorkoutSport, 0, 0, "");
    f->addField("capabilities", fit::EnumType, eWorkoutCapabilities, 0, 0, "");
    f->addField("num_valid_steps", fit::Uint16Type, eWorkoutNumValidSteps, 0, 0, "");
    f->addField("wkt_name", fit::StringType, eWorkoutWktName, 0, 0, "");
    profiles.insert(eMesgNumWorkout, f);
}

void initWorkoutStep(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("workout_step", eMesgNumWorkoutStep);
    f->addField("message_index", fit::EnumType, eWorkoutStepMessageIndex, 0, 0, "");
    f->addField("wkt_step_name", fit::StringType, eWorkoutStepWktStepName, 0, 0, "");
    f->addField("duration_type", fit::EnumType, eWorkoutStepDurationType, 0, 0, "");
    f->addField("duration_value", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "");
    f->addSubfield("duration_time", fit::Uint32Type, eWorkoutStepDurationValue, 1000, 0, "s", eWorkoutStepDurationType, eWktStepDurationTime);
    f->addSubfield("duration_time", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepetitionTime);
    f->addSubfield("duration_distance", fit::Uint32Type, eWorkoutStepDurationValue, 100, 0, "m", eWorkoutStepDurationType, eWktStepDurationDistance);
    f->addSubfield("duration_hr", fit::EnumType, eWorkoutStepDurationValue, 0, 0, "% or bpm", eWorkoutStepDurationType, eWktStepDurationHrLessThan);
    f->addSubfield("duration_hr", fit::EnumType, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationHrGreaterThan);
    f->addSubfield("duration_calories", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "calories", eWorkoutStepDurationType, eWktStepDurationCalories);
    f->addSubfield("duration_step", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilStepsCmplt);
    f->addSubfield("duration_step", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilTime);
    f->addSubfield("duration_step", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilDistance);
    f->addSubfield("duration_step", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilCalories);
    f->addSubfield("duration_step", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrLessThan);
    f->addSubfield("duration_step", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrGreaterThan);
    f->addSubfield("duration_step", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerLessThan);
    f->addSubfield("duration_step", fit::Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerGreaterThan);
    f->addSubfield("duration_power", fit::EnumType, eWorkoutStepDurationValue, 0, 0, "% or watts", eWorkoutStepDurationType, eWktStepDurationPowerLessThan);
    f->addSubfield("duration_power", fit::EnumType, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationPowerGreaterThan);
    f->addField("target_type", fit::EnumType, eWorkoutStepTargetType, 0, 0, "");
    f->addField("target_value", fit::Uint32Type, eWorkoutStepTargetValue, 0, 0, "");
    f->addSubfield("target_speed_zone", fit::Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetSpeed);
    f->addSubfield("target_hr_zone", fit::Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("target_cadence_zone", fit::Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetCadence);
    f->addSubfield("target_power_zone", fit::Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addSubfield("repeat_steps", fit::Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilStepsCmplt);
    f->addSubfield("repeat_time", fit::Uint32Type, eWorkoutStepTargetValue, 1000, 0, "s", eWorkoutStepDurationType, eWktStepDurationRepeatUntilTime);
    f->addSubfield("repeat_distance", fit::Uint32Type, eWorkoutStepTargetValue, 100, 0, "m", eWorkoutStepDurationType, eWktStepDurationRepeatUntilDistance);
    f->addSubfield("repeat_calories", fit::Uint32Type, eWorkoutStepTargetValue, 0, 0, "calories", eWorkoutStepDurationType, eWktStepDurationRepeatUntilCalories);
    f->addSubfield("repeat_hr", fit::EnumType, eWorkoutStepTargetValue, 0, 0, "% or bpm", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrLessThan);
    f->addSubfield("repeat_hr", fit::EnumType, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrGreaterThan);
    f->addSubfield("repeat_power", fit::EnumType, eWorkoutStepTargetValue, 0, 0, "% or watts", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerLessThan);
    f->addSubfield("repeat_power", fit::EnumType, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerGreaterThan);
    f->addField("custom_target_value_low", fit::Uint32Type, eWorkoutStepCustomTargetValueLow, 0, 0, "");
    f->addSubfield("custom_target_speed_low", fit::Uint32Type, eWorkoutStepCustomTargetValueLow, 1000, 0, "m/s", eWorkoutStepTargetType, eWktStepTargetSpeed);
    f->addSubfield("custom_target_heart_rate_low", fit::EnumType, eWorkoutStepCustomTargetValueLow, 0, 0, "% or bpm", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("custom_target_cadence_low", fit::Uint32Type, eWorkoutStepCustomTargetValueLow, 0, 0, "rpm", eWorkoutStepTargetType, eWktStepTargetCadence);
    f->addSubfield("custom_target_power_low", fit::EnumType, eWorkoutStepCustomTargetValueLow, 0, 0, "% or watts", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addField("custom_target_value_high", fit::Uint32Type, eWorkoutStepCustomTargetValueHigh, 0, 0, "");
    f->addSubfield("custom_target_speed_high", fit::Uint32Type, eWorkoutStepCustomTargetValueHigh, 1000, 0, "m/s", eWorkoutStepTargetType, eWktStepTargetSpeed);
    f->addSubfield("custom_target_heart_rate_high", fit::EnumType, eWorkoutStepCustomTargetValueHigh, 0, 0, "% or bpm", eWorkoutStepTargetType, eWktStepTargetHeartRate);
    f->addSubfield("custom_target_cadence_high", fit::Uint32Type, eWorkoutStepCustomTargetValueHigh, 0, 0, "rpm", eWorkoutStepTargetType, eWktStepTargetCadence);
    f->addSubfield("custom_target_power_high", fit::EnumType, eWorkoutStepCustomTargetValueHigh, 0, 0, "% or watts", eWorkoutStepTargetType, eWktStepTargetPower);
    f->addField("intensity", fit::EnumType, eWorkoutStepIntensity, 0, 0, "");
    f->addField("notes", fit::StringType, eWorkoutStepNotes, 0, 0, "");
    profiles.insert(eMesgNumWorkoutStep, f);
}

void initSchedule(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("schedule", eMesgNumSchedule);
    f->addField("manufacturer", fit::EnumType, eScheduleManufacturer, 0, 0, "");
    f->addField("product", fit::Uint16Type, eScheduleProduct, 0, 0, "");
    f->addSubfield("garmin_product", fit::EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerGarmin);
    f->addSubfield("garmin_product", fit::EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerDynastream);
    f->addSubfield("garmin_product", fit::EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerDynastreamOem);
    f->addField("serial_number", fit::Uint32zType, eScheduleSerialNumber, 0, 0, "");
    f->addField("time_created", fit::EnumType, eScheduleTimeCreated, 0, 0, "");
    f->addField("completed", fit::EnumType, eScheduleCompleted, 0, 0, "");
    f->addField("type", fit::EnumType, eScheduleType, 0, 0, "");
    f->addField("scheduled_time", fit::EnumType, eScheduleScheduledTime, 0, 0, "");
    profiles.insert(eMesgNumSchedule, f);
}

void initTotals(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("totals", eMesgNumTotals);
    f->addField("message_index", fit::EnumType, eTotalsMessageIndex, 0, 0, "");
    f->addField("timestamp", fit::EnumType, eTotalsTimestamp, 0, 0, "s");
    f->addField("timer_time", fit::Uint32Type, eTotalsTimerTime, 0, 0, "s");
    f->addField("distance", fit::Uint32Type, eTotalsDistance, 0, 0, "m");
    f->addField("calories", fit::Uint32Type, eTotalsCalories, 0, 0, "kcal");
    f->addField("sport", fit::EnumType, eTotalsSport, 0, 0, "");
    f->addField("elapsed_time", fit::Uint32Type, eTotalsElapsedTime, 0, 0, "s");
    f->addField("sessions", fit::Uint16Type, eTotalsSessions, 0, 0, "");
    f->addField("active_time", fit::Uint32Type, eTotalsActiveTime, 0, 0, "s");
    f->addField("sport_index", fit::Uint8Type, eTotalsSportIndex, 0, 0, "");
    profiles.insert(eMesgNumTotals, f);
}

void initWeightScale(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("weight_scale", eMesgNumWeightScale);
    f->addField("timestamp", fit::EnumType, eWeightScaleTimestamp, 0, 0, "s");
    f->addField("weight", fit::EnumType, eWeightScaleWeight, 100, 0, "kg");
    f->addField("percent_fat", fit::Uint16Type, eWeightScalePercentFat, 100, 0, "%");
    f->addField("percent_hydration", fit::Uint16Type, eWeightScalePercentHydration, 100, 0, "%");
    f->addField("visceral_fat_mass", fit::Uint16Type, eWeightScaleVisceralFatMass, 100, 0, "kg");
    f->addField("bone_mass", fit::Uint16Type, eWeightScaleBoneMass, 100, 0, "kg");
    f->addField("muscle_mass", fit::Uint16Type, eWeightScaleMuscleMass, 100, 0, "kg");
    f->addField("basal_met", fit::Uint16Type, eWeightScaleBasalMet, 4, 0, "kcal/day");
    f->addField("physique_rating", fit::Uint8Type, eWeightScalePhysiqueRating, 0, 0, "");
    f->addField("active_met", fit::Uint16Type, eWeightScaleActiveMet, 4, 0, "kcal/day");
    f->addField("metabolic_age", fit::Uint8Type, eWeightScaleMetabolicAge, 0, 0, "years");
    f->addField("visceral_fat_rating", fit::Uint8Type, eWeightScaleVisceralFatRating, 0, 0, "");
    f->addField("user_profile_index", fit::EnumType, eWeightScaleUserProfileIndex, 0, 0, "");
    profiles.insert(eMesgNumWeightScale, f);
}

void initBloodPressure(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("blood_pressure", eMesgNumBloodPressure);
    f->addField("timestamp", fit::EnumType, eBloodPressureTimestamp, 0, 0, "s");
    f->addField("systolic_pressure", fit::Uint16Type, eBloodPressureSystolicPressure, 0, 0, "mmHg");
    f->addField("diastolic_pressure", fit::Uint16Type, eBloodPressureDiastolicPressure, 0, 0, "mmHg");
    f->addField("mean_arterial_pressure", fit::Uint16Type, eBloodPressureMeanArterialPressure, 0, 0, "mmHg");
    f->addField("map_3_sample_mean", fit::Uint16Type, eBloodPressureMap3SampleMean, 0, 0, "mmHg");
    f->addField("map_morning_values", fit::Uint16Type, eBloodPressureMapMorningValues, 0, 0, "mmHg");
    f->addField("map_evening_values", fit::Uint16Type, eBloodPressureMapEveningValues, 0, 0, "mmHg");
    f->addField("heart_rate", fit::Uint8Type, eBloodPressureHeartRate, 0, 0, "bpm");
    f->addField("heart_rate_type", fit::EnumType, eBloodPressureHeartRateType, 0, 0, "");
    f->addField("status", fit::EnumType, eBloodPressureStatus, 0, 0, "");
    f->addField("user_profile_index", fit::EnumType, eBloodPressureUserProfileIndex, 0, 0, "");
    profiles.insert(eMesgNumBloodPressure, f);
}

void initMonitoringInfo(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("monitoring_info", eMesgNumMonitoringInfo);
    f->addField("timestamp", fit::EnumType, eMonitoringInfoTimestamp, 0, 0, "s");
    f->addField("local_timestamp", fit::EnumType, eMonitoringInfoLocalTimestamp, 0, 0, "s");
    f->addField("activity_type", fit::EnumType, eMonitoringInfoActivityType, 0, 0, "");
    f->addField("cycles_to_distance", fit::Uint16Type, eMonitoringInfoCyclesToDistance, 5000, 0, "m/cycle");
    f->addField("cycles_to_calories", fit::Uint16Type, eMonitoringInfoCyclesToCalories, 5000, 0, "kcal/cycle");
    f->addField("resting_metabolic_rate", fit::Uint16Type, eMonitoringInfoRestingMetabolicRate, 0, 0, "kcal / day");
    profiles.insert(eMesgNumMonitoringInfo, f);
}

void initMonitoring(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("monitoring", eMesgNumMonitoring);
    f->addField("timestamp", fit::EnumType, eMonitoringTimestamp, 0, 0, "s");
    f->addField("device_index", fit::EnumType, eMonitoringDeviceIndex, 0, 0, "");
    f->addField("calories", fit::Uint16Type, eMonitoringCalories, 0, 0, "kcal");
    f->addField("distance", fit::Uint32Type, eMonitoringDistance, 100, 0, "m");
    f->addField("cycles", fit::Uint32Type, eMonitoringCycles, 2, 0, "cycles");
    f->addSubfield("steps", fit::Uint32Type, eMonitoringCycles, 1, 0, "steps", eMonitoringActivityType, eActivityTypeWalking);
    f->addSubfield("steps", fit::Uint32Type, eMonitoringCycles, 0, 0, "", eMonitoringActivityType, eActivityTypeRunning);
    f->addSubfield("strokes", fit::Uint32Type, eMonitoringCycles, 2, 0, "strokes", eMonitoringActivityType, eActivityTypeCycling);
    f->addSubfield("strokes", fit::Uint32Type, eMonitoringCycles, 0, 0, "", eMonitoringActivityType, eActivityTypeSwimming);
    f->addField("active_time", fit::Uint32Type, eMonitoringActiveTime, 1000, 0, "s");
    f->addField("activity_type", fit::EnumType, eMonitoringActivityType, 0, 0, "");
    f->addField("activity_subtype", fit::EnumType, eMonitoringActivitySubtype, 0, 0, "");
    f->addField("activity_level", fit::EnumType, eMonitoringActivityLevel, 0, 0, "");
    f->addField("distance_16", fit::Uint16Type, eMonitoringDistance16, 0, 0, "100 * m");
    f->addField("cycles_16", fit::Uint16Type, eMonitoringCycles16, 0, 0, "2 * cycles (steps)");
    f->addField("active_time_16", fit::Uint16Type, eMonitoringActiveTime16, 0, 0, "s");
    f->addField("local_timestamp", fit::EnumType, eMonitoringLocalTimestamp, 0, 0, "");
    f->addField("temperature", fit::Sint16Type, eMonitoringTemperature, 100, 0, "C");
    f->addField("temperature_min", fit::Sint16Type, eMonitoringTemperatureMin, 100, 0, "C");
    f->addField("temperature_max", fit::Sint16Type, eMonitoringTemperatureMax, 100, 0, "C");
    f->addField("activity_time", fit::Uint16Type, eMonitoringActivityTime, 0, 0, "minutes");
    f->addField("active_calories", fit::Uint16Type, eMonitoringActiveCalories, 0, 0, "kcal");
    f->addField("current_activity_type_intensity", fit::ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "");
    f->addComponent("current_activity_type_intensity", fit::ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "", eMonitoringActivityType, 53);
    f->addComponent("current_activity_type_intensity", fit::ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "", eMonitoringIntensity, 0);
    f->addField("timestamp_min_8", fit::Uint8Type, eMonitoringTimestampMin8, 0, 0, "min");
    f->addField("timestamp_16", fit::Uint16Type, eMonitoringTimestamp16, 0, 0, "s");
    f->addField("heart_rate", fit::Uint8Type, eMonitoringHeartRate, 0, 0, "bpm");
    f->addField("intensity", fit::Uint8Type, eMonitoringIntensity, 10, 0, "");
    f->addField("duration_min", fit::Uint16Type, eMonitoringDurationMin, 0, 0, "min");
    f->addField("duration", fit::Uint32Type, eMonitoringDuration, 0, 0, "s");
    f->addField("ascent", fit::Uint32Type, eMonitoringAscent, 1000, 0, "m");
    f->addField("descent", fit::Uint32Type, eMonitoringDescent, 1000, 0, "m");
    f->addField("moderate_activity_minutes", fit::Uint16Type, eMonitoringModerateActivityMinutes, 0, 0, "minutes");
    f->addField("vigorous_activity_minutes", fit::Uint16Type, eMonitoringVigorousActivityMinutes, 0, 0, "minutes");
    profiles.insert(eMesgNumMonitoring, f);
}

void initHr(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("hr", eMesgNumHr);
    f->addField("timestamp", fit::EnumType, eHrTimestamp, 0, 0, "");
    f->addField("fractional_timestamp", fit::Uint16Type, eHrFractionalTimestamp, 32768, 0, "s");
    f->addField("time256", fit::Uint8Type, eHrTime256, 0, 0, "");
    f->addComponent("time256", fit::Uint8Type, eHrTime256, 256, 0, "s", eHrFractionalTimestamp, 8);
    f->addField("filtered_bpm", fit::Uint8Type, eHrFilteredBpm, 0, 0, "bpm");
    f->addField("event_timestamp", fit::Uint32Type, eHrEventTimestamp, 1024, 0, "s");
    f->addField("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 0, 0, "");
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    f->addComponent("event_timestamp_12", fit::ByteType, eHrEventTimestamp12, 1024, 0, "s", eHrEventTimestamp, 12);
    profiles.insert(eMesgNumHr, f);
}

void initMemoGlob(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("memo_glob", eMesgNumMemoGlob);
    f->addField("part_index", fit::Uint32Type, eMemoGlobPartIndex, 0, 0, "");
    f->addField("memo", fit::ByteType, eMemoGlobMemo, 0, 0, "");
    f->addField("message_number", fit::Uint16Type, eMemoGlobMessageNumber, 0, 0, "");
    f->addField("message_index", fit::EnumType, eMemoGlobMessageIndex, 0, 0, "");
    profiles.insert(eMesgNumMemoGlob, f);
}

void initAntChannelId(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("ant_channel_id", eMesgNumAntChannelId);
    f->addField("channel_number", fit::Uint8Type, eAntChannelIdChannelNumber, 0, 0, "");
    f->addField("device_type", fit::Uint8zType, eAntChannelIdDeviceType, 0, 0, "");
    f->addField("device_number", fit::Uint16zType, eAntChannelIdDeviceNumber, 0, 0, "");
    f->addField("transmission_type", fit::Uint8zType, eAntChannelIdTransmissionType, 0, 0, "");
    f->addField("device_index", fit::EnumType, eAntChannelIdDeviceIndex, 0, 0, "");
    profiles.insert(eMesgNumAntChannelId, f);
}

void initAntRx(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("ant_rx", eMesgNumAntRx);
    f->addField("timestamp", fit::EnumType, eAntRxTimestamp, 0, 0, "s");
    f->addField("fractional_timestamp", fit::Uint16Type, eAntRxFractionalTimestamp, 32768, 0, "s");
    f->addField("mesg_id", fit::ByteType, eAntRxMesgId, 0, 0, "");
    f->addField("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "");
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxChannelNumber, 888888888);
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntRxMesgData, 0, 0, "", eAntRxData, 0);
    f->addField("channel_number", fit::Uint8Type, eAntRxChannelNumber, 0, 0, "");
    f->addField("data", fit::ByteType, eAntRxData, 0, 0, "");
    profiles.insert(eMesgNumAntRx, f);
}

void initAntTx(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("ant_tx", eMesgNumAntTx);
    f->addField("timestamp", fit::EnumType, eAntTxTimestamp, 0, 0, "s");
    f->addField("fractional_timestamp", fit::Uint16Type, eAntTxFractionalTimestamp, 32768, 0, "s");
    f->addField("mesg_id", fit::ByteType, eAntTxMesgId, 0, 0, "");
    f->addField("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "");
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxChannelNumber, 888888888);
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxData, 0);
    f->addComponent("mesg_data", fit::ByteType, eAntTxMesgData, 0, 0, "", eAntTxData, 0);
    f->addField("channel_number", fit::Uint8Type, eAntTxChannelNumber, 0, 0, "");
    f->addField("data", fit::ByteType, eAntTxData, 0, 0, "");
    profiles.insert(eMesgNumAntTx, f);
}

void initExdScreenConfiguration(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("exd_screen_configuration", eMesgNumExdScreenConfiguration);
    f->addField("screen_index", fit::Uint8Type, eExdScreenConfigurationScreenIndex, 0, 0, "");
    f->addField("field_count", fit::Uint8Type, eExdScreenConfigurationFieldCount, 0, 0, "");
    f->addField("layout", fit::EnumType, eExdScreenConfigurationLayout, 0, 0, "");
    f->addField("screen_enabled", fit::EnumType, eExdScreenConfigurationScreenEnabled, 0, 0, "");
    profiles.insert(eMesgNumExdScreenConfiguration, f);
}

void initExdDataFieldConfiguration(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("exd_data_field_configuration", eMesgNumExdDataFieldConfiguration);
    f->addField("screen_index", fit::Uint8Type, eExdDataFieldConfigurationScreenIndex, 0, 0, "");
    f->addField("concept_field", fit::ByteType, eExdDataFieldConfigurationConceptField, 0, 0, "");
    f->addComponent("concept_field", fit::ByteType, eExdDataFieldConfigurationConceptField, 0, 0, "", eExdDataFieldConfigurationFieldId, 44);
    f->addComponent("concept_field", fit::ByteType, eExdDataFieldConfigurationConceptField, 0, 0, "", eExdDataFieldConfigurationConceptCount, 0);
    f->addField("field_id", fit::Uint8Type, eExdDataFieldConfigurationFieldId, 0, 0, "");
    f->addField("concept_count", fit::Uint8Type, eExdDataFieldConfigurationConceptCount, 0, 0, "");
    f->addField("display_type", fit::EnumType, eExdDataFieldConfigurationDisplayType, 0, 0, "");
    f->addField("title", fit::StringType, eExdDataFieldConfigurationTitle, 0, 0, "");
    profiles.insert(eMesgNumExdDataFieldConfiguration, f);
}

void initExdDataConceptConfiguration(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("exd_data_concept_configuration", eMesgNumExdDataConceptConfiguration);
    f->addField("screen_index", fit::Uint8Type, eExdDataConceptConfigurationScreenIndex, 0, 0, "");
    f->addField("concept_field", fit::ByteType, eExdDataConceptConfigurationConceptField, 0, 0, "");
    f->addComponent("concept_field", fit::ByteType, eExdDataConceptConfigurationConceptField, 0, 0, "", eExdDataConceptConfigurationFieldId, 44);
    f->addComponent("concept_field", fit::ByteType, eExdDataConceptConfigurationConceptField, 0, 0, "", eExdDataConceptConfigurationConceptIndex, 0);
    f->addField("field_id", fit::Uint8Type, eExdDataConceptConfigurationFieldId, 0, 0, "");
    f->addField("concept_index", fit::Uint8Type, eExdDataConceptConfigurationConceptIndex, 0, 0, "");
    f->addField("data_page", fit::Uint8Type, eExdDataConceptConfigurationDataPage, 0, 0, "");
    f->addField("concept_key", fit::Uint8Type, eExdDataConceptConfigurationConceptKey, 0, 0, "");
    f->addField("scaling", fit::Uint8Type, eExdDataConceptConfigurationScaling, 0, 0, "");
    f->addField("data_units", fit::EnumType, eExdDataConceptConfigurationDataUnits, 0, 0, "");
    f->addField("qualifier", fit::EnumType, eExdDataConceptConfigurationQualifier, 0, 0, "");
    f->addField("descriptor", fit::EnumType, eExdDataConceptConfigurationDescriptor, 0, 0, "");
    f->addField("is_signed", fit::EnumType, eExdDataConceptConfigurationIsSigned, 0, 0, "");
    profiles.insert(eMesgNumExdDataConceptConfiguration, f);
}

void initFieldDescription(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("field_description", eMesgNumFieldDescription);
    f->addField("developer_data_index", fit::Uint8Type, eFieldDescriptionDeveloperDataIndex, 0, 0, "");
    f->addField("field_definition_number", fit::Uint8Type, eFieldDescriptionFieldDefinitionNumber, 0, 0, "");
    f->addField("fit_base_type_id", fit::EnumType, eFieldDescriptionFitBaseTypeId, 0, 0, "");
    f->addField("field_name", fit::StringType, eFieldDescriptionFieldName, 0, 0, "");
    f->addField("array", fit::Uint8Type, eFieldDescriptionArray, 0, 0, "");
    f->addField("components", fit::StringType, eFieldDescriptionComponents, 0, 0, "");
    f->addField("scale", fit::Uint8Type, eFieldDescriptionScale, 0, 0, "");
    f->addField("offset", fit::Sint8Type, eFieldDescriptionOffset, 0, 0, "");
    f->addField("units", fit::StringType, eFieldDescriptionUnits, 0, 0, "");
    f->addField("bits", fit::StringType, eFieldDescriptionBits, 0, 0, "");
    f->addField("accumulate", fit::StringType, eFieldDescriptionAccumulate, 0, 0, "");
    f->addField("fit_base_unit_id", fit::EnumType, eFieldDescriptionFitBaseUnitId, 0, 0, "");
    f->addField("native_mesg_num", fit::EnumType, eFieldDescriptionNativeMesgNum, 0, 0, "");
    f->addField("native_field_num", fit::Uint8Type, eFieldDescriptionNativeFieldNum, 0, 0, "");
    profiles.insert(eMesgNumFieldDescription, f);
}

void initDeveloperDataId(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("developer_data_id", eMesgNumDeveloperDataId);
    f->addField("developer_id", fit::ByteType, eDeveloperDataIdDeveloperId, 0, 0, "");
    f->addField("application_id", fit::ByteType, eDeveloperDataIdApplicationId, 0, 0, "");
    f->addField("manufacturer_id", fit::EnumType, eDeveloperDataIdManufacturerId, 0, 0, "");
    f->addField("developer_data_index", fit::Uint8Type, eDeveloperDataIdDeveloperDataIndex, 0, 0, "");
    f->addField("application_version", fit::Uint32Type, eDeveloperDataIdApplicationVersion, 0, 0, "");
    profiles.insert(eMesgNumDeveloperDataId, f);
}

// ----------- end generated code -----------

void initExtProfiles(QMap<quint16, CFitProfile*>& profiles)
{
    CFitProfile* f = new CFitProfile("location", eMesgNumLocation);
    f->addField("name", fit::StringType, eLocationName, 0, 0, "");
    f->addField("timestamp", fit::Uint32Type, eLocationTimestamp, 0, 0, "s");
    f->addField("index", fit::Uint16Type, eLocationMessageIndex, 0, 0, "");
    f->addField("position_lat", fit::Sint32Type, eLocationPositionLat, 0, 0, "semicircles");
    f->addField("position_long", fit::Sint32Type, eLocationPositionLong, 0, 0, "semicircles");
    f->addField("symbol", fit::Uint32Type, eLocationSymbol, 0, 0, "");
    f->addField("altitude", fit::Uint16Type, eLocationAltitude, 5, 500, "m");
    f->addField("???", fit::Uint16Type, eLocation5, 0, 0, "");
    f->addField("comment", fit::StringType, eLocationComment, 0, 0, "");
    profiles.insert(eMesgNumLocation, f);
}


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
    initConnectivity(allProfiles);
    initWatchfaceSettings(allProfiles);
    initOhrSettings(allProfiles);
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
    initWeatherConditions(allProfiles);
    initWeatherAlert(allProfiles);
    initGpsMetadata(allProfiles);
    initCameraEvent(allProfiles);
    initGyroscopeData(allProfiles);
    initAccelerometerData(allProfiles);
    initMagnetometerData(allProfiles);
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
    initHr(allProfiles);
    initMemoGlob(allProfiles);
    initAntChannelId(allProfiles);
    initAntRx(allProfiles);
    initAntTx(allProfiles);
    initExdScreenConfiguration(allProfiles);
    initExdDataFieldConfiguration(allProfiles);
    initExdDataConceptConfiguration(allProfiles);
    initFieldDescription(allProfiles);
    initDeveloperDataId(allProfiles);

    initExtProfiles(allProfiles);

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