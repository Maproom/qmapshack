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

#include "gis/fit/defs/CFitProfileLockup.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_fields.h"


// ----------- start generated code -----------
void initFileId(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumFileId] = new CFitProfile("file_id", eMesgNumFileId);
    profiles[eMesgNumFileId]->addField(new CFitFieldProfile("type", &EnumType, eFileIdType, 0, 0, ""));
    profiles[eMesgNumFileId]->addField(new CFitFieldProfile("manufacturer", &EnumType, eFileIdManufacturer, 0, 0, ""));
    profiles[eMesgNumFileId]->addField(new CFitFieldProfile("product", &Uint16Type, eFileIdProduct, 0, 0, ""));
    profiles[eMesgNumFileId]->addSubfield(eFileIdProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerGarmin));
    profiles[eMesgNumFileId]->addSubfield(eFileIdProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerDynastream));
    profiles[eMesgNumFileId]->addSubfield(eFileIdProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eFileIdProduct, 0, 0, "", eFileIdManufacturer, eManufacturerDynastreamOem));
    profiles[eMesgNumFileId]->addField(new CFitFieldProfile("serial_number", &Uint32zType, eFileIdSerialNumber, 0, 0, ""));
    profiles[eMesgNumFileId]->addField(new CFitFieldProfile("time_created", &EnumType, eFileIdTimeCreated, 0, 0, ""));
    profiles[eMesgNumFileId]->addField(new CFitFieldProfile("number", &Uint16Type, eFileIdNumber, 0, 0, ""));
    profiles[eMesgNumFileId]->addField(new CFitFieldProfile("product_name", &StringType, eFileIdProductName, 0, 0, ""));
}

void initFileCreator(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumFileCreator] = new CFitProfile("file_creator", eMesgNumFileCreator);
    profiles[eMesgNumFileCreator]->addField(new CFitFieldProfile("software_version", &Uint16Type, eFileCreatorSoftwareVersion, 0, 0, ""));
    profiles[eMesgNumFileCreator]->addField(new CFitFieldProfile("hardware_version", &Uint8Type, eFileCreatorHardwareVersion, 0, 0, ""));
}

void initTimestampCorrelation(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumTimestampCorrelation] = new CFitProfile("timestamp_correlation", eMesgNumTimestampCorrelation);
    profiles[eMesgNumTimestampCorrelation]->addField(new CFitFieldProfile("timestamp", &EnumType, eTimestampCorrelationTimestamp, 0, 0, "s"));
    profiles[eMesgNumTimestampCorrelation]->addField(new CFitFieldProfile("fractional_timestamp", &Uint16Type, eTimestampCorrelationFractionalTimestamp, 32768, 0, "s"));
    profiles[eMesgNumTimestampCorrelation]->addField(new CFitFieldProfile("system_timestamp", &EnumType, eTimestampCorrelationSystemTimestamp, 0, 0, "s"));
    profiles[eMesgNumTimestampCorrelation]->addField(new CFitFieldProfile("fractional_system_timestamp", &Uint16Type, eTimestampCorrelationFractionalSystemTimestamp, 32768, 0, "s"));
    profiles[eMesgNumTimestampCorrelation]->addField(new CFitFieldProfile("local_timestamp", &EnumType, eTimestampCorrelationLocalTimestamp, 0, 0, "s"));
    profiles[eMesgNumTimestampCorrelation]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, eTimestampCorrelationTimestampMs, 0, 0, "ms"));
    profiles[eMesgNumTimestampCorrelation]->addField(new CFitFieldProfile("system_timestamp_ms", &Uint16Type, eTimestampCorrelationSystemTimestampMs, 0, 0, "ms"));
}

void initSoftware(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSoftware] = new CFitProfile("software", eMesgNumSoftware);
    profiles[eMesgNumSoftware]->addField(new CFitFieldProfile("message_index", &EnumType, eSoftwareMessageIndex, 0, 0, ""));
    profiles[eMesgNumSoftware]->addField(new CFitFieldProfile("version", &Uint16Type, eSoftwareVersion, 100, 0, ""));
    profiles[eMesgNumSoftware]->addField(new CFitFieldProfile("part_number", &StringType, eSoftwarePartNumber, 0, 0, ""));
}

void initSlaveDevice(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSlaveDevice] = new CFitProfile("slave_device", eMesgNumSlaveDevice);
    profiles[eMesgNumSlaveDevice]->addField(new CFitFieldProfile("manufacturer", &EnumType, eSlaveDeviceManufacturer, 0, 0, ""));
    profiles[eMesgNumSlaveDevice]->addField(new CFitFieldProfile("product", &Uint16Type, eSlaveDeviceProduct, 0, 0, ""));
    profiles[eMesgNumSlaveDevice]->addSubfield(eSlaveDeviceProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerGarmin));
    profiles[eMesgNumSlaveDevice]->addSubfield(eSlaveDeviceProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerDynastream));
    profiles[eMesgNumSlaveDevice]->addSubfield(eSlaveDeviceProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eSlaveDeviceProduct, 0, 0, "", eSlaveDeviceManufacturer, eManufacturerDynastreamOem));
}

void initCapabilities(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumCapabilities] = new CFitProfile("capabilities", eMesgNumCapabilities);
    profiles[eMesgNumCapabilities]->addField(new CFitFieldProfile("languages", &Uint8zType, eCapabilitiesLanguages, 0, 0, ""));
    profiles[eMesgNumCapabilities]->addField(new CFitFieldProfile("sports", &EnumType, eCapabilitiesSports, 0, 0, ""));
    profiles[eMesgNumCapabilities]->addField(new CFitFieldProfile("workouts_supported", &EnumType, eCapabilitiesWorkoutsSupported, 0, 0, ""));
    profiles[eMesgNumCapabilities]->addField(new CFitFieldProfile("connectivity_supported", &EnumType, eCapabilitiesConnectivitySupported, 0, 0, ""));
}

void initFileCapabilities(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumFileCapabilities] = new CFitProfile("file_capabilities", eMesgNumFileCapabilities);
    profiles[eMesgNumFileCapabilities]->addField(new CFitFieldProfile("message_index", &EnumType, eFileCapabilitiesMessageIndex, 0, 0, ""));
    profiles[eMesgNumFileCapabilities]->addField(new CFitFieldProfile("type", &EnumType, eFileCapabilitiesType, 0, 0, ""));
    profiles[eMesgNumFileCapabilities]->addField(new CFitFieldProfile("flags", &EnumType, eFileCapabilitiesFlags, 0, 0, ""));
    profiles[eMesgNumFileCapabilities]->addField(new CFitFieldProfile("directory", &StringType, eFileCapabilitiesDirectory, 0, 0, ""));
    profiles[eMesgNumFileCapabilities]->addField(new CFitFieldProfile("max_count", &Uint16Type, eFileCapabilitiesMaxCount, 0, 0, ""));
    profiles[eMesgNumFileCapabilities]->addField(new CFitFieldProfile("max_size", &Uint32Type, eFileCapabilitiesMaxSize, 0, 0, "bytes"));
}

void initMesgCapabilities(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumMesgCapabilities] = new CFitProfile("mesg_capabilities", eMesgNumMesgCapabilities);
    profiles[eMesgNumMesgCapabilities]->addField(new CFitFieldProfile("message_index", &EnumType, eMesgCapabilitiesMessageIndex, 0, 0, ""));
    profiles[eMesgNumMesgCapabilities]->addField(new CFitFieldProfile("file", &EnumType, eMesgCapabilitiesFile, 0, 0, ""));
    profiles[eMesgNumMesgCapabilities]->addField(new CFitFieldProfile("mesg_num", &EnumType, eMesgCapabilitiesMesgNum, 0, 0, ""));
    profiles[eMesgNumMesgCapabilities]->addField(new CFitFieldProfile("count_type", &EnumType, eMesgCapabilitiesCountType, 0, 0, ""));
    profiles[eMesgNumMesgCapabilities]->addField(new CFitFieldProfile("count", &Uint16Type, eMesgCapabilitiesCount, 0, 0, ""));
    profiles[eMesgNumMesgCapabilities]->addSubfield(eMesgCapabilitiesCount, new CFitSubfieldProfile("num_per_file", &Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountNumPerFile));
    profiles[eMesgNumMesgCapabilities]->addSubfield(eMesgCapabilitiesCount, new CFitSubfieldProfile("max_per_file", &Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountMaxPerFile));
    profiles[eMesgNumMesgCapabilities]->addSubfield(eMesgCapabilitiesCount, new CFitSubfieldProfile("max_per_file_type", &Uint16Type, eMesgCapabilitiesCount, 0, 0, "", eMesgCapabilitiesCountType, eMesgCountMaxPerFileType));
}

void initFieldCapabilities(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumFieldCapabilities] = new CFitProfile("field_capabilities", eMesgNumFieldCapabilities);
    profiles[eMesgNumFieldCapabilities]->addField(new CFitFieldProfile("message_index", &EnumType, eFieldCapabilitiesMessageIndex, 0, 0, ""));
    profiles[eMesgNumFieldCapabilities]->addField(new CFitFieldProfile("file", &EnumType, eFieldCapabilitiesFile, 0, 0, ""));
    profiles[eMesgNumFieldCapabilities]->addField(new CFitFieldProfile("mesg_num", &EnumType, eFieldCapabilitiesMesgNum, 0, 0, ""));
    profiles[eMesgNumFieldCapabilities]->addField(new CFitFieldProfile("field_num", &Uint8Type, eFieldCapabilitiesFieldNum, 0, 0, ""));
    profiles[eMesgNumFieldCapabilities]->addField(new CFitFieldProfile("count", &Uint16Type, eFieldCapabilitiesCount, 0, 0, ""));
}

void initDeviceSettings(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumDeviceSettings] = new CFitProfile("device_settings", eMesgNumDeviceSettings);
    profiles[eMesgNumDeviceSettings]->addField(new CFitFieldProfile("active_time_zone", &Uint8Type, eDeviceSettingsActiveTimeZone, 0, 0, ""));
    profiles[eMesgNumDeviceSettings]->addField(new CFitFieldProfile("utc_offset", &Uint32Type, eDeviceSettingsUtcOffset, 0, 0, ""));
    profiles[eMesgNumDeviceSettings]->addField(new CFitFieldProfile("time_zone_offset", &Sint8Type, eDeviceSettingsTimeZoneOffset, 4, 0, "hr"));
}

void initUserProfile(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumUserProfile] = new CFitProfile("user_profile", eMesgNumUserProfile);
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("message_index", &EnumType, eUserProfileMessageIndex, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("friendly_name", &StringType, eUserProfileFriendlyName, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("gender", &EnumType, eUserProfileGender, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("age", &Uint8Type, eUserProfileAge, 0, 0, "years"));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("height", &Uint8Type, eUserProfileHeight, 100, 0, "m"));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("weight", &Uint16Type, eUserProfileWeight, 10, 0, "kg"));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("language", &EnumType, eUserProfileLanguage, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("elev_setting", &EnumType, eUserProfileElevSetting, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("weight_setting", &EnumType, eUserProfileWeightSetting, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("resting_heart_rate", &Uint8Type, eUserProfileRestingHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("default_max_running_heart_rate", &Uint8Type, eUserProfileDefaultMaxRunningHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("default_max_biking_heart_rate", &Uint8Type, eUserProfileDefaultMaxBikingHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("default_max_heart_rate", &Uint8Type, eUserProfileDefaultMaxHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("hr_setting", &EnumType, eUserProfileHrSetting, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("speed_setting", &EnumType, eUserProfileSpeedSetting, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("dist_setting", &EnumType, eUserProfileDistSetting, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("power_setting", &EnumType, eUserProfilePowerSetting, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("activity_class", &EnumType, eUserProfileActivityClass, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("position_setting", &EnumType, eUserProfilePositionSetting, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("temperature_setting", &EnumType, eUserProfileTemperatureSetting, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("local_id", &EnumType, eUserProfileLocalId, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("global_id", &ByteType, eUserProfileGlobalId, 0, 0, ""));
    profiles[eMesgNumUserProfile]->addField(new CFitFieldProfile("height_setting", &EnumType, eUserProfileHeightSetting, 0, 0, ""));
}

void initHrmProfile(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumHrmProfile] = new CFitProfile("hrm_profile", eMesgNumHrmProfile);
    profiles[eMesgNumHrmProfile]->addField(new CFitFieldProfile("message_index", &EnumType, eHrmProfileMessageIndex, 0, 0, ""));
    profiles[eMesgNumHrmProfile]->addField(new CFitFieldProfile("enabled", &EnumType, eHrmProfileEnabled, 0, 0, ""));
    profiles[eMesgNumHrmProfile]->addField(new CFitFieldProfile("hrm_ant_id", &Uint16zType, eHrmProfileHrmAntId, 0, 0, ""));
    profiles[eMesgNumHrmProfile]->addField(new CFitFieldProfile("log_hrv", &EnumType, eHrmProfileLogHrv, 0, 0, ""));
    profiles[eMesgNumHrmProfile]->addField(new CFitFieldProfile("hrm_ant_id_trans_type", &Uint8zType, eHrmProfileHrmAntIdTransType, 0, 0, ""));
}

void initSdmProfile(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSdmProfile] = new CFitProfile("sdm_profile", eMesgNumSdmProfile);
    profiles[eMesgNumSdmProfile]->addField(new CFitFieldProfile("message_index", &EnumType, eSdmProfileMessageIndex, 0, 0, ""));
    profiles[eMesgNumSdmProfile]->addField(new CFitFieldProfile("enabled", &EnumType, eSdmProfileEnabled, 0, 0, ""));
    profiles[eMesgNumSdmProfile]->addField(new CFitFieldProfile("sdm_ant_id", &Uint16zType, eSdmProfileSdmAntId, 0, 0, ""));
    profiles[eMesgNumSdmProfile]->addField(new CFitFieldProfile("sdm_cal_factor", &Uint16Type, eSdmProfileSdmCalFactor, 10, 0, "%"));
    profiles[eMesgNumSdmProfile]->addField(new CFitFieldProfile("odometer", &Uint32Type, eSdmProfileOdometer, 100, 0, "m"));
    profiles[eMesgNumSdmProfile]->addField(new CFitFieldProfile("speed_source", &EnumType, eSdmProfileSpeedSource, 0, 0, ""));
    profiles[eMesgNumSdmProfile]->addField(new CFitFieldProfile("sdm_ant_id_trans_type", &Uint8zType, eSdmProfileSdmAntIdTransType, 0, 0, ""));
    profiles[eMesgNumSdmProfile]->addField(new CFitFieldProfile("odometer_rollover", &Uint8Type, eSdmProfileOdometerRollover, 0, 0, ""));
}

void initBikeProfile(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumBikeProfile] = new CFitProfile("bike_profile", eMesgNumBikeProfile);
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("message_index", &EnumType, eBikeProfileMessageIndex, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("name", &StringType, eBikeProfileName, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("sport", &EnumType, eBikeProfileSport, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("sub_sport", &EnumType, eBikeProfileSubSport, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("odometer", &Uint32Type, eBikeProfileOdometer, 100, 0, "m"));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_spd_ant_id", &Uint16zType, eBikeProfileBikeSpdAntId, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_cad_ant_id", &Uint16zType, eBikeProfileBikeCadAntId, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_spdcad_ant_id", &Uint16zType, eBikeProfileBikeSpdcadAntId, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_power_ant_id", &Uint16zType, eBikeProfileBikePowerAntId, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("custom_wheelsize", &Uint16Type, eBikeProfileCustomWheelsize, 1000, 0, "m"));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("auto_wheelsize", &Uint16Type, eBikeProfileAutoWheelsize, 1000, 0, "m"));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_weight", &Uint16Type, eBikeProfileBikeWeight, 10, 0, "kg"));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("power_cal_factor", &Uint16Type, eBikeProfilePowerCalFactor, 10, 0, "%"));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("auto_wheel_cal", &EnumType, eBikeProfileAutoWheelCal, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("auto_power_zero", &EnumType, eBikeProfileAutoPowerZero, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("id", &Uint8Type, eBikeProfileId, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("spd_enabled", &EnumType, eBikeProfileSpdEnabled, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("cad_enabled", &EnumType, eBikeProfileCadEnabled, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("spdcad_enabled", &EnumType, eBikeProfileSpdcadEnabled, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("power_enabled", &EnumType, eBikeProfilePowerEnabled, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("crank_length", &Uint8Type, eBikeProfileCrankLength, 2, -110, "mm"));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("enabled", &EnumType, eBikeProfileEnabled, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_spd_ant_id_trans_type", &Uint8zType, eBikeProfileBikeSpdAntIdTransType, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_cad_ant_id_trans_type", &Uint8zType, eBikeProfileBikeCadAntIdTransType, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_spdcad_ant_id_trans_type", &Uint8zType, eBikeProfileBikeSpdcadAntIdTransType, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("bike_power_ant_id_trans_type", &Uint8zType, eBikeProfileBikePowerAntIdTransType, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("odometer_rollover", &Uint8Type, eBikeProfileOdometerRollover, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("front_gear_num", &Uint8zType, eBikeProfileFrontGearNum, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("front_gear", &Uint8zType, eBikeProfileFrontGear, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("rear_gear_num", &Uint8zType, eBikeProfileRearGearNum, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("rear_gear", &Uint8zType, eBikeProfileRearGear, 0, 0, ""));
    profiles[eMesgNumBikeProfile]->addField(new CFitFieldProfile("shimano_di2_enabled", &EnumType, eBikeProfileShimanoDi2Enabled, 0, 0, ""));
}

void initZonesTarget(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumZonesTarget] = new CFitProfile("zones_target", eMesgNumZonesTarget);
    profiles[eMesgNumZonesTarget]->addField(new CFitFieldProfile("max_heart_rate", &Uint8Type, eZonesTargetMaxHeartRate, 0, 0, ""));
    profiles[eMesgNumZonesTarget]->addField(new CFitFieldProfile("threshold_heart_rate", &Uint8Type, eZonesTargetThresholdHeartRate, 0, 0, ""));
    profiles[eMesgNumZonesTarget]->addField(new CFitFieldProfile("functional_threshold_power", &Uint16Type, eZonesTargetFunctionalThresholdPower, 0, 0, ""));
    profiles[eMesgNumZonesTarget]->addField(new CFitFieldProfile("hr_calc_type", &EnumType, eZonesTargetHrCalcType, 0, 0, ""));
    profiles[eMesgNumZonesTarget]->addField(new CFitFieldProfile("pwr_calc_type", &EnumType, eZonesTargetPwrCalcType, 0, 0, ""));
}

void initSport(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSport] = new CFitProfile("sport", eMesgNumSport);
    profiles[eMesgNumSport]->addField(new CFitFieldProfile("sport", &EnumType, eSportSport, 0, 0, ""));
    profiles[eMesgNumSport]->addField(new CFitFieldProfile("sub_sport", &EnumType, eSportSubSport, 0, 0, ""));
    profiles[eMesgNumSport]->addField(new CFitFieldProfile("name", &StringType, eSportName, 0, 0, ""));
}

void initHrZone(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumHrZone] = new CFitProfile("hr_zone", eMesgNumHrZone);
    profiles[eMesgNumHrZone]->addField(new CFitFieldProfile("message_index", &EnumType, eHrZoneMessageIndex, 0, 0, ""));
    profiles[eMesgNumHrZone]->addField(new CFitFieldProfile("high_bpm", &Uint8Type, eHrZoneHighBpm, 0, 0, "bpm"));
    profiles[eMesgNumHrZone]->addField(new CFitFieldProfile("name", &StringType, eHrZoneName, 0, 0, ""));
}

void initSpeedZone(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSpeedZone] = new CFitProfile("speed_zone", eMesgNumSpeedZone);
    profiles[eMesgNumSpeedZone]->addField(new CFitFieldProfile("message_index", &EnumType, eSpeedZoneMessageIndex, 0, 0, ""));
    profiles[eMesgNumSpeedZone]->addField(new CFitFieldProfile("high_value", &Uint16Type, eSpeedZoneHighValue, 1000, 0, "m/s"));
    profiles[eMesgNumSpeedZone]->addField(new CFitFieldProfile("name", &StringType, eSpeedZoneName, 0, 0, ""));
}

void initCadenceZone(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumCadenceZone] = new CFitProfile("cadence_zone", eMesgNumCadenceZone);
    profiles[eMesgNumCadenceZone]->addField(new CFitFieldProfile("message_index", &EnumType, eCadenceZoneMessageIndex, 0, 0, ""));
    profiles[eMesgNumCadenceZone]->addField(new CFitFieldProfile("high_value", &Uint8Type, eCadenceZoneHighValue, 0, 0, "rpm"));
    profiles[eMesgNumCadenceZone]->addField(new CFitFieldProfile("name", &StringType, eCadenceZoneName, 0, 0, ""));
}

void initPowerZone(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumPowerZone] = new CFitProfile("power_zone", eMesgNumPowerZone);
    profiles[eMesgNumPowerZone]->addField(new CFitFieldProfile("message_index", &EnumType, ePowerZoneMessageIndex, 0, 0, ""));
    profiles[eMesgNumPowerZone]->addField(new CFitFieldProfile("high_value", &Uint16Type, ePowerZoneHighValue, 0, 0, "watts"));
    profiles[eMesgNumPowerZone]->addField(new CFitFieldProfile("name", &StringType, ePowerZoneName, 0, 0, ""));
}

void initMetZone(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumMetZone] = new CFitProfile("met_zone", eMesgNumMetZone);
    profiles[eMesgNumMetZone]->addField(new CFitFieldProfile("message_index", &EnumType, eMetZoneMessageIndex, 0, 0, ""));
    profiles[eMesgNumMetZone]->addField(new CFitFieldProfile("high_bpm", &Uint8Type, eMetZoneHighBpm, 0, 0, ""));
    profiles[eMesgNumMetZone]->addField(new CFitFieldProfile("calories", &Uint16Type, eMetZoneCalories, 10, 0, "kcal / min"));
    profiles[eMesgNumMetZone]->addField(new CFitFieldProfile("fat_calories", &Uint8Type, eMetZoneFatCalories, 10, 0, "kcal / min"));
}

void initGoal(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumGoal] = new CFitProfile("goal", eMesgNumGoal);
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("message_index", &EnumType, eGoalMessageIndex, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("sport", &EnumType, eGoalSport, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("sub_sport", &EnumType, eGoalSubSport, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("start_date", &EnumType, eGoalStartDate, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("end_date", &EnumType, eGoalEndDate, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("type", &EnumType, eGoalType, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("value", &Uint32Type, eGoalValue, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("repeat", &EnumType, eGoalRepeat, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("target_value", &Uint32Type, eGoalTargetValue, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("recurrence", &EnumType, eGoalRecurrence, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("recurrence_value", &Uint16Type, eGoalRecurrenceValue, 0, 0, ""));
    profiles[eMesgNumGoal]->addField(new CFitFieldProfile("enabled", &EnumType, eGoalEnabled, 0, 0, ""));
}

void initActivity(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumActivity] = new CFitProfile("activity", eMesgNumActivity);
    profiles[eMesgNumActivity]->addField(new CFitFieldProfile("timestamp", &EnumType, eActivityTimestamp, 0, 0, ""));
    profiles[eMesgNumActivity]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, eActivityTotalTimerTime, 1000, 0, "s"));
    profiles[eMesgNumActivity]->addField(new CFitFieldProfile("num_sessions", &Uint16Type, eActivityNumSessions, 0, 0, ""));
    profiles[eMesgNumActivity]->addField(new CFitFieldProfile("type", &EnumType, eActivityType, 0, 0, ""));
    profiles[eMesgNumActivity]->addField(new CFitFieldProfile("event", &EnumType, eActivityEvent, 0, 0, ""));
    profiles[eMesgNumActivity]->addField(new CFitFieldProfile("event_type", &EnumType, eActivityEventType, 0, 0, ""));
    profiles[eMesgNumActivity]->addField(new CFitFieldProfile("local_timestamp", &EnumType, eActivityLocalTimestamp, 0, 0, ""));
    profiles[eMesgNumActivity]->addField(new CFitFieldProfile("event_group", &Uint8Type, eActivityEventGroup, 0, 0, ""));
}

void initSession(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSession] = new CFitProfile("session", eMesgNumSession);
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("message_index", &EnumType, eSessionMessageIndex, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("timestamp", &EnumType, eSessionTimestamp, 0, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("event", &EnumType, eSessionEvent, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("event_type", &EnumType, eSessionEventType, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("start_time", &EnumType, eSessionStartTime, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("start_position_lat", &Sint32Type, eSessionStartPositionLat, 0, 0, "semicircles"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("start_position_long", &Sint32Type, eSessionStartPositionLong, 0, 0, "semicircles"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("sport", &EnumType, eSessionSport, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("sub_sport", &EnumType, eSessionSubSport, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_elapsed_time", &Uint32Type, eSessionTotalElapsedTime, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, eSessionTotalTimerTime, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_distance", &Uint32Type, eSessionTotalDistance, 100, 0, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_cycles", &Uint32Type, eSessionTotalCycles, 0, 0, "cycles"));
    profiles[eMesgNumSession]->addSubfield(eSessionTotalCycles, new CFitSubfieldProfile("total_strides", &Uint32Type, eSessionTotalCycles, 0, 0, "strides", eSessionSport, eSportRunning));
    profiles[eMesgNumSession]->addSubfield(eSessionTotalCycles, new CFitSubfieldProfile("total_strides", &Uint32Type, eSessionTotalCycles, 0, 0, "", eSessionSport, eSportWalking));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_calories", &Uint16Type, eSessionTotalCalories, 0, 0, "kcal"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_fat_calories", &Uint16Type, eSessionTotalFatCalories, 0, 0, "kcal"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_speed", &Uint16Type, eSessionAvgSpeed, 0, 0, ""));
    profiles[eMesgNumSession]->addComponent(eSessionAvgSpeed, new CFitComponentfieldProfile("avg_speed", &Uint16Type, eSessionAvgSpeed, 1000, 0, "m/s", eSessionEnhancedAvgSpeed, 16));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_speed", &Uint16Type, eSessionMaxSpeed, 0, 0, ""));
    profiles[eMesgNumSession]->addComponent(eSessionMaxSpeed, new CFitComponentfieldProfile("max_speed", &Uint16Type, eSessionMaxSpeed, 1000, 0, "m/s", eSessionEnhancedMaxSpeed, 16));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_heart_rate", &Uint8Type, eSessionAvgHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_heart_rate", &Uint8Type, eSessionMaxHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_cadence", &Uint8Type, eSessionAvgCadence, 0, 0, "rpm"));
    profiles[eMesgNumSession]->addSubfield(eSessionAvgCadence, new CFitSubfieldProfile("avg_running_cadence", &Uint8Type, eSessionAvgCadence, 0, 0, "strides/min", eSessionSport, eSportRunning));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_cadence", &Uint8Type, eSessionMaxCadence, 0, 0, "rpm"));
    profiles[eMesgNumSession]->addSubfield(eSessionMaxCadence, new CFitSubfieldProfile("max_running_cadence", &Uint8Type, eSessionMaxCadence, 0, 0, "strides/min", eSessionSport, eSportRunning));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_power", &Uint16Type, eSessionAvgPower, 0, 0, "watts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_power", &Uint16Type, eSessionMaxPower, 0, 0, "watts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_ascent", &Uint16Type, eSessionTotalAscent, 0, 0, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_descent", &Uint16Type, eSessionTotalDescent, 0, 0, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_training_effect", &Uint8Type, eSessionTotalTrainingEffect, 10, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("first_lap_index", &Uint16Type, eSessionFirstLapIndex, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("num_laps", &Uint16Type, eSessionNumLaps, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("event_group", &Uint8Type, eSessionEventGroup, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("trigger", &EnumType, eSessionTrigger, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("nec_lat", &Sint32Type, eSessionNecLat, 0, 0, "semicircles"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("nec_long", &Sint32Type, eSessionNecLong, 0, 0, "semicircles"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("swc_lat", &Sint32Type, eSessionSwcLat, 0, 0, "semicircles"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("swc_long", &Sint32Type, eSessionSwcLong, 0, 0, "semicircles"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("normalized_power", &Uint16Type, eSessionNormalizedPower, 0, 0, "watts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("training_stress_score", &Uint16Type, eSessionTrainingStressScore, 10, 0, "tss"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("intensity_factor", &Uint16Type, eSessionIntensityFactor, 1000, 0, "if"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("left_right_balance", &EnumType, eSessionLeftRightBalance, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_stroke_count", &Uint32Type, eSessionAvgStrokeCount, 10, 0, "strokes/lap"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_stroke_distance", &Uint16Type, eSessionAvgStrokeDistance, 100, 0, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("swim_stroke", &EnumType, eSessionSwimStroke, 0, 0, "swim_stroke"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("pool_length", &Uint16Type, eSessionPoolLength, 100, 0, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("threshold_power", &Uint16Type, eSessionThresholdPower, 0, 0, "watts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("pool_length_unit", &EnumType, eSessionPoolLengthUnit, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("num_active_lengths", &Uint16Type, eSessionNumActiveLengths, 0, 0, "lengths"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_work", &Uint32Type, eSessionTotalWork, 0, 0, "J"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_altitude", &Uint16Type, eSessionAvgAltitude, 0, 0, ""));
    profiles[eMesgNumSession]->addComponent(eSessionAvgAltitude, new CFitComponentfieldProfile("avg_altitude", &Uint16Type, eSessionAvgAltitude, 5, 500, "m", eSessionEnhancedAvgAltitude, 16));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_altitude", &Uint16Type, eSessionMaxAltitude, 0, 0, ""));
    profiles[eMesgNumSession]->addComponent(eSessionMaxAltitude, new CFitComponentfieldProfile("max_altitude", &Uint16Type, eSessionMaxAltitude, 5, 500, "m", eSessionEnhancedMaxAltitude, 16));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("gps_accuracy", &Uint8Type, eSessionGpsAccuracy, 0, 0, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_grade", &Sint16Type, eSessionAvgGrade, 100, 0, "%"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_pos_grade", &Sint16Type, eSessionAvgPosGrade, 100, 0, "%"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_neg_grade", &Sint16Type, eSessionAvgNegGrade, 100, 0, "%"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_pos_grade", &Sint16Type, eSessionMaxPosGrade, 100, 0, "%"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_neg_grade", &Sint16Type, eSessionMaxNegGrade, 100, 0, "%"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_temperature", &Sint8Type, eSessionAvgTemperature, 0, 0, "C"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_temperature", &Sint8Type, eSessionMaxTemperature, 0, 0, "C"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_moving_time", &Uint32Type, eSessionTotalMovingTime, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_pos_vertical_speed", &Sint16Type, eSessionAvgPosVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_neg_vertical_speed", &Sint16Type, eSessionAvgNegVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_pos_vertical_speed", &Sint16Type, eSessionMaxPosVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_neg_vertical_speed", &Sint16Type, eSessionMaxNegVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("min_heart_rate", &Uint8Type, eSessionMinHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("time_in_hr_zone", &Uint32Type, eSessionTimeInHrZone, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("time_in_speed_zone", &Uint32Type, eSessionTimeInSpeedZone, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("time_in_cadence_zone", &Uint32Type, eSessionTimeInCadenceZone, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("time_in_power_zone", &Uint32Type, eSessionTimeInPowerZone, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_lap_time", &Uint32Type, eSessionAvgLapTime, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("best_lap_index", &Uint16Type, eSessionBestLapIndex, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("min_altitude", &Uint16Type, eSessionMinAltitude, 0, 0, ""));
    profiles[eMesgNumSession]->addComponent(eSessionMinAltitude, new CFitComponentfieldProfile("min_altitude", &Uint16Type, eSessionMinAltitude, 5, 500, "m", eSessionEnhancedMinAltitude, 16));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("player_score", &Uint16Type, eSessionPlayerScore, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("opponent_score", &Uint16Type, eSessionOpponentScore, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("opponent_name", &StringType, eSessionOpponentName, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("stroke_count", &Uint16Type, eSessionStrokeCount, 0, 0, "counts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("zone_count", &Uint16Type, eSessionZoneCount, 0, 0, "counts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_ball_speed", &Uint16Type, eSessionMaxBallSpeed, 100, 0, "m/s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_ball_speed", &Uint16Type, eSessionAvgBallSpeed, 100, 0, "m/s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_vertical_oscillation", &Uint16Type, eSessionAvgVerticalOscillation, 10, 0, "mm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_stance_time_percent", &Uint16Type, eSessionAvgStanceTimePercent, 100, 0, "percent"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_stance_time", &Uint16Type, eSessionAvgStanceTime, 10, 0, "ms"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_fractional_cadence", &Uint8Type, eSessionAvgFractionalCadence, 128, 0, "rpm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_fractional_cadence", &Uint8Type, eSessionMaxFractionalCadence, 128, 0, "rpm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("total_fractional_cycles", &Uint8Type, eSessionTotalFractionalCycles, 128, 0, "cycles"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_total_hemoglobin_conc", &Uint16Type, eSessionAvgTotalHemoglobinConc, 100, 0, "g/dL"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("min_total_hemoglobin_conc", &Uint16Type, eSessionMinTotalHemoglobinConc, 100, 0, "g/dL"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_total_hemoglobin_conc", &Uint16Type, eSessionMaxTotalHemoglobinConc, 100, 0, "g/dL"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_saturated_hemoglobin_percent", &Uint16Type, eSessionAvgSaturatedHemoglobinPercent, 10, 0, "%"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("min_saturated_hemoglobin_percent", &Uint16Type, eSessionMinSaturatedHemoglobinPercent, 10, 0, "%"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_saturated_hemoglobin_percent", &Uint16Type, eSessionMaxSaturatedHemoglobinPercent, 10, 0, "%"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_left_torque_effectiveness", &Uint8Type, eSessionAvgLeftTorqueEffectiveness, 2, 0, "percent"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_right_torque_effectiveness", &Uint8Type, eSessionAvgRightTorqueEffectiveness, 2, 0, "percent"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_left_pedal_smoothness", &Uint8Type, eSessionAvgLeftPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_right_pedal_smoothness", &Uint8Type, eSessionAvgRightPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_combined_pedal_smoothness", &Uint8Type, eSessionAvgCombinedPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("sport_index", &Uint8Type, eSessionSportIndex, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("time_standing", &Uint32Type, eSessionTimeStanding, 1000, 0, "s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("stand_count", &Uint16Type, eSessionStandCount, 0, 0, ""));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_left_pco", &Sint8Type, eSessionAvgLeftPco, 0, 0, "mm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_right_pco", &Sint8Type, eSessionAvgRightPco, 0, 0, "mm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_left_power_phase", &Uint8Type, eSessionAvgLeftPowerPhase, 0.7111111, 0, "degrees"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_left_power_phase_peak", &Uint8Type, eSessionAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_right_power_phase", &Uint8Type, eSessionAvgRightPowerPhase, 0.7111111, 0, "degrees"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_right_power_phase_peak", &Uint8Type, eSessionAvgRightPowerPhasePeak, 0.7111111, 0, "degrees"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_power_position", &Uint16Type, eSessionAvgPowerPosition, 0, 0, "watts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_power_position", &Uint16Type, eSessionMaxPowerPosition, 0, 0, "watts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_cadence_position", &Uint8Type, eSessionAvgCadencePosition, 0, 0, "rpm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_cadence_position", &Uint8Type, eSessionMaxCadencePosition, 0, 0, "rpm"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("enhanced_avg_speed", &Uint32Type, eSessionEnhancedAvgSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("enhanced_max_speed", &Uint32Type, eSessionEnhancedMaxSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("enhanced_avg_altitude", &Uint32Type, eSessionEnhancedAvgAltitude, 5, 500, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("enhanced_min_altitude", &Uint32Type, eSessionEnhancedMinAltitude, 5, 500, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("enhanced_max_altitude", &Uint32Type, eSessionEnhancedMaxAltitude, 5, 500, "m"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("avg_lev_motor_power", &Uint16Type, eSessionAvgLevMotorPower, 0, 0, "watts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("max_lev_motor_power", &Uint16Type, eSessionMaxLevMotorPower, 0, 0, "watts"));
    profiles[eMesgNumSession]->addField(new CFitFieldProfile("lev_battery_consumption", &Uint8Type, eSessionLevBatteryConsumption, 2, 0, "percent"));
}

void initLap(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumLap] = new CFitProfile("lap", eMesgNumLap);
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("message_index", &EnumType, eLapMessageIndex, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("timestamp", &EnumType, eLapTimestamp, 0, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("event", &EnumType, eLapEvent, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("event_type", &EnumType, eLapEventType, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("start_time", &EnumType, eLapStartTime, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("start_position_lat", &Sint32Type, eLapStartPositionLat, 0, 0, "semicircles"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("start_position_long", &Sint32Type, eLapStartPositionLong, 0, 0, "semicircles"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("end_position_lat", &Sint32Type, eLapEndPositionLat, 0, 0, "semicircles"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("end_position_long", &Sint32Type, eLapEndPositionLong, 0, 0, "semicircles"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_elapsed_time", &Uint32Type, eLapTotalElapsedTime, 1000, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, eLapTotalTimerTime, 1000, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_distance", &Uint32Type, eLapTotalDistance, 100, 0, "m"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_cycles", &Uint32Type, eLapTotalCycles, 0, 0, "cycles"));
    profiles[eMesgNumLap]->addSubfield(eLapTotalCycles, new CFitSubfieldProfile("total_strides", &Uint32Type, eLapTotalCycles, 0, 0, "strides", eLapSport, eSportRunning));
    profiles[eMesgNumLap]->addSubfield(eLapTotalCycles, new CFitSubfieldProfile("total_strides", &Uint32Type, eLapTotalCycles, 0, 0, "", eLapSport, eSportWalking));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_calories", &Uint16Type, eLapTotalCalories, 0, 0, "kcal"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_fat_calories", &Uint16Type, eLapTotalFatCalories, 0, 0, "kcal"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_speed", &Uint16Type, eLapAvgSpeed, 0, 0, ""));
    profiles[eMesgNumLap]->addComponent(eLapAvgSpeed, new CFitComponentfieldProfile("avg_speed", &Uint16Type, eLapAvgSpeed, 1000, 0, "m/s", eLapEnhancedAvgSpeed, 16));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_speed", &Uint16Type, eLapMaxSpeed, 0, 0, ""));
    profiles[eMesgNumLap]->addComponent(eLapMaxSpeed, new CFitComponentfieldProfile("max_speed", &Uint16Type, eLapMaxSpeed, 1000, 0, "m/s", eLapEnhancedMaxSpeed, 16));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_heart_rate", &Uint8Type, eLapAvgHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_heart_rate", &Uint8Type, eLapMaxHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_cadence", &Uint8Type, eLapAvgCadence, 0, 0, "rpm"));
    profiles[eMesgNumLap]->addSubfield(eLapAvgCadence, new CFitSubfieldProfile("avg_running_cadence", &Uint8Type, eLapAvgCadence, 0, 0, "strides/min", eLapSport, eSportRunning));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_cadence", &Uint8Type, eLapMaxCadence, 0, 0, "rpm"));
    profiles[eMesgNumLap]->addSubfield(eLapMaxCadence, new CFitSubfieldProfile("max_running_cadence", &Uint8Type, eLapMaxCadence, 0, 0, "strides/min", eLapSport, eSportRunning));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_power", &Uint16Type, eLapAvgPower, 0, 0, "watts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_power", &Uint16Type, eLapMaxPower, 0, 0, "watts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_ascent", &Uint16Type, eLapTotalAscent, 0, 0, "m"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_descent", &Uint16Type, eLapTotalDescent, 0, 0, "m"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("intensity", &EnumType, eLapIntensity, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("lap_trigger", &EnumType, eLapLapTrigger, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("sport", &EnumType, eLapSport, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("event_group", &Uint8Type, eLapEventGroup, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("num_lengths", &Uint16Type, eLapNumLengths, 0, 0, "lengths"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("normalized_power", &Uint16Type, eLapNormalizedPower, 0, 0, "watts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("left_right_balance", &EnumType, eLapLeftRightBalance, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("first_length_index", &Uint16Type, eLapFirstLengthIndex, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_stroke_distance", &Uint16Type, eLapAvgStrokeDistance, 100, 0, "m"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("swim_stroke", &EnumType, eLapSwimStroke, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("sub_sport", &EnumType, eLapSubSport, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("num_active_lengths", &Uint16Type, eLapNumActiveLengths, 0, 0, "lengths"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_work", &Uint32Type, eLapTotalWork, 0, 0, "J"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_altitude", &Uint16Type, eLapAvgAltitude, 0, 0, ""));
    profiles[eMesgNumLap]->addComponent(eLapAvgAltitude, new CFitComponentfieldProfile("avg_altitude", &Uint16Type, eLapAvgAltitude, 5, 500, "m", eLapEnhancedAvgAltitude, 16));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_altitude", &Uint16Type, eLapMaxAltitude, 0, 0, ""));
    profiles[eMesgNumLap]->addComponent(eLapMaxAltitude, new CFitComponentfieldProfile("max_altitude", &Uint16Type, eLapMaxAltitude, 5, 500, "m", eLapEnhancedMaxAltitude, 16));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("gps_accuracy", &Uint8Type, eLapGpsAccuracy, 0, 0, "m"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_grade", &Sint16Type, eLapAvgGrade, 100, 0, "%"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_pos_grade", &Sint16Type, eLapAvgPosGrade, 100, 0, "%"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_neg_grade", &Sint16Type, eLapAvgNegGrade, 100, 0, "%"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_pos_grade", &Sint16Type, eLapMaxPosGrade, 100, 0, "%"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_neg_grade", &Sint16Type, eLapMaxNegGrade, 100, 0, "%"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_temperature", &Sint8Type, eLapAvgTemperature, 0, 0, "C"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_temperature", &Sint8Type, eLapMaxTemperature, 0, 0, "C"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_moving_time", &Uint32Type, eLapTotalMovingTime, 1000, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_pos_vertical_speed", &Sint16Type, eLapAvgPosVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_neg_vertical_speed", &Sint16Type, eLapAvgNegVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_pos_vertical_speed", &Sint16Type, eLapMaxPosVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_neg_vertical_speed", &Sint16Type, eLapMaxNegVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("time_in_hr_zone", &Uint32Type, eLapTimeInHrZone, 1000, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("time_in_speed_zone", &Uint32Type, eLapTimeInSpeedZone, 1000, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("time_in_cadence_zone", &Uint32Type, eLapTimeInCadenceZone, 1000, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("time_in_power_zone", &Uint32Type, eLapTimeInPowerZone, 1000, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("repetition_num", &Uint16Type, eLapRepetitionNum, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("min_altitude", &Uint16Type, eLapMinAltitude, 0, 0, ""));
    profiles[eMesgNumLap]->addComponent(eLapMinAltitude, new CFitComponentfieldProfile("min_altitude", &Uint16Type, eLapMinAltitude, 5, 500, "m", eLapEnhancedMinAltitude, 16));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("min_heart_rate", &Uint8Type, eLapMinHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("wkt_step_index", &EnumType, eLapWktStepIndex, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("opponent_score", &Uint16Type, eLapOpponentScore, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("stroke_count", &Uint16Type, eLapStrokeCount, 0, 0, "counts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("zone_count", &Uint16Type, eLapZoneCount, 0, 0, "counts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_vertical_oscillation", &Uint16Type, eLapAvgVerticalOscillation, 10, 0, "mm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_stance_time_percent", &Uint16Type, eLapAvgStanceTimePercent, 100, 0, "percent"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_stance_time", &Uint16Type, eLapAvgStanceTime, 10, 0, "ms"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_fractional_cadence", &Uint8Type, eLapAvgFractionalCadence, 128, 0, "rpm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_fractional_cadence", &Uint8Type, eLapMaxFractionalCadence, 128, 0, "rpm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("total_fractional_cycles", &Uint8Type, eLapTotalFractionalCycles, 128, 0, "cycles"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("player_score", &Uint16Type, eLapPlayerScore, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_total_hemoglobin_conc", &Uint16Type, eLapAvgTotalHemoglobinConc, 100, 0, "g/dL"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("min_total_hemoglobin_conc", &Uint16Type, eLapMinTotalHemoglobinConc, 100, 0, "g/dL"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_total_hemoglobin_conc", &Uint16Type, eLapMaxTotalHemoglobinConc, 100, 0, "g/dL"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_saturated_hemoglobin_percent", &Uint16Type, eLapAvgSaturatedHemoglobinPercent, 10, 0, "%"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("min_saturated_hemoglobin_percent", &Uint16Type, eLapMinSaturatedHemoglobinPercent, 10, 0, "%"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_saturated_hemoglobin_percent", &Uint16Type, eLapMaxSaturatedHemoglobinPercent, 10, 0, "%"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_left_torque_effectiveness", &Uint8Type, eLapAvgLeftTorqueEffectiveness, 2, 0, "percent"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_right_torque_effectiveness", &Uint8Type, eLapAvgRightTorqueEffectiveness, 2, 0, "percent"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_left_pedal_smoothness", &Uint8Type, eLapAvgLeftPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_right_pedal_smoothness", &Uint8Type, eLapAvgRightPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_combined_pedal_smoothness", &Uint8Type, eLapAvgCombinedPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("time_standing", &Uint32Type, eLapTimeStanding, 1000, 0, "s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("stand_count", &Uint16Type, eLapStandCount, 0, 0, ""));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_left_pco", &Sint8Type, eLapAvgLeftPco, 0, 0, "mm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_right_pco", &Sint8Type, eLapAvgRightPco, 0, 0, "mm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_left_power_phase", &Uint8Type, eLapAvgLeftPowerPhase, 0.7111111, 0, "degrees"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_left_power_phase_peak", &Uint8Type, eLapAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_right_power_phase", &Uint8Type, eLapAvgRightPowerPhase, 0.7111111, 0, "degrees"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_right_power_phase_peak", &Uint8Type, eLapAvgRightPowerPhasePeak, 0.7111111, 0, "degrees"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_power_position", &Uint16Type, eLapAvgPowerPosition, 0, 0, "watts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_power_position", &Uint16Type, eLapMaxPowerPosition, 0, 0, "watts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_cadence_position", &Uint8Type, eLapAvgCadencePosition, 0, 0, "rpm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_cadence_position", &Uint8Type, eLapMaxCadencePosition, 0, 0, "rpm"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("enhanced_avg_speed", &Uint32Type, eLapEnhancedAvgSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("enhanced_max_speed", &Uint32Type, eLapEnhancedMaxSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("enhanced_avg_altitude", &Uint32Type, eLapEnhancedAvgAltitude, 5, 500, "m"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("enhanced_min_altitude", &Uint32Type, eLapEnhancedMinAltitude, 5, 500, "m"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("enhanced_max_altitude", &Uint32Type, eLapEnhancedMaxAltitude, 5, 500, "m"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("avg_lev_motor_power", &Uint16Type, eLapAvgLevMotorPower, 0, 0, "watts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("max_lev_motor_power", &Uint16Type, eLapMaxLevMotorPower, 0, 0, "watts"));
    profiles[eMesgNumLap]->addField(new CFitFieldProfile("lev_battery_consumption", &Uint8Type, eLapLevBatteryConsumption, 2, 0, "percent"));
}

void initLength(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumLength] = new CFitProfile("length", eMesgNumLength);
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("message_index", &EnumType, eLengthMessageIndex, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("timestamp", &EnumType, eLengthTimestamp, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("event", &EnumType, eLengthEvent, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("event_type", &EnumType, eLengthEventType, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("start_time", &EnumType, eLengthStartTime, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("total_elapsed_time", &Uint32Type, eLengthTotalElapsedTime, 1000, 0, "s"));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, eLengthTotalTimerTime, 1000, 0, "s"));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("total_strokes", &Uint16Type, eLengthTotalStrokes, 0, 0, "strokes"));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("avg_speed", &Uint16Type, eLengthAvgSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("swim_stroke", &EnumType, eLengthSwimStroke, 0, 0, "swim_stroke"));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("avg_swimming_cadence", &Uint8Type, eLengthAvgSwimmingCadence, 0, 0, "strokes/min"));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("event_group", &Uint8Type, eLengthEventGroup, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("total_calories", &Uint16Type, eLengthTotalCalories, 0, 0, "kcal"));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("length_type", &EnumType, eLengthLengthType, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("player_score", &Uint16Type, eLengthPlayerScore, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("opponent_score", &Uint16Type, eLengthOpponentScore, 0, 0, ""));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("stroke_count", &Uint16Type, eLengthStrokeCount, 0, 0, "counts"));
    profiles[eMesgNumLength]->addField(new CFitFieldProfile("zone_count", &Uint16Type, eLengthZoneCount, 0, 0, "counts"));
}

void initRecord(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumRecord] = new CFitProfile("record", eMesgNumRecord);
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("timestamp", &EnumType, eRecordTimestamp, 0, 0, "s"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("position_lat", &Sint32Type, eRecordPositionLat, 0, 0, "semicircles"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("position_long", &Sint32Type, eRecordPositionLong, 0, 0, "semicircles"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("altitude", &Uint16Type, eRecordAltitude, 0, 0, ""));
    profiles[eMesgNumRecord]->addComponent(eRecordAltitude, new CFitComponentfieldProfile("altitude", &Uint16Type, eRecordAltitude, 5, 500, "m", eRecordEnhancedAltitude, 16));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("heart_rate", &Uint8Type, eRecordHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("cadence", &Uint8Type, eRecordCadence, 0, 0, "rpm"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("distance", &Uint32Type, eRecordDistance, 100, 0, "m"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("speed", &Uint16Type, eRecordSpeed, 0, 0, ""));
    profiles[eMesgNumRecord]->addComponent(eRecordSpeed, new CFitComponentfieldProfile("speed", &Uint16Type, eRecordSpeed, 1000, 0, "m/s", eRecordEnhancedSpeed, 16));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("power", &Uint16Type, eRecordPower, 0, 0, "watts"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("compressed_speed_distance", &ByteType, eRecordCompressedSpeedDistance, 0, 0, ""));
    profiles[eMesgNumRecord]->addComponent(eRecordCompressedSpeedDistance, new CFitComponentfieldProfile("compressed_speed_distance", &ByteType, eRecordCompressedSpeedDistance, 100, 0, "m/s", eRecordSpeed, 12));
    profiles[eMesgNumRecord]->addComponent(eRecordCompressedSpeedDistance, new CFitComponentfieldProfile("compressed_speed_distance", &ByteType, eRecordCompressedSpeedDistance, 16, 0, "m", eRecordDistance, 12));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("grade", &Sint16Type, eRecordGrade, 100, 0, "%"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("resistance", &Uint8Type, eRecordResistance, 0, 0, ""));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("time_from_course", &Sint32Type, eRecordTimeFromCourse, 1000, 0, "s"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("cycle_length", &Uint8Type, eRecordCycleLength, 100, 0, "m"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("temperature", &Sint8Type, eRecordTemperature, 0, 0, "C"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("speed_1s", &Uint8Type, eRecordSpeed1s, 16, 0, "m/s"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("cycles", &Uint8Type, eRecordCycles, 0, 0, ""));
    profiles[eMesgNumRecord]->addComponent(eRecordCycles, new CFitComponentfieldProfile("cycles", &Uint8Type, eRecordCycles, 0, 0, "cycles", eRecordTotalCycles, 8));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("total_cycles", &Uint32Type, eRecordTotalCycles, 0, 0, "cycles"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("compressed_accumulated_power", &Uint16Type, eRecordCompressedAccumulatedPower, 0, 0, ""));
    profiles[eMesgNumRecord]->addComponent(eRecordCompressedAccumulatedPower, new CFitComponentfieldProfile("compressed_accumulated_power", &Uint16Type, eRecordCompressedAccumulatedPower, 0, 0, "watts", eRecordAccumulatedPower, 16));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("accumulated_power", &Uint32Type, eRecordAccumulatedPower, 0, 0, "watts"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("left_right_balance", &EnumType, eRecordLeftRightBalance, 0, 0, ""));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("gps_accuracy", &Uint8Type, eRecordGpsAccuracy, 0, 0, "m"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("vertical_speed", &Sint16Type, eRecordVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("calories", &Uint16Type, eRecordCalories, 0, 0, "kcal"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("vertical_oscillation", &Uint16Type, eRecordVerticalOscillation, 10, 0, "mm"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("stance_time_percent", &Uint16Type, eRecordStanceTimePercent, 100, 0, "percent"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("stance_time", &Uint16Type, eRecordStanceTime, 10, 0, "ms"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("activity_type", &EnumType, eRecordActivityType, 0, 0, ""));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("left_torque_effectiveness", &Uint8Type, eRecordLeftTorqueEffectiveness, 2, 0, "percent"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("right_torque_effectiveness", &Uint8Type, eRecordRightTorqueEffectiveness, 2, 0, "percent"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("left_pedal_smoothness", &Uint8Type, eRecordLeftPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("right_pedal_smoothness", &Uint8Type, eRecordRightPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("combined_pedal_smoothness", &Uint8Type, eRecordCombinedPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("time128", &Uint8Type, eRecordTime128, 128, 0, "s"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("stroke_type", &EnumType, eRecordStrokeType, 0, 0, ""));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("zone", &Uint8Type, eRecordZone, 0, 0, ""));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("ball_speed", &Uint16Type, eRecordBallSpeed, 100, 0, "m/s"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("cadence256", &Uint16Type, eRecordCadence256, 256, 0, "rpm"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("fractional_cadence", &Uint8Type, eRecordFractionalCadence, 128, 0, "rpm"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("total_hemoglobin_conc", &Uint16Type, eRecordTotalHemoglobinConc, 100, 0, "g/dL"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("total_hemoglobin_conc_min", &Uint16Type, eRecordTotalHemoglobinConcMin, 100, 0, "g/dL"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("total_hemoglobin_conc_max", &Uint16Type, eRecordTotalHemoglobinConcMax, 100, 0, "g/dL"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("saturated_hemoglobin_percent", &Uint16Type, eRecordSaturatedHemoglobinPercent, 10, 0, "%"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("saturated_hemoglobin_percent_min", &Uint16Type, eRecordSaturatedHemoglobinPercentMin, 10, 0, "%"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("saturated_hemoglobin_percent_max", &Uint16Type, eRecordSaturatedHemoglobinPercentMax, 10, 0, "%"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("device_index", &EnumType, eRecordDeviceIndex, 0, 0, ""));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("left_pco", &Sint8Type, eRecordLeftPco, 0, 0, "mm"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("right_pco", &Sint8Type, eRecordRightPco, 0, 0, "mm"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("left_power_phase", &Uint8Type, eRecordLeftPowerPhase, 0.7111111, 0, "degrees"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("left_power_phase_peak", &Uint8Type, eRecordLeftPowerPhasePeak, 0.7111111, 0, "degrees"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("right_power_phase", &Uint8Type, eRecordRightPowerPhase, 0.7111111, 0, "degrees"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("right_power_phase_peak", &Uint8Type, eRecordRightPowerPhasePeak, 0.7111111, 0, "degrees"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("enhanced_speed", &Uint32Type, eRecordEnhancedSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("enhanced_altitude", &Uint32Type, eRecordEnhancedAltitude, 5, 500, "m"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("battery_soc", &Uint8Type, eRecordBatterySoc, 2, 0, "percent"));
    profiles[eMesgNumRecord]->addField(new CFitFieldProfile("motor_power", &Uint16Type, eRecordMotorPower, 0, 0, "watts"));
}

void initEvent(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumEvent] = new CFitProfile("event", eMesgNumEvent);
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("timestamp", &EnumType, eEventTimestamp, 0, 0, "s"));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("event", &EnumType, eEventEvent, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("event_type", &EnumType, eEventEventType, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("data16", &Uint16Type, eEventData16, 0, 0, ""));
    profiles[eMesgNumEvent]->addComponent(eEventData16, new CFitComponentfieldProfile("data16", &Uint16Type, eEventData16, 0, 0, "", eEventData, 16));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("data", &Uint32Type, eEventData, 0, 0, ""));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("timer_trigger", &EnumType, eEventData, 0, 0, "", eEventEvent, eEventTimer));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("course_point_index", &EnumType, eEventData, 0, 0, "", eEventEvent, eEventCoursePoint));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("battery_level", &Uint16Type, eEventData, 1000, 0, "V", eEventEvent, eEventBattery));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("virtual_partner_speed", &Uint16Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventVirtualPartnerPace));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("hr_high_alert", &Uint8Type, eEventData, 0, 0, "bpm", eEventEvent, eEventHrHighAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("hr_low_alert", &Uint8Type, eEventData, 0, 0, "bpm", eEventEvent, eEventHrLowAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("speed_high_alert", &Uint32Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventSpeedHighAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("speed_low_alert", &Uint32Type, eEventData, 1000, 0, "m/s", eEventEvent, eEventSpeedLowAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("cad_high_alert", &Uint16Type, eEventData, 0, 0, "rpm", eEventEvent, eEventCadHighAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("cad_low_alert", &Uint16Type, eEventData, 0, 0, "rpm", eEventEvent, eEventCadLowAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("power_high_alert", &Uint16Type, eEventData, 0, 0, "watts", eEventEvent, eEventPowerHighAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("power_low_alert", &Uint16Type, eEventData, 0, 0, "watts", eEventEvent, eEventPowerLowAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("time_duration_alert", &Uint32Type, eEventData, 1000, 0, "s", eEventEvent, eEventTimeDurationAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("distance_duration_alert", &Uint32Type, eEventData, 100, 0, "m", eEventEvent, eEventDistanceDurationAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("calorie_duration_alert", &Uint32Type, eEventData, 0, 0, "calories", eEventEvent, eEventCalorieDurationAlert));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("fitness_equipment_state", &EnumType, eEventData, 0, 0, "", eEventEvent, eEventFitnessEquipment));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("sport_point", &Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventSportPoint));
    profiles[eMesgNumEvent]->addComponent(eEventData, 16, new CFitComponentfieldProfile("sport_point", &Uint32Type, eEventData, 1, 0, "", eEventScore, 16));
    profiles[eMesgNumEvent]->addComponent(eEventData, 16, new CFitComponentfieldProfile("sport_point", &Uint32Type, eEventData, 1, 0, "", eEventOpponentScore, 16));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("gear_change_data", &Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventFrontGearChange));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("gear_change_data", &Uint32Type, eEventData, 0, 0, "", eEventEvent, eEventRearGearChange));
    profiles[eMesgNumEvent]->addComponent(eEventData, 18, new CFitComponentfieldProfile("gear_change_data", &Uint32Type, eEventData, 1, 0, "", eEventRearGearNum, 8));
    profiles[eMesgNumEvent]->addComponent(eEventData, 18, new CFitComponentfieldProfile("gear_change_data", &Uint32Type, eEventData, 1, 0, "", eEventRearGear, 8));
    profiles[eMesgNumEvent]->addComponent(eEventData, 18, new CFitComponentfieldProfile("gear_change_data", &Uint32Type, eEventData, 1, 0, "", eEventFrontGearNum, 8));
    profiles[eMesgNumEvent]->addComponent(eEventData, 18, new CFitComponentfieldProfile("gear_change_data", &Uint32Type, eEventData, 1, 0, "", eEventFrontGear, 8));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("rider_position", &EnumType, eEventData, 0, 0, "", eEventEvent, eEventRiderPositionChange));
    profiles[eMesgNumEvent]->addSubfield(eEventData, new CFitSubfieldProfile("comm_timeout", &EnumType, eEventData, 0, 0, "", eEventEvent, eEventCommTimeout));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("event_group", &Uint8Type, eEventEventGroup, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("score", &Uint16Type, eEventScore, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("opponent_score", &Uint16Type, eEventOpponentScore, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("front_gear_num", &Uint8zType, eEventFrontGearNum, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("front_gear", &Uint8zType, eEventFrontGear, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("rear_gear_num", &Uint8zType, eEventRearGearNum, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("rear_gear", &Uint8zType, eEventRearGear, 0, 0, ""));
    profiles[eMesgNumEvent]->addField(new CFitFieldProfile("device_index", &EnumType, eEventDeviceIndex, 0, 0, ""));
}

void initDeviceInfo(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumDeviceInfo] = new CFitProfile("device_info", eMesgNumDeviceInfo);
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("timestamp", &EnumType, eDeviceInfoTimestamp, 0, 0, "s"));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("device_index", &EnumType, eDeviceInfoDeviceIndex, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("device_type", &Uint8Type, eDeviceInfoDeviceType, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addSubfield(eDeviceInfoDeviceType, new CFitSubfieldProfile("antplus_device_type", &EnumType, eDeviceInfoDeviceType, 0, 0, "", eDeviceInfoSourceType, eSourceTypeAntplus));
    profiles[eMesgNumDeviceInfo]->addSubfield(eDeviceInfoDeviceType, new CFitSubfieldProfile("ant_device_type", &Uint8Type, eDeviceInfoDeviceType, 0, 0, "", eDeviceInfoSourceType, eSourceTypeAnt));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("manufacturer", &EnumType, eDeviceInfoManufacturer, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("serial_number", &Uint32zType, eDeviceInfoSerialNumber, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("product", &Uint16Type, eDeviceInfoProduct, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addSubfield(eDeviceInfoProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerGarmin));
    profiles[eMesgNumDeviceInfo]->addSubfield(eDeviceInfoProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerDynastream));
    profiles[eMesgNumDeviceInfo]->addSubfield(eDeviceInfoProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eDeviceInfoProduct, 0, 0, "", eDeviceInfoManufacturer, eManufacturerDynastreamOem));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("software_version", &Uint16Type, eDeviceInfoSoftwareVersion, 100, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("hardware_version", &Uint8Type, eDeviceInfoHardwareVersion, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("cum_operating_time", &Uint32Type, eDeviceInfoCumOperatingTime, 0, 0, "s"));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("battery_voltage", &Uint16Type, eDeviceInfoBatteryVoltage, 256, 0, "V"));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("battery_status", &EnumType, eDeviceInfoBatteryStatus, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("sensor_position", &EnumType, eDeviceInfoSensorPosition, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("descriptor", &StringType, eDeviceInfoDescriptor, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("ant_transmission_type", &Uint8zType, eDeviceInfoAntTransmissionType, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("ant_device_number", &Uint16zType, eDeviceInfoAntDeviceNumber, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("ant_network", &EnumType, eDeviceInfoAntNetwork, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("source_type", &EnumType, eDeviceInfoSourceType, 0, 0, ""));
    profiles[eMesgNumDeviceInfo]->addField(new CFitFieldProfile("product_name", &StringType, eDeviceInfoProductName, 0, 0, ""));
}

void initTrainingFile(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumTrainingFile] = new CFitProfile("training_file", eMesgNumTrainingFile);
    profiles[eMesgNumTrainingFile]->addField(new CFitFieldProfile("timestamp", &EnumType, eTrainingFileTimestamp, 0, 0, ""));
    profiles[eMesgNumTrainingFile]->addField(new CFitFieldProfile("type", &EnumType, eTrainingFileType, 0, 0, ""));
    profiles[eMesgNumTrainingFile]->addField(new CFitFieldProfile("manufacturer", &EnumType, eTrainingFileManufacturer, 0, 0, ""));
    profiles[eMesgNumTrainingFile]->addField(new CFitFieldProfile("product", &Uint16Type, eTrainingFileProduct, 0, 0, ""));
    profiles[eMesgNumTrainingFile]->addSubfield(eTrainingFileProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerGarmin));
    profiles[eMesgNumTrainingFile]->addSubfield(eTrainingFileProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerDynastream));
    profiles[eMesgNumTrainingFile]->addSubfield(eTrainingFileProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eTrainingFileProduct, 0, 0, "", eTrainingFileManufacturer, eManufacturerDynastreamOem));
    profiles[eMesgNumTrainingFile]->addField(new CFitFieldProfile("serial_number", &Uint32zType, eTrainingFileSerialNumber, 0, 0, ""));
    profiles[eMesgNumTrainingFile]->addField(new CFitFieldProfile("time_created", &EnumType, eTrainingFileTimeCreated, 0, 0, ""));
}

void initHrv(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumHrv] = new CFitProfile("hrv", eMesgNumHrv);
    profiles[eMesgNumHrv]->addField(new CFitFieldProfile("time", &Uint16Type, eHrvTime, 1000, 0, "s"));
}

void initCameraEvent(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumCameraEvent] = new CFitProfile("camera_event", eMesgNumCameraEvent);
    profiles[eMesgNumCameraEvent]->addField(new CFitFieldProfile("timestamp", &EnumType, eCameraEventTimestamp, 0, 0, "s"));
    profiles[eMesgNumCameraEvent]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, eCameraEventTimestampMs, 0, 0, "ms"));
    profiles[eMesgNumCameraEvent]->addField(new CFitFieldProfile("camera_event_type", &EnumType, eCameraEventCameraEventType, 0, 0, ""));
    profiles[eMesgNumCameraEvent]->addField(new CFitFieldProfile("camera_file_uuid", &StringType, eCameraEventCameraFileUuid, 0, 0, ""));
    profiles[eMesgNumCameraEvent]->addField(new CFitFieldProfile("camera_orientation", &EnumType, eCameraEventCameraOrientation, 0, 0, ""));
}

void initGyroscopeData(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumGyroscopeData] = new CFitProfile("gyroscope_data", eMesgNumGyroscopeData);
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("timestamp", &EnumType, eGyroscopeDataTimestamp, 0, 0, "s"));
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, eGyroscopeDataTimestampMs, 0, 0, "ms"));
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("sample_time_offset", &Uint16Type, eGyroscopeDataSampleTimeOffset, 0, 0, "ms"));
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("gyro_x", &Uint16Type, eGyroscopeDataGyroX, 0, 0, "counts"));
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("gyro_y", &Uint16Type, eGyroscopeDataGyroY, 0, 0, "counts"));
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("gyro_z", &Uint16Type, eGyroscopeDataGyroZ, 0, 0, "counts"));
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("calibrated_gyro_x", &Float32Type, eGyroscopeDataCalibratedGyroX, 0, 0, "deg/s"));
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("calibrated_gyro_y", &Float32Type, eGyroscopeDataCalibratedGyroY, 0, 0, "deg/s"));
    profiles[eMesgNumGyroscopeData]->addField(new CFitFieldProfile("calibrated_gyro_z", &Float32Type, eGyroscopeDataCalibratedGyroZ, 0, 0, "deg/s"));
}

void initAccelerometerData(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumAccelerometerData] = new CFitProfile("accelerometer_data", eMesgNumAccelerometerData);
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("timestamp", &EnumType, eAccelerometerDataTimestamp, 0, 0, "s"));
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, eAccelerometerDataTimestampMs, 0, 0, "ms"));
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("sample_time_offset", &Uint16Type, eAccelerometerDataSampleTimeOffset, 0, 0, "ms"));
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("accel_x", &Uint16Type, eAccelerometerDataAccelX, 0, 0, "counts"));
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("accel_y", &Uint16Type, eAccelerometerDataAccelY, 0, 0, "counts"));
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("accel_z", &Uint16Type, eAccelerometerDataAccelZ, 0, 0, "counts"));
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("calibrated_accel_x", &Float32Type, eAccelerometerDataCalibratedAccelX, 0, 0, "g"));
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("calibrated_accel_y", &Float32Type, eAccelerometerDataCalibratedAccelY, 0, 0, "g"));
    profiles[eMesgNumAccelerometerData]->addField(new CFitFieldProfile("calibrated_accel_z", &Float32Type, eAccelerometerDataCalibratedAccelZ, 0, 0, "g"));
}

void initThreeDSensorCalibration(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumThreeDSensorCalibration] = new CFitProfile("three_d_sensor_calibration", eMesgNumThreeDSensorCalibration);
    profiles[eMesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("timestamp", &EnumType, eThreeDSensorCalibrationTimestamp, 0, 0, "s"));
    profiles[eMesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("sensor_type", &EnumType, eThreeDSensorCalibrationSensorType, 0, 0, ""));
    profiles[eMesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("calibration_factor", &Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, ""));
    profiles[eMesgNumThreeDSensorCalibration]->addSubfield(eThreeDSensorCalibrationCalibrationFactor, new CFitSubfieldProfile("accel_cal_factor", &Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "g", eThreeDSensorCalibrationSensorType, eSensorTypeAccelerometer));
    profiles[eMesgNumThreeDSensorCalibration]->addSubfield(eThreeDSensorCalibrationCalibrationFactor, new CFitSubfieldProfile("gyro_cal_factor", &Uint32Type, eThreeDSensorCalibrationCalibrationFactor, 0, 0, "deg/s", eThreeDSensorCalibrationSensorType, eSensorTypeGyroscope));
    profiles[eMesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("calibration_divisor", &Uint32Type, eThreeDSensorCalibrationCalibrationDivisor, 0, 0, "counts"));
    profiles[eMesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("level_shift", &Uint32Type, eThreeDSensorCalibrationLevelShift, 0, 0, ""));
    profiles[eMesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("offset_cal", &Sint32Type, eThreeDSensorCalibrationOffsetCal, 0, 0, ""));
    profiles[eMesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("orientation_matrix", &Sint32Type, eThreeDSensorCalibrationOrientationMatrix, 65535, 0, ""));
}

void initVideoFrame(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumVideoFrame] = new CFitProfile("video_frame", eMesgNumVideoFrame);
    profiles[eMesgNumVideoFrame]->addField(new CFitFieldProfile("timestamp", &EnumType, eVideoFrameTimestamp, 0, 0, "s"));
    profiles[eMesgNumVideoFrame]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, eVideoFrameTimestampMs, 0, 0, "ms"));
    profiles[eMesgNumVideoFrame]->addField(new CFitFieldProfile("frame_number", &Uint32Type, eVideoFrameFrameNumber, 0, 0, ""));
}

void initObdiiData(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumObdiiData] = new CFitProfile("obdii_data", eMesgNumObdiiData);
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("timestamp", &EnumType, eObdiiDataTimestamp, 0, 0, "s"));
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, eObdiiDataTimestampMs, 0, 0, "ms"));
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("time_offset", &Uint16Type, eObdiiDataTimeOffset, 0, 0, "ms"));
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("pid", &ByteType, eObdiiDataPid, 0, 0, ""));
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("raw_data", &ByteType, eObdiiDataRawData, 0, 0, ""));
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("pid_data_size", &Uint8Type, eObdiiDataPidDataSize, 0, 0, ""));
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("system_time", &Uint32Type, eObdiiDataSystemTime, 0, 0, ""));
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("start_timestamp", &EnumType, eObdiiDataStartTimestamp, 0, 0, ""));
    profiles[eMesgNumObdiiData]->addField(new CFitFieldProfile("start_timestamp_ms", &Uint16Type, eObdiiDataStartTimestampMs, 0, 0, "ms"));
}

void initNmeaSentence(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumNmeaSentence] = new CFitProfile("nmea_sentence", eMesgNumNmeaSentence);
    profiles[eMesgNumNmeaSentence]->addField(new CFitFieldProfile("timestamp", &EnumType, eNmeaSentenceTimestamp, 0, 0, "s"));
    profiles[eMesgNumNmeaSentence]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, eNmeaSentenceTimestampMs, 0, 0, "ms"));
    profiles[eMesgNumNmeaSentence]->addField(new CFitFieldProfile("sentence", &StringType, eNmeaSentenceSentence, 0, 0, ""));
}

void initAviationAttitude(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumAviationAttitude] = new CFitProfile("aviation_attitude", eMesgNumAviationAttitude);
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("timestamp", &EnumType, eAviationAttitudeTimestamp, 0, 0, "s"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, eAviationAttitudeTimestampMs, 0, 0, "ms"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("system_time", &Uint32Type, eAviationAttitudeSystemTime, 0, 0, "ms"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("pitch", &Sint16Type, eAviationAttitudePitch, 10430.38, 0, "radians"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("roll", &Sint16Type, eAviationAttitudeRoll, 10430.38, 0, "radians"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("accel_lateral", &Sint16Type, eAviationAttitudeAccelLateral, 100, 0, "m/s^2"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("accel_normal", &Sint16Type, eAviationAttitudeAccelNormal, 100, 0, "m/s^2"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("turn_rate", &Sint16Type, eAviationAttitudeTurnRate, 1024, 0, "radians/second"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("stage", &EnumType, eAviationAttitudeStage, 0, 0, ""));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("attitude_stage_complete", &Uint8Type, eAviationAttitudeAttitudeStageComplete, 0, 0, "%"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("track", &Uint16Type, eAviationAttitudeTrack, 10430.38, 0, "radians"));
    profiles[eMesgNumAviationAttitude]->addField(new CFitFieldProfile("validity", &EnumType, eAviationAttitudeValidity, 0, 0, ""));
}

void initVideo(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumVideo] = new CFitProfile("video", eMesgNumVideo);
    profiles[eMesgNumVideo]->addField(new CFitFieldProfile("url", &StringType, eVideoUrl, 0, 0, ""));
    profiles[eMesgNumVideo]->addField(new CFitFieldProfile("hosting_provider", &StringType, eVideoHostingProvider, 0, 0, ""));
    profiles[eMesgNumVideo]->addField(new CFitFieldProfile("duration", &Uint32Type, eVideoDuration, 0, 0, "ms"));
}

void initVideoTitle(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumVideoTitle] = new CFitProfile("video_title", eMesgNumVideoTitle);
    profiles[eMesgNumVideoTitle]->addField(new CFitFieldProfile("message_index", &EnumType, eVideoTitleMessageIndex, 0, 0, ""));
    profiles[eMesgNumVideoTitle]->addField(new CFitFieldProfile("message_count", &Uint16Type, eVideoTitleMessageCount, 0, 0, ""));
    profiles[eMesgNumVideoTitle]->addField(new CFitFieldProfile("text", &StringType, eVideoTitleText, 0, 0, ""));
}

void initVideoDescription(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumVideoDescription] = new CFitProfile("video_description", eMesgNumVideoDescription);
    profiles[eMesgNumVideoDescription]->addField(new CFitFieldProfile("message_index", &EnumType, eVideoDescriptionMessageIndex, 0, 0, ""));
    profiles[eMesgNumVideoDescription]->addField(new CFitFieldProfile("message_count", &Uint16Type, eVideoDescriptionMessageCount, 0, 0, ""));
    profiles[eMesgNumVideoDescription]->addField(new CFitFieldProfile("text", &StringType, eVideoDescriptionText, 0, 0, ""));
}

void initVideoClip(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumVideoClip] = new CFitProfile("video_clip", eMesgNumVideoClip);
    profiles[eMesgNumVideoClip]->addField(new CFitFieldProfile("clip_number", &Uint16Type, eVideoClipClipNumber, 0, 0, ""));
    profiles[eMesgNumVideoClip]->addField(new CFitFieldProfile("start_timestamp", &EnumType, eVideoClipStartTimestamp, 0, 0, ""));
    profiles[eMesgNumVideoClip]->addField(new CFitFieldProfile("start_timestamp_ms", &Uint16Type, eVideoClipStartTimestampMs, 0, 0, ""));
    profiles[eMesgNumVideoClip]->addField(new CFitFieldProfile("end_timestamp", &EnumType, eVideoClipEndTimestamp, 0, 0, ""));
    profiles[eMesgNumVideoClip]->addField(new CFitFieldProfile("end_timestamp_ms", &Uint16Type, eVideoClipEndTimestampMs, 0, 0, ""));
    profiles[eMesgNumVideoClip]->addField(new CFitFieldProfile("clip_start", &Uint32Type, eVideoClipClipStart, 0, 0, "ms"));
    profiles[eMesgNumVideoClip]->addField(new CFitFieldProfile("clip_end", &Uint32Type, eVideoClipClipEnd, 0, 0, "ms"));
}

void initCourse(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumCourse] = new CFitProfile("course", eMesgNumCourse);
    profiles[eMesgNumCourse]->addField(new CFitFieldProfile("sport", &EnumType, eCourseSport, 0, 0, ""));
    profiles[eMesgNumCourse]->addField(new CFitFieldProfile("name", &StringType, eCourseName, 0, 0, ""));
    profiles[eMesgNumCourse]->addField(new CFitFieldProfile("capabilities", &EnumType, eCourseCapabilities, 0, 0, ""));
}

void initCoursePoint(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumCoursePoint] = new CFitProfile("course_point", eMesgNumCoursePoint);
    profiles[eMesgNumCoursePoint]->addField(new CFitFieldProfile("message_index", &EnumType, eCoursePointMessageIndex, 0, 0, ""));
    profiles[eMesgNumCoursePoint]->addField(new CFitFieldProfile("timestamp", &EnumType, eCoursePointTimestamp, 0, 0, ""));
    profiles[eMesgNumCoursePoint]->addField(new CFitFieldProfile("position_lat", &Sint32Type, eCoursePointPositionLat, 0, 0, "semicircles"));
    profiles[eMesgNumCoursePoint]->addField(new CFitFieldProfile("position_long", &Sint32Type, eCoursePointPositionLong, 0, 0, "semicircles"));
    profiles[eMesgNumCoursePoint]->addField(new CFitFieldProfile("distance", &Uint32Type, eCoursePointDistance, 100, 0, "m"));
    profiles[eMesgNumCoursePoint]->addField(new CFitFieldProfile("type", &EnumType, eCoursePointType, 0, 0, ""));
    profiles[eMesgNumCoursePoint]->addField(new CFitFieldProfile("name", &StringType, eCoursePointName, 0, 0, ""));
    profiles[eMesgNumCoursePoint]->addField(new CFitFieldProfile("favorite", &EnumType, eCoursePointFavorite, 0, 0, ""));
}

void initSegmentId(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSegmentId] = new CFitProfile("segment_id", eMesgNumSegmentId);
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("name", &StringType, eSegmentIdName, 0, 0, ""));
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("uuid", &StringType, eSegmentIdUuid, 0, 0, ""));
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("sport", &EnumType, eSegmentIdSport, 0, 0, ""));
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("enabled", &EnumType, eSegmentIdEnabled, 0, 0, ""));
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("user_profile_primary_key", &Uint32Type, eSegmentIdUserProfilePrimaryKey, 0, 0, ""));
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("device_id", &Uint32Type, eSegmentIdDeviceId, 0, 0, ""));
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("default_race_leader", &Uint8Type, eSegmentIdDefaultRaceLeader, 0, 0, ""));
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("delete_status", &EnumType, eSegmentIdDeleteStatus, 0, 0, ""));
    profiles[eMesgNumSegmentId]->addField(new CFitFieldProfile("selection_type", &EnumType, eSegmentIdSelectionType, 0, 0, ""));
}

void initSegmentLeaderboardEntry(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSegmentLeaderboardEntry] = new CFitProfile("segment_leaderboard_entry", eMesgNumSegmentLeaderboardEntry);
    profiles[eMesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("message_index", &EnumType, eSegmentLeaderboardEntryMessageIndex, 0, 0, ""));
    profiles[eMesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("name", &StringType, eSegmentLeaderboardEntryName, 0, 0, ""));
    profiles[eMesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("type", &EnumType, eSegmentLeaderboardEntryType, 0, 0, ""));
    profiles[eMesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("group_primary_key", &Uint32Type, eSegmentLeaderboardEntryGroupPrimaryKey, 0, 0, ""));
    profiles[eMesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("activity_id", &Uint32Type, eSegmentLeaderboardEntryActivityId, 0, 0, ""));
    profiles[eMesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("segment_time", &Uint32Type, eSegmentLeaderboardEntrySegmentTime, 1000, 0, "s"));
    profiles[eMesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("activity_id_string", &StringType, eSegmentLeaderboardEntryActivityIdString, 0, 0, ""));
}

void initSegmentPoint(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSegmentPoint] = new CFitProfile("segment_point", eMesgNumSegmentPoint);
    profiles[eMesgNumSegmentPoint]->addField(new CFitFieldProfile("message_index", &EnumType, eSegmentPointMessageIndex, 0, 0, ""));
    profiles[eMesgNumSegmentPoint]->addField(new CFitFieldProfile("position_lat", &Sint32Type, eSegmentPointPositionLat, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentPoint]->addField(new CFitFieldProfile("position_long", &Sint32Type, eSegmentPointPositionLong, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentPoint]->addField(new CFitFieldProfile("distance", &Uint32Type, eSegmentPointDistance, 100, 0, "m"));
    profiles[eMesgNumSegmentPoint]->addField(new CFitFieldProfile("altitude", &Uint16Type, eSegmentPointAltitude, 5, 500, "m"));
    profiles[eMesgNumSegmentPoint]->addField(new CFitFieldProfile("leader_time", &Uint32Type, eSegmentPointLeaderTime, 1000, 0, "s"));
}

void initSegmentLap(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSegmentLap] = new CFitProfile("segment_lap", eMesgNumSegmentLap);
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("message_index", &EnumType, eSegmentLapMessageIndex, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("timestamp", &EnumType, eSegmentLapTimestamp, 0, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("event", &EnumType, eSegmentLapEvent, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("event_type", &EnumType, eSegmentLapEventType, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("start_time", &EnumType, eSegmentLapStartTime, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("start_position_lat", &Sint32Type, eSegmentLapStartPositionLat, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("start_position_long", &Sint32Type, eSegmentLapStartPositionLong, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("end_position_lat", &Sint32Type, eSegmentLapEndPositionLat, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("end_position_long", &Sint32Type, eSegmentLapEndPositionLong, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_elapsed_time", &Uint32Type, eSegmentLapTotalElapsedTime, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, eSegmentLapTotalTimerTime, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_distance", &Uint32Type, eSegmentLapTotalDistance, 100, 0, "m"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_cycles", &Uint32Type, eSegmentLapTotalCycles, 0, 0, "cycles"));
    profiles[eMesgNumSegmentLap]->addSubfield(eSegmentLapTotalCycles, new CFitSubfieldProfile("total_strokes", &Uint32Type, eSegmentLapTotalCycles, 0, 0, "strokes", eSegmentLapSport, eSportCycling));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_calories", &Uint16Type, eSegmentLapTotalCalories, 0, 0, "kcal"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_fat_calories", &Uint16Type, eSegmentLapTotalFatCalories, 0, 0, "kcal"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_speed", &Uint16Type, eSegmentLapAvgSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_speed", &Uint16Type, eSegmentLapMaxSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_heart_rate", &Uint8Type, eSegmentLapAvgHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_heart_rate", &Uint8Type, eSegmentLapMaxHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_cadence", &Uint8Type, eSegmentLapAvgCadence, 0, 0, "rpm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_cadence", &Uint8Type, eSegmentLapMaxCadence, 0, 0, "rpm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_power", &Uint16Type, eSegmentLapAvgPower, 0, 0, "watts"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_power", &Uint16Type, eSegmentLapMaxPower, 0, 0, "watts"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_ascent", &Uint16Type, eSegmentLapTotalAscent, 0, 0, "m"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_descent", &Uint16Type, eSegmentLapTotalDescent, 0, 0, "m"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("sport", &EnumType, eSegmentLapSport, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("event_group", &Uint8Type, eSegmentLapEventGroup, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("nec_lat", &Sint32Type, eSegmentLapNecLat, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("nec_long", &Sint32Type, eSegmentLapNecLong, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("swc_lat", &Sint32Type, eSegmentLapSwcLat, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("swc_long", &Sint32Type, eSegmentLapSwcLong, 0, 0, "semicircles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("name", &StringType, eSegmentLapName, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("normalized_power", &Uint16Type, eSegmentLapNormalizedPower, 0, 0, "watts"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("left_right_balance", &EnumType, eSegmentLapLeftRightBalance, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("sub_sport", &EnumType, eSegmentLapSubSport, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_work", &Uint32Type, eSegmentLapTotalWork, 0, 0, "J"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_altitude", &Uint16Type, eSegmentLapAvgAltitude, 5, 500, "m"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_altitude", &Uint16Type, eSegmentLapMaxAltitude, 5, 500, "m"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("gps_accuracy", &Uint8Type, eSegmentLapGpsAccuracy, 0, 0, "m"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_grade", &Sint16Type, eSegmentLapAvgGrade, 100, 0, "%"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_pos_grade", &Sint16Type, eSegmentLapAvgPosGrade, 100, 0, "%"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_neg_grade", &Sint16Type, eSegmentLapAvgNegGrade, 100, 0, "%"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_pos_grade", &Sint16Type, eSegmentLapMaxPosGrade, 100, 0, "%"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_neg_grade", &Sint16Type, eSegmentLapMaxNegGrade, 100, 0, "%"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_temperature", &Sint8Type, eSegmentLapAvgTemperature, 0, 0, "C"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_temperature", &Sint8Type, eSegmentLapMaxTemperature, 0, 0, "C"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_moving_time", &Uint32Type, eSegmentLapTotalMovingTime, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_pos_vertical_speed", &Sint16Type, eSegmentLapAvgPosVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_neg_vertical_speed", &Sint16Type, eSegmentLapAvgNegVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_pos_vertical_speed", &Sint16Type, eSegmentLapMaxPosVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_neg_vertical_speed", &Sint16Type, eSegmentLapMaxNegVerticalSpeed, 1000, 0, "m/s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("time_in_hr_zone", &Uint32Type, eSegmentLapTimeInHrZone, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("time_in_speed_zone", &Uint32Type, eSegmentLapTimeInSpeedZone, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("time_in_cadence_zone", &Uint32Type, eSegmentLapTimeInCadenceZone, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("time_in_power_zone", &Uint32Type, eSegmentLapTimeInPowerZone, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("repetition_num", &Uint16Type, eSegmentLapRepetitionNum, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("min_altitude", &Uint16Type, eSegmentLapMinAltitude, 5, 500, "m"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("min_heart_rate", &Uint8Type, eSegmentLapMinHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("active_time", &Uint32Type, eSegmentLapActiveTime, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("wkt_step_index", &EnumType, eSegmentLapWktStepIndex, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("sport_event", &EnumType, eSegmentLapSportEvent, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_torque_effectiveness", &Uint8Type, eSegmentLapAvgLeftTorqueEffectiveness, 2, 0, "percent"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_torque_effectiveness", &Uint8Type, eSegmentLapAvgRightTorqueEffectiveness, 2, 0, "percent"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_pedal_smoothness", &Uint8Type, eSegmentLapAvgLeftPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_pedal_smoothness", &Uint8Type, eSegmentLapAvgRightPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_combined_pedal_smoothness", &Uint8Type, eSegmentLapAvgCombinedPedalSmoothness, 2, 0, "percent"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("status", &EnumType, eSegmentLapStatus, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("uuid", &StringType, eSegmentLapUuid, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_fractional_cadence", &Uint8Type, eSegmentLapAvgFractionalCadence, 128, 0, "rpm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_fractional_cadence", &Uint8Type, eSegmentLapMaxFractionalCadence, 128, 0, "rpm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("total_fractional_cycles", &Uint8Type, eSegmentLapTotalFractionalCycles, 128, 0, "cycles"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("front_gear_shift_count", &Uint16Type, eSegmentLapFrontGearShiftCount, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("rear_gear_shift_count", &Uint16Type, eSegmentLapRearGearShiftCount, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("time_standing", &Uint32Type, eSegmentLapTimeStanding, 1000, 0, "s"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("stand_count", &Uint16Type, eSegmentLapStandCount, 0, 0, ""));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_pco", &Sint8Type, eSegmentLapAvgLeftPco, 0, 0, "mm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_pco", &Sint8Type, eSegmentLapAvgRightPco, 0, 0, "mm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_power_phase", &Uint8Type, eSegmentLapAvgLeftPowerPhase, 0.7111111, 0, "degrees"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_power_phase_peak", &Uint8Type, eSegmentLapAvgLeftPowerPhasePeak, 0.7111111, 0, "degrees"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_power_phase", &Uint8Type, eSegmentLapAvgRightPowerPhase, 0.7111111, 0, "degrees"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_power_phase_peak", &Uint8Type, eSegmentLapAvgRightPowerPhasePeak, 0.7111111, 0, "degrees"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_power_position", &Uint16Type, eSegmentLapAvgPowerPosition, 0, 0, "watts"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_power_position", &Uint16Type, eSegmentLapMaxPowerPosition, 0, 0, "watts"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("avg_cadence_position", &Uint8Type, eSegmentLapAvgCadencePosition, 0, 0, "rpm"));
    profiles[eMesgNumSegmentLap]->addField(new CFitFieldProfile("max_cadence_position", &Uint8Type, eSegmentLapMaxCadencePosition, 0, 0, "rpm"));
}

void initSegmentFile(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSegmentFile] = new CFitProfile("segment_file", eMesgNumSegmentFile);
    profiles[eMesgNumSegmentFile]->addField(new CFitFieldProfile("message_index", &EnumType, eSegmentFileMessageIndex, 0, 0, ""));
    profiles[eMesgNumSegmentFile]->addField(new CFitFieldProfile("file_uuid", &StringType, eSegmentFileFileUuid, 0, 0, ""));
    profiles[eMesgNumSegmentFile]->addField(new CFitFieldProfile("enabled", &EnumType, eSegmentFileEnabled, 0, 0, ""));
    profiles[eMesgNumSegmentFile]->addField(new CFitFieldProfile("user_profile_primary_key", &Uint32Type, eSegmentFileUserProfilePrimaryKey, 0, 0, ""));
    profiles[eMesgNumSegmentFile]->addField(new CFitFieldProfile("leader_type", &EnumType, eSegmentFileLeaderType, 0, 0, ""));
    profiles[eMesgNumSegmentFile]->addField(new CFitFieldProfile("leader_group_primary_key", &Uint32Type, eSegmentFileLeaderGroupPrimaryKey, 0, 0, ""));
    profiles[eMesgNumSegmentFile]->addField(new CFitFieldProfile("leader_activity_id", &Uint32Type, eSegmentFileLeaderActivityId, 0, 0, ""));
    profiles[eMesgNumSegmentFile]->addField(new CFitFieldProfile("leader_activity_id_string", &StringType, eSegmentFileLeaderActivityIdString, 0, 0, ""));
}

void initWorkout(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumWorkout] = new CFitProfile("workout", eMesgNumWorkout);
    profiles[eMesgNumWorkout]->addField(new CFitFieldProfile("sport", &EnumType, eWorkoutSport, 0, 0, ""));
    profiles[eMesgNumWorkout]->addField(new CFitFieldProfile("capabilities", &EnumType, eWorkoutCapabilities, 0, 0, ""));
    profiles[eMesgNumWorkout]->addField(new CFitFieldProfile("num_valid_steps", &Uint16Type, eWorkoutNumValidSteps, 0, 0, ""));
    profiles[eMesgNumWorkout]->addField(new CFitFieldProfile("wkt_name", &StringType, eWorkoutWktName, 0, 0, ""));
}

void initWorkoutStep(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumWorkoutStep] = new CFitProfile("workout_step", eMesgNumWorkoutStep);
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("message_index", &EnumType, eWorkoutStepMessageIndex, 0, 0, ""));
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("wkt_step_name", &StringType, eWorkoutStepWktStepName, 0, 0, ""));
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("duration_type", &EnumType, eWorkoutStepDurationType, 0, 0, ""));
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("duration_value", &Uint32Type, eWorkoutStepDurationValue, 0, 0, ""));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_time", &Uint32Type, eWorkoutStepDurationValue, 1000, 0, "s", eWorkoutStepDurationType, eWktStepDurationTime));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_time", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepetitionTime));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_distance", &Uint32Type, eWorkoutStepDurationValue, 100, 0, "m", eWorkoutStepDurationType, eWktStepDurationDistance));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_hr", &EnumType, eWorkoutStepDurationValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationHrLessThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_hr", &EnumType, eWorkoutStepDurationValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationHrGreaterThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_calories", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "calories", eWorkoutStepDurationType, eWktStepDurationCalories));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_step", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilStepsCmplt));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_step", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilTime));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_step", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilDistance));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_step", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilCalories));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_step", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrLessThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_step", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrGreaterThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_step", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerLessThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_step", &Uint32Type, eWorkoutStepDurationValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerGreaterThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_power", &EnumType, eWorkoutStepDurationValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationPowerLessThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepDurationValue, new CFitSubfieldProfile("duration_power", &EnumType, eWorkoutStepDurationValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationPowerGreaterThan));
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("target_type", &EnumType, eWorkoutStepTargetType, 0, 0, ""));
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("target_value", &Uint32Type, eWorkoutStepTargetValue, 0, 0, ""));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("target_hr_zone", &Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetHeartRate));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("target_power_zone", &Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepTargetType, eWktStepTargetPower));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("repeat_steps", &Uint32Type, eWorkoutStepTargetValue, 0, 0, "", eWorkoutStepDurationType, eWktStepDurationRepeatUntilStepsCmplt));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("repeat_time", &Uint32Type, eWorkoutStepTargetValue, 1000, 0, "s", eWorkoutStepDurationType, eWktStepDurationRepeatUntilTime));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("repeat_distance", &Uint32Type, eWorkoutStepTargetValue, 100, 0, "m", eWorkoutStepDurationType, eWktStepDurationRepeatUntilDistance));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("repeat_calories", &Uint32Type, eWorkoutStepTargetValue, 0, 0, "calories", eWorkoutStepDurationType, eWktStepDurationRepeatUntilCalories));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("repeat_hr", &EnumType, eWorkoutStepTargetValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrLessThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("repeat_hr", &EnumType, eWorkoutStepTargetValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationRepeatUntilHrGreaterThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("repeat_power", &EnumType, eWorkoutStepTargetValue, 0, 0, "%", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerLessThan));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepTargetValue, new CFitSubfieldProfile("repeat_power", &EnumType, eWorkoutStepTargetValue, 0, 0, "or", eWorkoutStepDurationType, eWktStepDurationRepeatUntilPowerGreaterThan));
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("custom_target_value_low", &Uint32Type, eWorkoutStepCustomTargetValueLow, 0, 0, ""));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepCustomTargetValueLow, new CFitSubfieldProfile("custom_target_speed_low", &Uint32Type, eWorkoutStepCustomTargetValueLow, 1000, 0, "m/s", eWorkoutStepTargetType, eWktStepTargetSpeed));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepCustomTargetValueLow, new CFitSubfieldProfile("custom_target_heart_rate_low", &EnumType, eWorkoutStepCustomTargetValueLow, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetHeartRate));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepCustomTargetValueLow, new CFitSubfieldProfile("custom_target_cadence_low", &Uint32Type, eWorkoutStepCustomTargetValueLow, 0, 0, "rpm", eWorkoutStepTargetType, eWktStepTargetCadence));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepCustomTargetValueLow, new CFitSubfieldProfile("custom_target_power_low", &EnumType, eWorkoutStepCustomTargetValueLow, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetPower));
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("custom_target_value_high", &Uint32Type, eWorkoutStepCustomTargetValueHigh, 0, 0, ""));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepCustomTargetValueHigh, new CFitSubfieldProfile("custom_target_speed_high", &Uint32Type, eWorkoutStepCustomTargetValueHigh, 1000, 0, "m/s", eWorkoutStepTargetType, eWktStepTargetSpeed));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepCustomTargetValueHigh, new CFitSubfieldProfile("custom_target_heart_rate_high", &EnumType, eWorkoutStepCustomTargetValueHigh, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetHeartRate));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepCustomTargetValueHigh, new CFitSubfieldProfile("custom_target_cadence_high", &Uint32Type, eWorkoutStepCustomTargetValueHigh, 0, 0, "rpm", eWorkoutStepTargetType, eWktStepTargetCadence));
    profiles[eMesgNumWorkoutStep]->addSubfield(eWorkoutStepCustomTargetValueHigh, new CFitSubfieldProfile("custom_target_power_high", &EnumType, eWorkoutStepCustomTargetValueHigh, 0, 0, "%", eWorkoutStepTargetType, eWktStepTargetPower));
    profiles[eMesgNumWorkoutStep]->addField(new CFitFieldProfile("intensity", &EnumType, eWorkoutStepIntensity, 0, 0, ""));
}

void initSchedule(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumSchedule] = new CFitProfile("schedule", eMesgNumSchedule);
    profiles[eMesgNumSchedule]->addField(new CFitFieldProfile("manufacturer", &EnumType, eScheduleManufacturer, 0, 0, ""));
    profiles[eMesgNumSchedule]->addField(new CFitFieldProfile("product", &Uint16Type, eScheduleProduct, 0, 0, ""));
    profiles[eMesgNumSchedule]->addSubfield(eScheduleProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerGarmin));
    profiles[eMesgNumSchedule]->addSubfield(eScheduleProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerDynastream));
    profiles[eMesgNumSchedule]->addSubfield(eScheduleProduct, new CFitSubfieldProfile("garmin_product", &EnumType, eScheduleProduct, 0, 0, "", eScheduleManufacturer, eManufacturerDynastreamOem));
    profiles[eMesgNumSchedule]->addField(new CFitFieldProfile("serial_number", &Uint32zType, eScheduleSerialNumber, 0, 0, ""));
    profiles[eMesgNumSchedule]->addField(new CFitFieldProfile("time_created", &EnumType, eScheduleTimeCreated, 0, 0, ""));
    profiles[eMesgNumSchedule]->addField(new CFitFieldProfile("completed", &EnumType, eScheduleCompleted, 0, 0, ""));
    profiles[eMesgNumSchedule]->addField(new CFitFieldProfile("type", &EnumType, eScheduleType, 0, 0, ""));
    profiles[eMesgNumSchedule]->addField(new CFitFieldProfile("scheduled_time", &EnumType, eScheduleScheduledTime, 0, 0, ""));
}

void initTotals(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumTotals] = new CFitProfile("totals", eMesgNumTotals);
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("message_index", &EnumType, eTotalsMessageIndex, 0, 0, ""));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("timestamp", &EnumType, eTotalsTimestamp, 0, 0, "s"));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("timer_time", &Uint32Type, eTotalsTimerTime, 0, 0, "s"));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("distance", &Uint32Type, eTotalsDistance, 0, 0, "m"));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("calories", &Uint32Type, eTotalsCalories, 0, 0, "kcal"));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("sport", &EnumType, eTotalsSport, 0, 0, ""));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("elapsed_time", &Uint32Type, eTotalsElapsedTime, 0, 0, "s"));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("sessions", &Uint16Type, eTotalsSessions, 0, 0, ""));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("active_time", &Uint32Type, eTotalsActiveTime, 0, 0, "s"));
    profiles[eMesgNumTotals]->addField(new CFitFieldProfile("sport_index", &Uint8Type, eTotalsSportIndex, 0, 0, ""));
}

void initWeightScale(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumWeightScale] = new CFitProfile("weight_scale", eMesgNumWeightScale);
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("timestamp", &EnumType, eWeightScaleTimestamp, 0, 0, "s"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("weight", &EnumType, eWeightScaleWeight, 100, 0, "kg"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("percent_fat", &Uint16Type, eWeightScalePercentFat, 100, 0, "%"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("percent_hydration", &Uint16Type, eWeightScalePercentHydration, 100, 0, "%"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("visceral_fat_mass", &Uint16Type, eWeightScaleVisceralFatMass, 100, 0, "kg"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("bone_mass", &Uint16Type, eWeightScaleBoneMass, 100, 0, "kg"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("muscle_mass", &Uint16Type, eWeightScaleMuscleMass, 100, 0, "kg"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("basal_met", &Uint16Type, eWeightScaleBasalMet, 4, 0, "kcal/day"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("physique_rating", &Uint8Type, eWeightScalePhysiqueRating, 0, 0, ""));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("active_met", &Uint16Type, eWeightScaleActiveMet, 4, 0, "kcal/day"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("metabolic_age", &Uint8Type, eWeightScaleMetabolicAge, 0, 0, "years"));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("visceral_fat_rating", &Uint8Type, eWeightScaleVisceralFatRating, 0, 0, ""));
    profiles[eMesgNumWeightScale]->addField(new CFitFieldProfile("user_profile_index", &EnumType, eWeightScaleUserProfileIndex, 0, 0, ""));
}

void initBloodPressure(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumBloodPressure] = new CFitProfile("blood_pressure", eMesgNumBloodPressure);
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("timestamp", &EnumType, eBloodPressureTimestamp, 0, 0, "s"));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("systolic_pressure", &Uint16Type, eBloodPressureSystolicPressure, 0, 0, "mmHg"));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("diastolic_pressure", &Uint16Type, eBloodPressureDiastolicPressure, 0, 0, "mmHg"));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("mean_arterial_pressure", &Uint16Type, eBloodPressureMeanArterialPressure, 0, 0, "mmHg"));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("map_3_sample_mean", &Uint16Type, eBloodPressureMap3SampleMean, 0, 0, "mmHg"));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("map_morning_values", &Uint16Type, eBloodPressureMapMorningValues, 0, 0, "mmHg"));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("map_evening_values", &Uint16Type, eBloodPressureMapEveningValues, 0, 0, "mmHg"));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("heart_rate", &Uint8Type, eBloodPressureHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("heart_rate_type", &EnumType, eBloodPressureHeartRateType, 0, 0, ""));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("status", &EnumType, eBloodPressureStatus, 0, 0, ""));
    profiles[eMesgNumBloodPressure]->addField(new CFitFieldProfile("user_profile_index", &EnumType, eBloodPressureUserProfileIndex, 0, 0, ""));
}

void initMonitoringInfo(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumMonitoringInfo] = new CFitProfile("monitoring_info", eMesgNumMonitoringInfo);
    profiles[eMesgNumMonitoringInfo]->addField(new CFitFieldProfile("timestamp", &EnumType, eMonitoringInfoTimestamp, 0, 0, "s"));
    profiles[eMesgNumMonitoringInfo]->addField(new CFitFieldProfile("local_timestamp", &EnumType, eMonitoringInfoLocalTimestamp, 0, 0, "s"));
    profiles[eMesgNumMonitoringInfo]->addField(new CFitFieldProfile("activity_type", &EnumType, eMonitoringInfoActivityType, 0, 0, ""));
    profiles[eMesgNumMonitoringInfo]->addField(new CFitFieldProfile("cycles_to_distance", &Uint16Type, eMonitoringInfoCyclesToDistance, 5000, 0, "m/cycle"));
    profiles[eMesgNumMonitoringInfo]->addField(new CFitFieldProfile("cycles_to_calories", &Uint16Type, eMonitoringInfoCyclesToCalories, 5000, 0, "kcal/cycle"));
    profiles[eMesgNumMonitoringInfo]->addField(new CFitFieldProfile("resting_metabolic_rate", &Uint16Type, eMonitoringInfoRestingMetabolicRate, 0, 0, "kcal / day"));
}

void initMonitoring(QMap<uint16_t, CFitProfile*>& profiles)
{
    profiles[eMesgNumMonitoring] = new CFitProfile("monitoring", eMesgNumMonitoring);
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("timestamp", &EnumType, eMonitoringTimestamp, 0, 0, "s"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("device_index", &EnumType, eMonitoringDeviceIndex, 0, 0, ""));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("calories", &Uint16Type, eMonitoringCalories, 0, 0, "kcal"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("distance", &Uint32Type, eMonitoringDistance, 100, 0, "m"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("cycles", &Uint32Type, eMonitoringCycles, 2, 0, "cycles"));
    profiles[eMesgNumMonitoring]->addSubfield(eMonitoringCycles, new CFitSubfieldProfile("steps", &Uint32Type, eMonitoringCycles, 1, 0, "steps", eMonitoringActivityType, eActivityTypeWalking));
    profiles[eMesgNumMonitoring]->addSubfield(eMonitoringCycles, new CFitSubfieldProfile("steps", &Uint32Type, eMonitoringCycles, 0, 0, "", eMonitoringActivityType, eActivityTypeRunning));
    profiles[eMesgNumMonitoring]->addSubfield(eMonitoringCycles, new CFitSubfieldProfile("strokes", &Uint32Type, eMonitoringCycles, 2, 0, "strokes", eMonitoringActivityType, eActivityTypeCycling));
    profiles[eMesgNumMonitoring]->addSubfield(eMonitoringCycles, new CFitSubfieldProfile("strokes", &Uint32Type, eMonitoringCycles, 0, 0, "", eMonitoringActivityType, eActivityTypeSwimming));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("active_time", &Uint32Type, eMonitoringActiveTime, 1000, 0, "s"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("activity_type", &EnumType, eMonitoringActivityType, 0, 0, ""));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("activity_subtype", &EnumType, eMonitoringActivitySubtype, 0, 0, ""));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("activity_level", &EnumType, eMonitoringActivityLevel, 0, 0, ""));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("distance_16", &Uint16Type, eMonitoringDistance16, 0, 0, "100 * m"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("cycles_16", &Uint16Type, eMonitoringCycles16, 0, 0, "2 * cycles (steps)"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("active_time_16", &Uint16Type, eMonitoringActiveTime16, 0, 0, "s"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("local_timestamp", &EnumType, eMonitoringLocalTimestamp, 0, 0, ""));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("temperature", &Sint16Type, eMonitoringTemperature, 100, 0, "C"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("temperature_min", &Sint16Type, eMonitoringTemperatureMin, 100, 0, "C"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("temperature_max", &Sint16Type, eMonitoringTemperatureMax, 100, 0, "C"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("activity_time", &Uint16Type, eMonitoringActivityTime, 0, 0, "minutes"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("active_calories", &Uint16Type, eMonitoringActiveCalories, 0, 0, "kcal"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("current_activity_type_intensity", &ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, ""));
    profiles[eMesgNumMonitoring]->addComponent(eMonitoringCurrentActivityTypeIntensity, new CFitComponentfieldProfile("current_activity_type_intensity", &ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "", eMonitoringActivityType, 5));
    profiles[eMesgNumMonitoring]->addComponent(eMonitoringCurrentActivityTypeIntensity, new CFitComponentfieldProfile("current_activity_type_intensity", &ByteType, eMonitoringCurrentActivityTypeIntensity, 0, 0, "", eMonitoringIntensity, 3));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("timestamp_min_8", &Uint8Type, eMonitoringTimestampMin8, 0, 0, "min"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("timestamp_16", &Uint16Type, eMonitoringTimestamp16, 0, 0, "s"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("heart_rate", &Uint8Type, eMonitoringHeartRate, 0, 0, "bpm"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("intensity", &Uint8Type, eMonitoringIntensity, 10, 0, ""));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("duration_min", &Uint16Type, eMonitoringDurationMin, 0, 0, "min"));
    profiles[eMesgNumMonitoring]->addField(new CFitFieldProfile("duration", &Uint32Type, eMonitoringDuration, 0, 0, "s"));
}

void initMemoGlob(QMap<uint16_t, CFitProfile*>& profiles) {
    profiles[eMesgNumMemoGlob] = new CFitProfile("memo_glob", eMesgNumMemoGlob);
    profiles[eMesgNumMemoGlob]->addField(new CFitFieldProfile("part_index", &Uint32Type, eMemoGlobPartIndex, 0, 0, ""));
    profiles[eMesgNumMemoGlob]->addField(new CFitFieldProfile("memo", &ByteType, eMemoGlobMemo, 0, 0, ""));
    profiles[eMesgNumMemoGlob]->addField(new CFitFieldProfile("message_number", &Uint16Type, eMemoGlobMessageNumber, 0, 0, ""));
    profiles[eMesgNumMemoGlob]->addField(new CFitFieldProfile("message_index", &EnumType, eMemoGlobMessageIndex, 0, 0, ""));
}
// ----------- end generated code -----------


QMap<uint16_t, CFitProfile*> initProfiles() {

    QMap<uint16_t, CFitProfile*> allProfiles;
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
    allProfiles[GlobalMesgNrInvalid] = new CFitProfile();
    allProfiles[GlobalMesgNrInvalid]->addField(new CFitFieldProfile());
    return allProfiles;
}

QMap<uint16_t, CFitProfile*> allProfiles = initProfiles();

const CFitProfile* CFitProfileLockup::getProfile(uint16_t globalMesgNr)
{
    if (allProfiles.contains(globalMesgNr))
    {
        return allProfiles[globalMesgNr];
    }
    return allProfiles[GlobalMesgNrInvalid];
}

const CFitFieldProfile* CFitProfileLockup::getFieldForProfile(uint16_t globalMesgNr, uint8_t fieldDefNr)
{
    if (allProfiles.contains(globalMesgNr)) {
        return  allProfiles[globalMesgNr]->getField(fieldDefNr);
    }
    return allProfiles[GlobalMesgNrInvalid]->getField(FieldDefNrInvalid);
}