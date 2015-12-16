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

#include "gis/fit/defs/CFitProfileLockup.h"
#include "gis/fit/defs/CFitBaseType.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/fit_const.h"

QMap<uint16_t, CFitProfile*>  initProfiles()
{
    QMap<uint16_t, CFitProfile*> profiles;
// ----------- start generated code -----------

    profiles[MesgNumFileId] = new CFitProfile("file_id", MesgNumFileId);
    profiles[MesgNumFileId]->addField(new CFitFieldProfile("type", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumFileId]->addField(new CFitFieldProfile("manufacturer", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumFileId]->addField(new CFitFieldProfile("product", &Uint16Type, 2, 0, 0, ""));
    profiles[MesgNumFileId]->addField(new CFitFieldProfile("serial_number", &Uint32zType, 3, 0, 0, ""));
    profiles[MesgNumFileId]->addField(new CFitFieldProfile("time_created", &EnumType, 4, 0, 0, ""));
    profiles[MesgNumFileId]->addField(new CFitFieldProfile("number", &Uint16Type, 5, 0, 0, ""));
    profiles[MesgNumFileId]->addField(new CFitFieldProfile("product_name", &StringType, 8, 0, 0, ""));

    profiles[MesgNumFileCreator] = new CFitProfile("file_creator", MesgNumFileCreator);
    profiles[MesgNumFileCreator]->addField(new CFitFieldProfile("software_version", &Uint16Type, 0, 0, 0, ""));
    profiles[MesgNumFileCreator]->addField(new CFitFieldProfile("hardware_version", &Uint8Type, 1, 0, 0, ""));

    profiles[MesgNumTimestampCorrelation] = new CFitProfile("timestamp_correlation", MesgNumTimestampCorrelation);
    profiles[MesgNumTimestampCorrelation]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumTimestampCorrelation]->addField(new CFitFieldProfile("fractional_timestamp", &Uint16Type, 0, 32768, 0, "s"));
    profiles[MesgNumTimestampCorrelation]->addField(new CFitFieldProfile("system_timestamp", &EnumType, 1, 0, 0, "s"));
    profiles[MesgNumTimestampCorrelation]->addField(new CFitFieldProfile("fractional_system_timestamp", &Uint16Type, 2, 32768, 0, "s"));
    profiles[MesgNumTimestampCorrelation]->addField(new CFitFieldProfile("local_timestamp", &EnumType, 3, 0, 0, "s"));
    profiles[MesgNumTimestampCorrelation]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, 4, 0, 0, "ms"));
    profiles[MesgNumTimestampCorrelation]->addField(new CFitFieldProfile("system_timestamp_ms", &Uint16Type, 5, 0, 0, "ms"));

    profiles[MesgNumSoftware] = new CFitProfile("software", MesgNumSoftware);
    profiles[MesgNumSoftware]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumSoftware]->addField(new CFitFieldProfile("version", &Uint16Type, 3, 100, 0, ""));
    profiles[MesgNumSoftware]->addField(new CFitFieldProfile("part_number", &StringType, 5, 0, 0, ""));

    profiles[MesgNumSlaveDevice] = new CFitProfile("slave_device", MesgNumSlaveDevice);
    profiles[MesgNumSlaveDevice]->addField(new CFitFieldProfile("manufacturer", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumSlaveDevice]->addField(new CFitFieldProfile("product", &Uint16Type, 1, 0, 0, ""));

    profiles[MesgNumCapabilities] = new CFitProfile("capabilities", MesgNumCapabilities);
    profiles[MesgNumCapabilities]->addField(new CFitFieldProfile("languages", &Uint8zType, 0, 0, 0, ""));
    profiles[MesgNumCapabilities]->addField(new CFitFieldProfile("sports", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumCapabilities]->addField(new CFitFieldProfile("workouts_supported", &EnumType, 21, 0, 0, ""));
    profiles[MesgNumCapabilities]->addField(new CFitFieldProfile("connectivity_supported", &EnumType, 23, 0, 0, ""));

    profiles[MesgNumFileCapabilities] = new CFitProfile("file_capabilities", MesgNumFileCapabilities);
    profiles[MesgNumFileCapabilities]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumFileCapabilities]->addField(new CFitFieldProfile("type", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumFileCapabilities]->addField(new CFitFieldProfile("flags", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumFileCapabilities]->addField(new CFitFieldProfile("directory", &StringType, 2, 0, 0, ""));
    profiles[MesgNumFileCapabilities]->addField(new CFitFieldProfile("max_count", &Uint16Type, 3, 0, 0, ""));
    profiles[MesgNumFileCapabilities]->addField(new CFitFieldProfile("max_size", &Uint32Type, 4, 0, 0, "bytes"));

    profiles[MesgNumMesgCapabilities] = new CFitProfile("mesg_capabilities", MesgNumMesgCapabilities);
    profiles[MesgNumMesgCapabilities]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumMesgCapabilities]->addField(new CFitFieldProfile("file", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumMesgCapabilities]->addField(new CFitFieldProfile("mesg_num", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumMesgCapabilities]->addField(new CFitFieldProfile("count_type", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumMesgCapabilities]->addField(new CFitFieldProfile("count", &Uint16Type, 3, 0, 0, ""));
    profiles[MesgNumMesgCapabilities]->getField(3)->addSubfield(new CFitFieldProfile("num_per_file", &Uint16Type, 3, 0, 0, ""));
    profiles[MesgNumMesgCapabilities]->getField(3)->addSubfield(new CFitFieldProfile("max_per_file", &Uint16Type, 3, 0, 0, ""));
    profiles[MesgNumMesgCapabilities]->getField(3)->addSubfield(new CFitFieldProfile("max_per_file_type", &Uint16Type, 3, 0, 0, ""));

    profiles[MesgNumFieldCapabilities] = new CFitProfile("field_capabilities", MesgNumFieldCapabilities);
    profiles[MesgNumFieldCapabilities]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumFieldCapabilities]->addField(new CFitFieldProfile("file", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumFieldCapabilities]->addField(new CFitFieldProfile("mesg_num", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumFieldCapabilities]->addField(new CFitFieldProfile("field_num", &Uint8Type, 2, 0, 0, ""));
    profiles[MesgNumFieldCapabilities]->addField(new CFitFieldProfile("count", &Uint16Type, 3, 0, 0, ""));

    profiles[MesgNumDeviceSettings] = new CFitProfile("device_settings", MesgNumDeviceSettings);
    profiles[MesgNumDeviceSettings]->addField(new CFitFieldProfile("active_time_zone", &Uint8Type, 0, 0, 0, ""));
    profiles[MesgNumDeviceSettings]->addField(new CFitFieldProfile("utc_offset", &Uint32Type, 1, 0, 0, ""));
    profiles[MesgNumDeviceSettings]->addField(new CFitFieldProfile("time_zone_offset", &Sint8Type, 5, 4, 0, "hr"));

    profiles[MesgNumUserProfile] = new CFitProfile("user_profile", MesgNumUserProfile);
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("friendly_name", &StringType, 0, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("gender", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("age", &Uint8Type, 2, 0, 0, "years"));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("height", &Uint8Type, 3, 100, 0, "m"));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("weight", &Uint16Type, 4, 10, 0, "kg"));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("language", &EnumType, 5, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("elev_setting", &EnumType, 6, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("weight_setting", &EnumType, 7, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("resting_heart_rate", &Uint8Type, 8, 0, 0, "bpm"));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("default_max_running_heart_rate", &Uint8Type, 9, 0, 0, "bpm"));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("default_max_biking_heart_rate", &Uint8Type, 10, 0, 0, "bpm"));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("default_max_heart_rate", &Uint8Type, 11, 0, 0, "bpm"));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("hr_setting", &EnumType, 12, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("speed_setting", &EnumType, 13, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("dist_setting", &EnumType, 14, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("power_setting", &EnumType, 16, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("activity_class", &EnumType, 17, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("position_setting", &EnumType, 18, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("temperature_setting", &EnumType, 21, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("local_id", &EnumType, 22, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("global_id", &ByteType, 23, 0, 0, ""));
    profiles[MesgNumUserProfile]->addField(new CFitFieldProfile("height_setting", &EnumType, 30, 0, 0, ""));

    profiles[MesgNumHrmProfile] = new CFitProfile("hrm_profile", MesgNumHrmProfile);
    profiles[MesgNumHrmProfile]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumHrmProfile]->addField(new CFitFieldProfile("enabled", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumHrmProfile]->addField(new CFitFieldProfile("hrm_ant_id", &Uint16zType, 1, 0, 0, ""));
    profiles[MesgNumHrmProfile]->addField(new CFitFieldProfile("log_hrv", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumHrmProfile]->addField(new CFitFieldProfile("hrm_ant_id_trans_type", &Uint8zType, 3, 0, 0, ""));

    profiles[MesgNumSdmProfile] = new CFitProfile("sdm_profile", MesgNumSdmProfile);
    profiles[MesgNumSdmProfile]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumSdmProfile]->addField(new CFitFieldProfile("enabled", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumSdmProfile]->addField(new CFitFieldProfile("sdm_ant_id", &Uint16zType, 1, 0, 0, ""));
    profiles[MesgNumSdmProfile]->addField(new CFitFieldProfile("sdm_cal_factor", &Uint16Type, 2, 10, 0, "%"));
    profiles[MesgNumSdmProfile]->addField(new CFitFieldProfile("odometer", &Uint32Type, 3, 100, 0, "m"));
    profiles[MesgNumSdmProfile]->addField(new CFitFieldProfile("speed_source", &EnumType, 4, 0, 0, ""));
    profiles[MesgNumSdmProfile]->addField(new CFitFieldProfile("sdm_ant_id_trans_type", &Uint8zType, 5, 0, 0, ""));
    profiles[MesgNumSdmProfile]->addField(new CFitFieldProfile("odometer_rollover", &Uint8Type, 7, 0, 0, ""));

    profiles[MesgNumBikeProfile] = new CFitProfile("bike_profile", MesgNumBikeProfile);
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("name", &StringType, 0, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("sport", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("sub_sport", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("odometer", &Uint32Type, 3, 100, 0, "m"));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_spd_ant_id", &Uint16zType, 4, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_cad_ant_id", &Uint16zType, 5, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_spdcad_ant_id", &Uint16zType, 6, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_power_ant_id", &Uint16zType, 7, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("custom_wheelsize", &Uint16Type, 8, 1000, 0, "m"));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("auto_wheelsize", &Uint16Type, 9, 1000, 0, "m"));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_weight", &Uint16Type, 10, 10, 0, "kg"));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("power_cal_factor", &Uint16Type, 11, 10, 0, "%"));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("auto_wheel_cal", &EnumType, 12, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("auto_power_zero", &EnumType, 13, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("id", &Uint8Type, 14, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("spd_enabled", &EnumType, 15, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("cad_enabled", &EnumType, 16, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("spdcad_enabled", &EnumType, 17, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("power_enabled", &EnumType, 18, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("crank_length", &Uint8Type, 19, 2, -110, "mm"));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("enabled", &EnumType, 20, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_spd_ant_id_trans_type", &Uint8zType, 21, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_cad_ant_id_trans_type", &Uint8zType, 22, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_spdcad_ant_id_trans_type", &Uint8zType, 23, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("bike_power_ant_id_trans_type", &Uint8zType, 24, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("odometer_rollover", &Uint8Type, 37, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("front_gear_num", &Uint8zType, 38, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("front_gear", &Uint8zType, 39, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("rear_gear_num", &Uint8zType, 40, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("rear_gear", &Uint8zType, 41, 0, 0, ""));
    profiles[MesgNumBikeProfile]->addField(new CFitFieldProfile("shimano_di2_enabled", &EnumType, 44, 0, 0, ""));

    profiles[MesgNumZonesTarget] = new CFitProfile("zones_target", MesgNumZonesTarget);
    profiles[MesgNumZonesTarget]->addField(new CFitFieldProfile("max_heart_rate", &Uint8Type, 1, 0, 0, ""));
    profiles[MesgNumZonesTarget]->addField(new CFitFieldProfile("threshold_heart_rate", &Uint8Type, 2, 0, 0, ""));
    profiles[MesgNumZonesTarget]->addField(new CFitFieldProfile("functional_threshold_power", &Uint16Type, 3, 0, 0, ""));
    profiles[MesgNumZonesTarget]->addField(new CFitFieldProfile("hr_calc_type", &EnumType, 5, 0, 0, ""));
    profiles[MesgNumZonesTarget]->addField(new CFitFieldProfile("pwr_calc_type", &EnumType, 7, 0, 0, ""));

    profiles[MesgNumSport] = new CFitProfile("sport", MesgNumSport);
    profiles[MesgNumSport]->addField(new CFitFieldProfile("sport", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumSport]->addField(new CFitFieldProfile("sub_sport", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumSport]->addField(new CFitFieldProfile("name", &StringType, 3, 0, 0, ""));

    profiles[MesgNumHrZone] = new CFitProfile("hr_zone", MesgNumHrZone);
    profiles[MesgNumHrZone]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumHrZone]->addField(new CFitFieldProfile("high_bpm", &Uint8Type, 1, 0, 0, "bpm"));
    profiles[MesgNumHrZone]->addField(new CFitFieldProfile("name", &StringType, 2, 0, 0, ""));

    profiles[MesgNumSpeedZone] = new CFitProfile("speed_zone", MesgNumSpeedZone);
    profiles[MesgNumSpeedZone]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumSpeedZone]->addField(new CFitFieldProfile("high_value", &Uint16Type, 0, 1000, 0, "m/s"));
    profiles[MesgNumSpeedZone]->addField(new CFitFieldProfile("name", &StringType, 1, 0, 0, ""));

    profiles[MesgNumCadenceZone] = new CFitProfile("cadence_zone", MesgNumCadenceZone);
    profiles[MesgNumCadenceZone]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumCadenceZone]->addField(new CFitFieldProfile("high_value", &Uint8Type, 0, 0, 0, "rpm"));
    profiles[MesgNumCadenceZone]->addField(new CFitFieldProfile("name", &StringType, 1, 0, 0, ""));

    profiles[MesgNumPowerZone] = new CFitProfile("power_zone", MesgNumPowerZone);
    profiles[MesgNumPowerZone]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumPowerZone]->addField(new CFitFieldProfile("high_value", &Uint16Type, 1, 0, 0, "watts"));
    profiles[MesgNumPowerZone]->addField(new CFitFieldProfile("name", &StringType, 2, 0, 0, ""));

    profiles[MesgNumMetZone] = new CFitProfile("met_zone", MesgNumMetZone);
    profiles[MesgNumMetZone]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumMetZone]->addField(new CFitFieldProfile("high_bpm", &Uint8Type, 1, 0, 0, ""));
    profiles[MesgNumMetZone]->addField(new CFitFieldProfile("calories", &Uint16Type, 2, 10, 0, "kcal / min"));
    profiles[MesgNumMetZone]->addField(new CFitFieldProfile("fat_calories", &Uint8Type, 3, 10, 0, "kcal / min"));

    profiles[MesgNumGoal] = new CFitProfile("goal", MesgNumGoal);
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("sport", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("sub_sport", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("start_date", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("end_date", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("type", &EnumType, 4, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("value", &Uint32Type, 5, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("repeat", &EnumType, 6, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("target_value", &Uint32Type, 7, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("recurrence", &EnumType, 8, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("recurrence_value", &Uint16Type, 9, 0, 0, ""));
    profiles[MesgNumGoal]->addField(new CFitFieldProfile("enabled", &EnumType, 10, 0, 0, ""));

    profiles[MesgNumActivity] = new CFitProfile("activity", MesgNumActivity);
    profiles[MesgNumActivity]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, ""));
    profiles[MesgNumActivity]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, 0, 1000, 0, "s"));
    profiles[MesgNumActivity]->addField(new CFitFieldProfile("num_sessions", &Uint16Type, 1, 0, 0, ""));
    profiles[MesgNumActivity]->addField(new CFitFieldProfile("type", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumActivity]->addField(new CFitFieldProfile("event", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumActivity]->addField(new CFitFieldProfile("event_type", &EnumType, 4, 0, 0, ""));
    profiles[MesgNumActivity]->addField(new CFitFieldProfile("local_timestamp", &EnumType, 5, 0, 0, ""));
    profiles[MesgNumActivity]->addField(new CFitFieldProfile("event_group", &Uint8Type, 6, 0, 0, ""));

    profiles[MesgNumSession] = new CFitProfile("session", MesgNumSession);
    profiles[MesgNumSession]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("event", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("event_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("start_time", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("start_position_lat", &Sint32Type, 3, 0, 0, "semicircles"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("start_position_long", &Sint32Type, 4, 0, 0, "semicircles"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("sport", &EnumType, 5, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("sub_sport", &EnumType, 6, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_elapsed_time", &Uint32Type, 7, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, 8, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_distance", &Uint32Type, 9, 100, 0, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_cycles", &Uint32Type, 10, 0, 0, "cycles"));
    profiles[MesgNumSession]->getField(10)->addSubfield(new CFitFieldProfile("total_strides", &Uint32Type, 10, 0, 0, "strides"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_calories", &Uint16Type, 11, 0, 0, "kcal"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_fat_calories", &Uint16Type, 13, 0, 0, "kcal"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_speed", &Uint16Type, 14, 1000, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_speed", &Uint16Type, 15, 1000, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_heart_rate", &Uint8Type, 16, 0, 0, "bpm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_heart_rate", &Uint8Type, 17, 0, 0, "bpm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_cadence", &Uint8Type, 18, 0, 0, "rpm"));
    profiles[MesgNumSession]->getField(18)->addSubfield(new CFitFieldProfile("avg_running_cadence", &Uint8Type, 18, 0, 0, "strides/min"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_cadence", &Uint8Type, 19, 0, 0, "rpm"));
    profiles[MesgNumSession]->getField(19)->addSubfield(new CFitFieldProfile("max_running_cadence", &Uint8Type, 19, 0, 0, "strides/min"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_power", &Uint16Type, 20, 0, 0, "watts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_power", &Uint16Type, 21, 0, 0, "watts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_ascent", &Uint16Type, 22, 0, 0, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_descent", &Uint16Type, 23, 0, 0, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_training_effect", &Uint8Type, 24, 10, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("first_lap_index", &Uint16Type, 25, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("num_laps", &Uint16Type, 26, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("event_group", &Uint8Type, 27, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("trigger", &EnumType, 28, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("nec_lat", &Sint32Type, 29, 0, 0, "semicircles"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("nec_long", &Sint32Type, 30, 0, 0, "semicircles"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("swc_lat", &Sint32Type, 31, 0, 0, "semicircles"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("swc_long", &Sint32Type, 32, 0, 0, "semicircles"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("normalized_power", &Uint16Type, 34, 0, 0, "watts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("training_stress_score", &Uint16Type, 35, 10, 0, "tss"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("intensity_factor", &Uint16Type, 36, 1000, 0, "if"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("left_right_balance", &EnumType, 37, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_stroke_count", &Uint32Type, 41, 10, 0, "strokes/lap"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_stroke_distance", &Uint16Type, 42, 100, 0, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("swim_stroke", &EnumType, 43, 0, 0, "swim_stroke"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("pool_length", &Uint16Type, 44, 100, 0, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("threshold_power", &Uint16Type, 45, 0, 0, "watts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("pool_length_unit", &EnumType, 46, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("num_active_lengths", &Uint16Type, 47, 0, 0, "lengths"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_work", &Uint32Type, 48, 0, 0, "J"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_altitude", &Uint16Type, 49, 5, 500, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_altitude", &Uint16Type, 50, 5, 500, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("gps_accuracy", &Uint8Type, 51, 0, 0, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_grade", &Sint16Type, 52, 100, 0, "%"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_pos_grade", &Sint16Type, 53, 100, 0, "%"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_neg_grade", &Sint16Type, 54, 100, 0, "%"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_pos_grade", &Sint16Type, 55, 100, 0, "%"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_neg_grade", &Sint16Type, 56, 100, 0, "%"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_temperature", &Sint8Type, 57, 0, 0, "C"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_temperature", &Sint8Type, 58, 0, 0, "C"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_moving_time", &Uint32Type, 59, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_pos_vertical_speed", &Sint16Type, 60, 1000, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_neg_vertical_speed", &Sint16Type, 61, 1000, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_pos_vertical_speed", &Sint16Type, 62, 1000, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_neg_vertical_speed", &Sint16Type, 63, 1000, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("min_heart_rate", &Uint8Type, 64, 0, 0, "bpm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("time_in_hr_zone", &Uint32Type, 65, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("time_in_speed_zone", &Uint32Type, 66, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("time_in_cadence_zone", &Uint32Type, 67, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("time_in_power_zone", &Uint32Type, 68, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_lap_time", &Uint32Type, 69, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("best_lap_index", &Uint16Type, 70, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("min_altitude", &Uint16Type, 71, 5, 500, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("player_score", &Uint16Type, 82, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("opponent_score", &Uint16Type, 83, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("opponent_name", &StringType, 84, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("stroke_count", &Uint16Type, 85, 0, 0, "counts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("zone_count", &Uint16Type, 86, 0, 0, "counts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_ball_speed", &Uint16Type, 87, 100, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_ball_speed", &Uint16Type, 88, 100, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_vertical_oscillation", &Uint16Type, 89, 10, 0, "mm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_stance_time_percent", &Uint16Type, 90, 100, 0, "percent"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_stance_time", &Uint16Type, 91, 10, 0, "ms"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_fractional_cadence", &Uint8Type, 92, 128, 0, "rpm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_fractional_cadence", &Uint8Type, 93, 128, 0, "rpm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("total_fractional_cycles", &Uint8Type, 94, 128, 0, "cycles"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_total_hemoglobin_conc", &Uint16Type, 95, 100, 0, "g/dL"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("min_total_hemoglobin_conc", &Uint16Type, 96, 100, 0, "g/dL"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_total_hemoglobin_conc", &Uint16Type, 97, 100, 0, "g/dL"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_saturated_hemoglobin_percent", &Uint16Type, 98, 10, 0, "%"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("min_saturated_hemoglobin_percent", &Uint16Type, 99, 10, 0, "%"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_saturated_hemoglobin_percent", &Uint16Type, 100, 10, 0, "%"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_left_torque_effectiveness", &Uint8Type, 101, 2, 0, "percent"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_right_torque_effectiveness", &Uint8Type, 102, 2, 0, "percent"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_left_pedal_smoothness", &Uint8Type, 103, 2, 0, "percent"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_right_pedal_smoothness", &Uint8Type, 104, 2, 0, "percent"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_combined_pedal_smoothness", &Uint8Type, 105, 2, 0, "percent"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("sport_index", &Uint8Type, 111, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("time_standing", &Uint32Type, 112, 1000, 0, "s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("stand_count", &Uint16Type, 113, 0, 0, ""));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_left_pco", &Sint8Type, 114, 0, 0, "mm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_right_pco", &Sint8Type, 115, 0, 0, "mm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_left_power_phase", &Uint8Type, 116, 0.7111111, 0, "degrees"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_left_power_phase_peak", &Uint8Type, 117, 0.7111111, 0, "degrees"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_right_power_phase", &Uint8Type, 118, 0.7111111, 0, "degrees"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_right_power_phase_peak", &Uint8Type, 119, 0.7111111, 0, "degrees"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_power_position", &Uint16Type, 120, 0, 0, "watts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_power_position", &Uint16Type, 121, 0, 0, "watts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_cadence_position", &Uint8Type, 122, 0, 0, "rpm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_cadence_position", &Uint8Type, 123, 0, 0, "rpm"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("enhanced_avg_speed", &Uint32Type, 124, 1000, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("enhanced_max_speed", &Uint32Type, 125, 1000, 0, "m/s"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("enhanced_avg_altitude", &Uint32Type, 126, 5, 500, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("enhanced_min_altitude", &Uint32Type, 127, 5, 500, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("enhanced_max_altitude", &Uint32Type, 128, 5, 500, "m"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("avg_lev_motor_power", &Uint16Type, 129, 0, 0, "watts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("max_lev_motor_power", &Uint16Type, 130, 0, 0, "watts"));
    profiles[MesgNumSession]->addField(new CFitFieldProfile("lev_battery_consumption", &Uint8Type, 131, 2, 0, "percent"));

    profiles[MesgNumLap] = new CFitProfile("lap", MesgNumLap);
    profiles[MesgNumLap]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("event", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("event_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("start_time", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("start_position_lat", &Sint32Type, 3, 0, 0, "semicircles"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("start_position_long", &Sint32Type, 4, 0, 0, "semicircles"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("end_position_lat", &Sint32Type, 5, 0, 0, "semicircles"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("end_position_long", &Sint32Type, 6, 0, 0, "semicircles"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_elapsed_time", &Uint32Type, 7, 1000, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, 8, 1000, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_distance", &Uint32Type, 9, 100, 0, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_cycles", &Uint32Type, 10, 0, 0, "cycles"));
    profiles[MesgNumLap]->getField(10)->addSubfield(new CFitFieldProfile("strides", &EnumType, 10, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_calories", &Uint16Type, 11, 0, 0, "kcal"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_fat_calories", &Uint16Type, 12, 0, 0, "kcal"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_speed", &Uint16Type, 13, 1000, 0, "m/s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_speed", &Uint16Type, 14, 1000, 0, "m/s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_heart_rate", &Uint8Type, 15, 0, 0, "bpm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_heart_rate", &Uint8Type, 16, 0, 0, "bpm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_cadence", &Uint8Type, 17, 0, 0, "rpm"));
    profiles[MesgNumLap]->getField(17)->addSubfield(new CFitFieldProfile("avg_running_cadence", &Uint8Type, 17, 0, 0, "strides/min"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_cadence", &Uint8Type, 18, 0, 0, "rpm"));
    profiles[MesgNumLap]->getField(18)->addSubfield(new CFitFieldProfile("max_running_cadence", &Uint8Type, 18, 0, 0, "strides/min"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_power", &Uint16Type, 19, 0, 0, "watts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_power", &Uint16Type, 20, 0, 0, "watts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_ascent", &Uint16Type, 21, 0, 0, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_descent", &Uint16Type, 22, 0, 0, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("intensity", &EnumType, 23, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("lap_trigger", &EnumType, 24, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("sport", &EnumType, 25, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("event_group", &Uint8Type, 26, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("num_lengths", &Uint16Type, 32, 0, 0, "lengths"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("normalized_power", &Uint16Type, 33, 0, 0, "watts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("left_right_balance", &EnumType, 34, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("first_length_index", &Uint16Type, 35, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_stroke_distance", &Uint16Type, 37, 100, 0, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("swim_stroke", &EnumType, 38, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("sub_sport", &EnumType, 39, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("num_active_lengths", &Uint16Type, 40, 0, 0, "lengths"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_work", &Uint32Type, 41, 0, 0, "J"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_altitude", &Uint16Type, 42, 5, 500, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_altitude", &Uint16Type, 43, 5, 500, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("gps_accuracy", &Uint8Type, 44, 0, 0, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_grade", &Sint16Type, 45, 100, 0, "%"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_pos_grade", &Sint16Type, 46, 100, 0, "%"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_neg_grade", &Sint16Type, 47, 100, 0, "%"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_pos_grade", &Sint16Type, 48, 100, 0, "%"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_neg_grade", &Sint16Type, 49, 100, 0, "%"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_temperature", &Sint8Type, 50, 0, 0, "C"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_temperature", &Sint8Type, 51, 0, 0, "C"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_moving_time", &Uint32Type, 52, 1000, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_pos_vertical_speed", &Sint16Type, 53, 1000, 0, "m/s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_neg_vertical_speed", &Sint16Type, 54, 1000, 0, "m/s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_pos_vertical_speed", &Sint16Type, 55, 1000, 0, "m/s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_neg_vertical_speed", &Sint16Type, 56, 1000, 0, "m/s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("time_in_hr_zone", &Uint32Type, 57, 1000, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("time_in_speed_zone", &Uint32Type, 58, 1000, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("time_in_cadence_zone", &Uint32Type, 59, 1000, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("time_in_power_zone", &Uint32Type, 60, 1000, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("repetition_num", &Uint16Type, 61, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("min_altitude", &Uint16Type, 62, 5, 500, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("min_heart_rate", &Uint8Type, 63, 0, 0, "bpm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("wkt_step_index", &EnumType, 71, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("opponent_score", &Uint16Type, 74, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("stroke_count", &Uint16Type, 75, 0, 0, "counts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("zone_count", &Uint16Type, 76, 0, 0, "counts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_vertical_oscillation", &Uint16Type, 77, 10, 0, "mm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_stance_time_percent", &Uint16Type, 78, 100, 0, "percent"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_stance_time", &Uint16Type, 79, 10, 0, "ms"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_fractional_cadence", &Uint8Type, 80, 128, 0, "rpm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_fractional_cadence", &Uint8Type, 81, 128, 0, "rpm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("total_fractional_cycles", &Uint8Type, 82, 128, 0, "cycles"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("player_score", &Uint16Type, 83, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_total_hemoglobin_conc", &Uint16Type, 84, 100, 0, "g/dL"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("min_total_hemoglobin_conc", &Uint16Type, 85, 100, 0, "g/dL"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_total_hemoglobin_conc", &Uint16Type, 86, 100, 0, "g/dL"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_saturated_hemoglobin_percent", &Uint16Type, 87, 10, 0, "%"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("min_saturated_hemoglobin_percent", &Uint16Type, 88, 10, 0, "%"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_saturated_hemoglobin_percent", &Uint16Type, 89, 10, 0, "%"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_left_torque_effectiveness", &Uint8Type, 91, 2, 0, "percent"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_right_torque_effectiveness", &Uint8Type, 92, 2, 0, "percent"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_left_pedal_smoothness", &Uint8Type, 93, 2, 0, "percent"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_right_pedal_smoothness", &Uint8Type, 94, 2, 0, "percent"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_combined_pedal_smoothness", &Uint8Type, 95, 2, 0, "percent"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("time_standing", &Uint32Type, 98, 1000, 0, "s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("stand_count", &Uint16Type, 99, 0, 0, ""));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_left_pco", &Sint8Type, 100, 0, 0, "mm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_right_pco", &Sint8Type, 101, 0, 0, "mm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_left_power_phase", &Uint8Type, 102, 0.7111111, 0, "degrees"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_left_power_phase_peak", &Uint8Type, 103, 0.7111111, 0, "degrees"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_right_power_phase", &Uint8Type, 104, 0.7111111, 0, "degrees"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_right_power_phase_peak", &Uint8Type, 105, 0.7111111, 0, "degrees"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_power_position", &Uint16Type, 106, 0, 0, "watts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_power_position", &Uint16Type, 107, 0, 0, "watts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_cadence_position", &Uint8Type, 108, 0, 0, "rpm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_cadence_position", &Uint8Type, 109, 0, 0, "rpm"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("enhanced_avg_speed", &Uint32Type, 110, 1000, 0, "m/s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("enhanced_max_speed", &Uint32Type, 111, 1000, 0, "m/s"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("enhanced_avg_altitude", &Uint32Type, 112, 5, 500, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("enhanced_min_altitude", &Uint32Type, 113, 5, 500, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("enhanced_max_altitude", &Uint32Type, 114, 5, 500, "m"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("avg_lev_motor_power", &Uint16Type, 115, 0, 0, "watts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("max_lev_motor_power", &Uint16Type, 116, 0, 0, "watts"));
    profiles[MesgNumLap]->addField(new CFitFieldProfile("lev_battery_consumption", &Uint8Type, 117, 2, 0, "percent"));

    profiles[MesgNumLength] = new CFitProfile("length", MesgNumLength);
    profiles[MesgNumLength]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("event", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("event_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("start_time", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("total_elapsed_time", &Uint32Type, 3, 1000, 0, "s"));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, 4, 1000, 0, "s"));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("total_strokes", &Uint16Type, 5, 0, 0, "strokes"));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("avg_speed", &Uint16Type, 6, 1000, 0, "m/s"));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("swim_stroke", &EnumType, 7, 0, 0, "swim_stroke"));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("avg_swimming_cadence", &Uint8Type, 9, 0, 0, "strokes/min"));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("event_group", &Uint8Type, 10, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("total_calories", &Uint16Type, 11, 0, 0, "kcal"));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("length_type", &EnumType, 12, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("player_score", &Uint16Type, 18, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("opponent_score", &Uint16Type, 19, 0, 0, ""));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("stroke_count", &Uint16Type, 20, 0, 0, "counts"));
    profiles[MesgNumLength]->addField(new CFitFieldProfile("zone_count", &Uint16Type, 21, 0, 0, "counts"));

    profiles[MesgNumRecord] = new CFitProfile("record", MesgNumRecord);
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("position_lat", &Sint32Type, 0, 0, 0, "semicircles"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("position_long", &Sint32Type, 1, 0, 0, "semicircles"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("altitude", &Uint16Type, 2, 5, 500, "m"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("heart_rate", &Uint8Type, 3, 0, 0, "bpm"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("cadence", &Uint8Type, 4, 0, 0, "rpm"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("distance", &Uint32Type, 5, 100, 0, "m"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("speed", &Uint16Type, 6, 1000, 0, "m/s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("power", &Uint16Type, 7, 0, 0, "watts"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("compressed_speed_distance", &ByteType, 8, 100, 0, "m/s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("grade", &Sint16Type, 9, 100, 0, "%"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("resistance", &Uint8Type, 10, 0, 0, ""));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("time_from_course", &Sint32Type, 11, 1000, 0, "s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("cycle_length", &Uint8Type, 12, 100, 0, "m"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("temperature", &Sint8Type, 13, 0, 0, "C"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("speed_1s", &Uint8Type, 17, 16, 0, "m/s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("cycles", &Uint8Type, 18, 0, 0, "cycles"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("total_cycles", &Uint32Type, 19, 0, 0, "cycles"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("compressed_accumulated_power", &Uint16Type, 28, 0, 0, "watts"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("accumulated_power", &Uint32Type, 29, 0, 0, "watts"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("left_right_balance", &EnumType, 30, 0, 0, ""));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("gps_accuracy", &Uint8Type, 31, 0, 0, "m"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("vertical_speed", &Sint16Type, 32, 1000, 0, "m/s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("calories", &Uint16Type, 33, 0, 0, "kcal"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("vertical_oscillation", &Uint16Type, 39, 10, 0, "mm"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("stance_time_percent", &Uint16Type, 40, 100, 0, "percent"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("stance_time", &Uint16Type, 41, 10, 0, "ms"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("activity_type", &EnumType, 42, 0, 0, ""));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("left_torque_effectiveness", &Uint8Type, 43, 2, 0, "percent"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("right_torque_effectiveness", &Uint8Type, 44, 2, 0, "percent"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("left_pedal_smoothness", &Uint8Type, 45, 2, 0, "percent"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("right_pedal_smoothness", &Uint8Type, 46, 2, 0, "percent"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("combined_pedal_smoothness", &Uint8Type, 47, 2, 0, "percent"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("time128", &Uint8Type, 48, 128, 0, "s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("stroke_type", &EnumType, 49, 0, 0, ""));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("zone", &Uint8Type, 50, 0, 0, ""));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("ball_speed", &Uint16Type, 51, 100, 0, "m/s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("cadence256", &Uint16Type, 52, 256, 0, "rpm"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("fractional_cadence", &Uint8Type, 53, 128, 0, "rpm"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("total_hemoglobin_conc", &Uint16Type, 54, 100, 0, "g/dL"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("total_hemoglobin_conc_min", &Uint16Type, 55, 100, 0, "g/dL"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("total_hemoglobin_conc_max", &Uint16Type, 56, 100, 0, "g/dL"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("saturated_hemoglobin_percent", &Uint16Type, 57, 10, 0, "%"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("saturated_hemoglobin_percent_min", &Uint16Type, 58, 10, 0, "%"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("saturated_hemoglobin_percent_max", &Uint16Type, 59, 10, 0, "%"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("device_index", &EnumType, 62, 0, 0, ""));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("left_pco", &Sint8Type, 67, 0, 0, "mm"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("right_pco", &Sint8Type, 68, 0, 0, "mm"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("left_power_phase", &Uint8Type, 69, 0.7111111, 0, "degrees"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("left_power_phase_peak", &Uint8Type, 70, 0.7111111, 0, "degrees"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("right_power_phase", &Uint8Type, 71, 0.7111111, 0, "degrees"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("right_power_phase_peak", &Uint8Type, 72, 0.7111111, 0, "degrees"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("enhanced_speed", &Uint32Type, 73, 1000, 0, "m/s"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("enhanced_altitude", &Uint32Type, 78, 5, 500, "m"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("battery_soc", &Uint8Type, 81, 2, 0, "percent"));
    profiles[MesgNumRecord]->addField(new CFitFieldProfile("motor_power", &Uint16Type, 82, 0, 0, "watts"));

    profiles[MesgNumEvent] = new CFitProfile("event", MesgNumEvent);
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("event", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("event_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("data16", &Uint16Type, 2, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("data", &Uint32Type, 3, 0, 0, ""));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("timer_trigger", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("course_point_index", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("battery_level", &Uint16Type, 3, 1000, 0, "V"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("virtual_partner_speed", &Uint16Type, 3, 1000, 0, "m/s"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("hr_high_alert", &Uint8Type, 3, 0, 0, "bpm"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("hr_low_alert", &Uint8Type, 3, 0, 0, "bpm"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("speed_high_alert", &Uint32Type, 3, 1000, 0, "m/s"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("speed_low_alert", &Uint32Type, 3, 1000, 0, "m/s"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("cad_high_alert", &Uint16Type, 3, 0, 0, "rpm"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("cad_low_alert", &Uint16Type, 3, 0, 0, "rpm"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("power_high_alert", &Uint16Type, 3, 0, 0, "watts"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("power_low_alert", &Uint16Type, 3, 0, 0, "watts"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("time_duration_alert", &Uint32Type, 3, 1000, 0, "s"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("distance_duration_alert", &Uint32Type, 3, 100, 0, "m"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("calorie_duration_alert", &Uint32Type, 3, 0, 0, "calories"));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("fitness_equipment_state", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("sport_point", &Uint32Type, 3, 1, 0, ""));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("gear_change_data", &Uint32Type, 3, 1, 0, ""));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("rider_position", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumEvent]->getField(3)->addSubfield(new CFitFieldProfile("comm_timeout", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("event_group", &Uint8Type, 4, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("score", &Uint16Type, 7, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("opponent_score", &Uint16Type, 8, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("front_gear_num", &Uint8zType, 9, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("front_gear", &Uint8zType, 10, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("rear_gear_num", &Uint8zType, 11, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("rear_gear", &Uint8zType, 12, 0, 0, ""));
    profiles[MesgNumEvent]->addField(new CFitFieldProfile("device_index", &EnumType, 13, 0, 0, ""));

    profiles[MesgNumDeviceInfo] = new CFitProfile("device_info", MesgNumDeviceInfo);
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("device_index", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("device_type", &Uint8Type, 1, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->getField(1)->addSubfield(new CFitFieldProfile("antplus_device_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->getField(1)->addSubfield(new CFitFieldProfile("ant_device_type", &Uint8Type, 1, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("manufacturer", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("serial_number", &Uint32zType, 3, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("product", &Uint16Type, 4, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("software_version", &Uint16Type, 5, 100, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("hardware_version", &Uint8Type, 6, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("cum_operating_time", &Uint32Type, 7, 0, 0, "s"));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("battery_voltage", &Uint16Type, 10, 256, 0, "V"));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("battery_status", &EnumType, 11, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("sensor_position", &EnumType, 18, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("descriptor", &StringType, 19, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("ant_transmission_type", &Uint8zType, 20, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("ant_device_number", &Uint16zType, 21, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("ant_network", &EnumType, 22, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("source_type", &EnumType, 25, 0, 0, ""));
    profiles[MesgNumDeviceInfo]->addField(new CFitFieldProfile("product_name", &StringType, 27, 0, 0, ""));

    profiles[MesgNumTrainingFile] = new CFitProfile("training_file", MesgNumTrainingFile);
    profiles[MesgNumTrainingFile]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, ""));
    profiles[MesgNumTrainingFile]->addField(new CFitFieldProfile("type", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumTrainingFile]->addField(new CFitFieldProfile("manufacturer", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumTrainingFile]->addField(new CFitFieldProfile("product", &Uint16Type, 2, 0, 0, ""));
    profiles[MesgNumTrainingFile]->addField(new CFitFieldProfile("serial_number", &Uint32zType, 3, 0, 0, ""));
    profiles[MesgNumTrainingFile]->addField(new CFitFieldProfile("time_created", &EnumType, 4, 0, 0, ""));

    profiles[MesgNumHrv] = new CFitProfile("hrv", MesgNumHrv);
    profiles[MesgNumHrv]->addField(new CFitFieldProfile("time", &Uint16Type, 0, 1000, 0, "s"));

    profiles[MesgNumCameraEvent] = new CFitProfile("camera_event", MesgNumCameraEvent);
    profiles[MesgNumCameraEvent]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumCameraEvent]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, 0, 0, 0, "ms"));
    profiles[MesgNumCameraEvent]->addField(new CFitFieldProfile("camera_event_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumCameraEvent]->addField(new CFitFieldProfile("camera_file_uuid", &StringType, 2, 0, 0, ""));
    profiles[MesgNumCameraEvent]->addField(new CFitFieldProfile("camera_orientation", &EnumType, 3, 0, 0, ""));

    profiles[MesgNumGyroscopeData] = new CFitProfile("gyroscope_data", MesgNumGyroscopeData);
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, 0, 0, 0, "ms"));
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("sample_time_offset", &Uint16Type, 1, 0, 0, "ms"));
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("gyro_x", &Uint16Type, 2, 0, 0, "counts"));
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("gyro_y", &Uint16Type, 3, 0, 0, "counts"));
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("gyro_z", &Uint16Type, 4, 0, 0, "counts"));
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("calibrated_gyro_x", &Float32Type, 5, 0, 0, "deg/s"));
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("calibrated_gyro_y", &Float32Type, 6, 0, 0, "deg/s"));
    profiles[MesgNumGyroscopeData]->addField(new CFitFieldProfile("calibrated_gyro_z", &Float32Type, 7, 0, 0, "deg/s"));

    profiles[MesgNumAccelerometerData] = new CFitProfile("accelerometer_data", MesgNumAccelerometerData);
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, 0, 0, 0, "ms"));
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("sample_time_offset", &Uint16Type, 1, 0, 0, "ms"));
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("accel_x", &Uint16Type, 2, 0, 0, "counts"));
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("accel_y", &Uint16Type, 3, 0, 0, "counts"));
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("accel_z", &Uint16Type, 4, 0, 0, "counts"));
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("calibrated_accel_x", &Float32Type, 5, 0, 0, "g"));
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("calibrated_accel_y", &Float32Type, 6, 0, 0, "g"));
    profiles[MesgNumAccelerometerData]->addField(new CFitFieldProfile("calibrated_accel_z", &Float32Type, 7, 0, 0, "g"));

    profiles[MesgNumThreeDSensorCalibration] = new CFitProfile("three_d_sensor_calibration", MesgNumThreeDSensorCalibration);
    profiles[MesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("sensor_type", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("calibration_factor", &Uint32Type, 1, 0, 0, ""));
    profiles[MesgNumThreeDSensorCalibration]->getField(1)->addSubfield(new CFitFieldProfile("accel_cal_factor", &Uint32Type, 1, 0, 0, "g"));
    profiles[MesgNumThreeDSensorCalibration]->getField(1)->addSubfield(new CFitFieldProfile("gyro_cal_factor", &Uint32Type, 1, 0, 0, "deg/s"));
    profiles[MesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("calibration_divisor", &Uint32Type, 2, 0, 0, "counts"));
    profiles[MesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("level_shift", &Uint32Type, 3, 0, 0, ""));
    profiles[MesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("offset_cal", &Sint32Type, 4, 0, 0, ""));
    profiles[MesgNumThreeDSensorCalibration]->addField(new CFitFieldProfile("orientation_matrix", &Sint32Type, 5, 65535, 0, ""));

    profiles[MesgNumVideoFrame] = new CFitProfile("video_frame", MesgNumVideoFrame);
    profiles[MesgNumVideoFrame]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumVideoFrame]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, 0, 0, 0, "ms"));
    profiles[MesgNumVideoFrame]->addField(new CFitFieldProfile("frame_number", &Uint32Type, 1, 0, 0, ""));

    profiles[MesgNumObdiiData] = new CFitProfile("obdii_data", MesgNumObdiiData);
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, 0, 0, 0, "ms"));
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("time_offset", &Uint16Type, 1, 0, 0, "ms"));
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("pid", &ByteType, 2, 0, 0, ""));
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("raw_data", &ByteType, 3, 0, 0, ""));
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("pid_data_size", &Uint8Type, 4, 0, 0, ""));
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("system_time", &Uint32Type, 5, 0, 0, ""));
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("start_timestamp", &EnumType, 6, 0, 0, ""));
    profiles[MesgNumObdiiData]->addField(new CFitFieldProfile("start_timestamp_ms", &Uint16Type, 7, 0, 0, "ms"));

    profiles[MesgNumNmeaSentence] = new CFitProfile("nmea_sentence", MesgNumNmeaSentence);
    profiles[MesgNumNmeaSentence]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumNmeaSentence]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, 0, 0, 0, "ms"));
    profiles[MesgNumNmeaSentence]->addField(new CFitFieldProfile("sentence", &StringType, 1, 0, 0, ""));

    profiles[MesgNumAviationAttitude] = new CFitProfile("aviation_attitude", MesgNumAviationAttitude);
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("timestamp_ms", &Uint16Type, 0, 0, 0, "ms"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("system_time", &Uint32Type, 1, 0, 0, "ms"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("pitch", &Sint16Type, 2, 10430.38, 0, "radians"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("roll", &Sint16Type, 3, 10430.38, 0, "radians"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("accel_lateral", &Sint16Type, 4, 100, 0, "m/s^2"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("accel_normal", &Sint16Type, 5, 100, 0, "m/s^2"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("turn_rate", &Sint16Type, 6, 1024, 0, "radians/second"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("stage", &EnumType, 7, 0, 0, ""));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("attitude_stage_complete", &Uint8Type, 8, 0, 0, "%"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("track", &Uint16Type, 9, 10430.38, 0, "radians"));
    profiles[MesgNumAviationAttitude]->addField(new CFitFieldProfile("validity", &EnumType, 10, 0, 0, ""));

    profiles[MesgNumVideo] = new CFitProfile("video", MesgNumVideo);
    profiles[MesgNumVideo]->addField(new CFitFieldProfile("url", &StringType, 0, 0, 0, ""));
    profiles[MesgNumVideo]->addField(new CFitFieldProfile("hosting_provider", &StringType, 1, 0, 0, ""));
    profiles[MesgNumVideo]->addField(new CFitFieldProfile("duration", &Uint32Type, 2, 0, 0, "ms"));

    profiles[MesgNumVideoTitle] = new CFitProfile("video_title", MesgNumVideoTitle);
    profiles[MesgNumVideoTitle]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumVideoTitle]->addField(new CFitFieldProfile("message_count", &Uint16Type, 0, 0, 0, ""));
    profiles[MesgNumVideoTitle]->addField(new CFitFieldProfile("text", &StringType, 1, 0, 0, ""));

    profiles[MesgNumVideoDescription] = new CFitProfile("video_description", MesgNumVideoDescription);
    profiles[MesgNumVideoDescription]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumVideoDescription]->addField(new CFitFieldProfile("message_count", &Uint16Type, 0, 0, 0, ""));
    profiles[MesgNumVideoDescription]->addField(new CFitFieldProfile("text", &StringType, 1, 0, 0, ""));

    profiles[MesgNumVideoClip] = new CFitProfile("video_clip", MesgNumVideoClip);
    profiles[MesgNumVideoClip]->addField(new CFitFieldProfile("clip_number", &Uint16Type, 0, 0, 0, ""));
    profiles[MesgNumVideoClip]->addField(new CFitFieldProfile("start_timestamp", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumVideoClip]->addField(new CFitFieldProfile("start_timestamp_ms", &Uint16Type, 2, 0, 0, ""));
    profiles[MesgNumVideoClip]->addField(new CFitFieldProfile("end_timestamp", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumVideoClip]->addField(new CFitFieldProfile("end_timestamp_ms", &Uint16Type, 4, 0, 0, ""));
    profiles[MesgNumVideoClip]->addField(new CFitFieldProfile("clip_start", &Uint32Type, 6, 0, 0, "ms"));
    profiles[MesgNumVideoClip]->addField(new CFitFieldProfile("clip_end", &Uint32Type, 7, 0, 0, "ms"));

    profiles[MesgNumCourse] = new CFitProfile("course", MesgNumCourse);
    profiles[MesgNumCourse]->addField(new CFitFieldProfile("sport", &EnumType, 4, 0, 0, ""));
    profiles[MesgNumCourse]->addField(new CFitFieldProfile("name", &StringType, 5, 0, 0, ""));
    profiles[MesgNumCourse]->addField(new CFitFieldProfile("capabilities", &EnumType, 6, 0, 0, ""));

    profiles[MesgNumCoursePoint] = new CFitProfile("course_point", MesgNumCoursePoint);
    profiles[MesgNumCoursePoint]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumCoursePoint]->addField(new CFitFieldProfile("timestamp", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumCoursePoint]->addField(new CFitFieldProfile("position_lat", &Sint32Type, 2, 0, 0, "semicircles"));
    profiles[MesgNumCoursePoint]->addField(new CFitFieldProfile("position_long", &Sint32Type, 3, 0, 0, "semicircles"));
    profiles[MesgNumCoursePoint]->addField(new CFitFieldProfile("distance", &Uint32Type, 4, 100, 0, "m"));
    profiles[MesgNumCoursePoint]->addField(new CFitFieldProfile("type", &EnumType, 5, 0, 0, ""));
    profiles[MesgNumCoursePoint]->addField(new CFitFieldProfile("name", &StringType, 6, 0, 0, ""));
    profiles[MesgNumCoursePoint]->addField(new CFitFieldProfile("favorite", &EnumType, 8, 0, 0, ""));

    profiles[MesgNumSegmentId] = new CFitProfile("segment_id", MesgNumSegmentId);
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("name", &StringType, 0, 0, 0, ""));
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("uuid", &StringType, 1, 0, 0, ""));
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("sport", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("enabled", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("user_profile_primary_key", &Uint32Type, 4, 0, 0, ""));
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("device_id", &Uint32Type, 5, 0, 0, ""));
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("default_race_leader", &Uint8Type, 6, 0, 0, ""));
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("delete_status", &EnumType, 7, 0, 0, ""));
    profiles[MesgNumSegmentId]->addField(new CFitFieldProfile("selection_type", &EnumType, 8, 0, 0, ""));

    profiles[MesgNumSegmentLeaderboardEntry] = new CFitProfile("segment_leaderboard_entry", MesgNumSegmentLeaderboardEntry);
    profiles[MesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("name", &StringType, 0, 0, 0, ""));
    profiles[MesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("group_primary_key", &Uint32Type, 2, 0, 0, ""));
    profiles[MesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("activity_id", &Uint32Type, 3, 0, 0, ""));
    profiles[MesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("segment_time", &Uint32Type, 4, 1000, 0, "s"));
    profiles[MesgNumSegmentLeaderboardEntry]->addField(new CFitFieldProfile("activity_id_string", &StringType, 5, 0, 0, ""));

    profiles[MesgNumSegmentPoint] = new CFitProfile("segment_point", MesgNumSegmentPoint);
    profiles[MesgNumSegmentPoint]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumSegmentPoint]->addField(new CFitFieldProfile("position_lat", &Sint32Type, 1, 0, 0, "semicircles"));
    profiles[MesgNumSegmentPoint]->addField(new CFitFieldProfile("position_long", &Sint32Type, 2, 0, 0, "semicircles"));
    profiles[MesgNumSegmentPoint]->addField(new CFitFieldProfile("distance", &Uint32Type, 3, 100, 0, "m"));
    profiles[MesgNumSegmentPoint]->addField(new CFitFieldProfile("altitude", &Uint16Type, 4, 5, 500, "m"));
    profiles[MesgNumSegmentPoint]->addField(new CFitFieldProfile("leader_time", &Uint32Type, 5, 1000, 0, "s"));

    profiles[MesgNumSegmentLap] = new CFitProfile("segment_lap", MesgNumSegmentLap);
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("event", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("event_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("start_time", &EnumType, 2, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("start_position_lat", &Sint32Type, 3, 0, 0, "semicircles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("start_position_long", &Sint32Type, 4, 0, 0, "semicircles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("end_position_lat", &Sint32Type, 5, 0, 0, "semicircles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("end_position_long", &Sint32Type, 6, 0, 0, "semicircles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_elapsed_time", &Uint32Type, 7, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_timer_time", &Uint32Type, 8, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_distance", &Uint32Type, 9, 100, 0, "m"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_cycles", &Uint32Type, 10, 0, 0, "cycles"));
    profiles[MesgNumSegmentLap]->getField(10)->addSubfield(new CFitFieldProfile("total_strokes", &Uint32Type, 10, 0, 0, "strokes"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_calories", &Uint16Type, 11, 0, 0, "kcal"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_fat_calories", &Uint16Type, 12, 0, 0, "kcal"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_speed", &Uint16Type, 13, 1000, 0, "m/s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_speed", &Uint16Type, 14, 1000, 0, "m/s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_heart_rate", &Uint8Type, 15, 0, 0, "bpm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_heart_rate", &Uint8Type, 16, 0, 0, "bpm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_cadence", &Uint8Type, 17, 0, 0, "rpm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_cadence", &Uint8Type, 18, 0, 0, "rpm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_power", &Uint16Type, 19, 0, 0, "watts"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_power", &Uint16Type, 20, 0, 0, "watts"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_ascent", &Uint16Type, 21, 0, 0, "m"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_descent", &Uint16Type, 22, 0, 0, "m"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("sport", &EnumType, 23, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("event_group", &Uint8Type, 24, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("nec_lat", &Sint32Type, 25, 0, 0, "semicircles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("nec_long", &Sint32Type, 26, 0, 0, "semicircles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("swc_lat", &Sint32Type, 27, 0, 0, "semicircles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("swc_long", &Sint32Type, 28, 0, 0, "semicircles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("name", &StringType, 29, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("normalized_power", &Uint16Type, 30, 0, 0, "watts"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("left_right_balance", &EnumType, 31, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("sub_sport", &EnumType, 32, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_work", &Uint32Type, 33, 0, 0, "J"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_altitude", &Uint16Type, 34, 5, 500, "m"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_altitude", &Uint16Type, 35, 5, 500, "m"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("gps_accuracy", &Uint8Type, 36, 0, 0, "m"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_grade", &Sint16Type, 37, 100, 0, "%"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_pos_grade", &Sint16Type, 38, 100, 0, "%"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_neg_grade", &Sint16Type, 39, 100, 0, "%"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_pos_grade", &Sint16Type, 40, 100, 0, "%"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_neg_grade", &Sint16Type, 41, 100, 0, "%"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_temperature", &Sint8Type, 42, 0, 0, "C"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_temperature", &Sint8Type, 43, 0, 0, "C"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_moving_time", &Uint32Type, 44, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_pos_vertical_speed", &Sint16Type, 45, 1000, 0, "m/s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_neg_vertical_speed", &Sint16Type, 46, 1000, 0, "m/s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_pos_vertical_speed", &Sint16Type, 47, 1000, 0, "m/s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_neg_vertical_speed", &Sint16Type, 48, 1000, 0, "m/s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("time_in_hr_zone", &Uint32Type, 49, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("time_in_speed_zone", &Uint32Type, 50, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("time_in_cadence_zone", &Uint32Type, 51, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("time_in_power_zone", &Uint32Type, 52, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("repetition_num", &Uint16Type, 53, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("min_altitude", &Uint16Type, 54, 5, 500, "m"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("min_heart_rate", &Uint8Type, 55, 0, 0, "bpm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("active_time", &Uint32Type, 56, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("wkt_step_index", &EnumType, 57, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("sport_event", &EnumType, 58, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_torque_effectiveness", &Uint8Type, 59, 2, 0, "percent"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_torque_effectiveness", &Uint8Type, 60, 2, 0, "percent"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_pedal_smoothness", &Uint8Type, 61, 2, 0, "percent"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_pedal_smoothness", &Uint8Type, 62, 2, 0, "percent"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_combined_pedal_smoothness", &Uint8Type, 63, 2, 0, "percent"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("status", &EnumType, 64, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("uuid", &StringType, 65, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_fractional_cadence", &Uint8Type, 66, 128, 0, "rpm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_fractional_cadence", &Uint8Type, 67, 128, 0, "rpm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("total_fractional_cycles", &Uint8Type, 68, 128, 0, "cycles"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("front_gear_shift_count", &Uint16Type, 69, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("rear_gear_shift_count", &Uint16Type, 70, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("time_standing", &Uint32Type, 71, 1000, 0, "s"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("stand_count", &Uint16Type, 72, 0, 0, ""));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_pco", &Sint8Type, 73, 0, 0, "mm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_pco", &Sint8Type, 74, 0, 0, "mm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_power_phase", &Uint8Type, 75, 0.7111111, 0, "degrees"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_left_power_phase_peak", &Uint8Type, 76, 0.7111111, 0, "degrees"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_power_phase", &Uint8Type, 77, 0.7111111, 0, "degrees"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_right_power_phase_peak", &Uint8Type, 78, 0.7111111, 0, "degrees"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_power_position", &Uint16Type, 79, 0, 0, "watts"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_power_position", &Uint16Type, 80, 0, 0, "watts"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("avg_cadence_position", &Uint8Type, 81, 0, 0, "rpm"));
    profiles[MesgNumSegmentLap]->addField(new CFitFieldProfile("max_cadence_position", &Uint8Type, 82, 0, 0, "rpm"));

    profiles[MesgNumSegmentFile] = new CFitProfile("segment_file", MesgNumSegmentFile);
    profiles[MesgNumSegmentFile]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumSegmentFile]->addField(new CFitFieldProfile("file_uuid", &StringType, 1, 0, 0, ""));
    profiles[MesgNumSegmentFile]->addField(new CFitFieldProfile("enabled", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumSegmentFile]->addField(new CFitFieldProfile("user_profile_primary_key", &Uint32Type, 4, 0, 0, ""));
    profiles[MesgNumSegmentFile]->addField(new CFitFieldProfile("leader_type", &EnumType, 7, 0, 0, ""));
    profiles[MesgNumSegmentFile]->addField(new CFitFieldProfile("leader_group_primary_key", &Uint32Type, 8, 0, 0, ""));
    profiles[MesgNumSegmentFile]->addField(new CFitFieldProfile("leader_activity_id", &Uint32Type, 9, 0, 0, ""));
    profiles[MesgNumSegmentFile]->addField(new CFitFieldProfile("leader_activity_id_string", &StringType, 10, 0, 0, ""));

    profiles[MesgNumWorkout] = new CFitProfile("workout", MesgNumWorkout);
    profiles[MesgNumWorkout]->addField(new CFitFieldProfile("sport", &EnumType, 4, 0, 0, ""));
    profiles[MesgNumWorkout]->addField(new CFitFieldProfile("capabilities", &EnumType, 5, 0, 0, ""));
    profiles[MesgNumWorkout]->addField(new CFitFieldProfile("num_valid_steps", &Uint16Type, 6, 0, 0, ""));
    profiles[MesgNumWorkout]->addField(new CFitFieldProfile("wkt_name", &StringType, 8, 0, 0, ""));

    profiles[MesgNumWorkoutStep] = new CFitProfile("workout_step", MesgNumWorkoutStep);
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("wkt_step_name", &StringType, 0, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("duration_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("duration_value", &Uint32Type, 2, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->getField(2)->addSubfield(new CFitFieldProfile("duration_time", &Uint32Type, 2, 1000, 0, "s"));
    profiles[MesgNumWorkoutStep]->getField(2)->addSubfield(new CFitFieldProfile("duration_distance", &Uint32Type, 2, 100, 0, "m"));
    profiles[MesgNumWorkoutStep]->getField(2)->addSubfield(new CFitFieldProfile("duration_hr", &EnumType, 2, 0, 0, "% or bpm"));
    profiles[MesgNumWorkoutStep]->getField(2)->addSubfield(new CFitFieldProfile("duration_calories", &Uint32Type, 2, 0, 0, "calories"));
    profiles[MesgNumWorkoutStep]->getField(2)->addSubfield(new CFitFieldProfile("duration_step", &Uint32Type, 2, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->getField(2)->addSubfield(new CFitFieldProfile("duration_power", &EnumType, 2, 0, 0, "% or watts"));
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("target_type", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("target_value", &Uint32Type, 4, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->getField(4)->addSubfield(new CFitFieldProfile("target_hr_zone", &Uint32Type, 4, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->getField(4)->addSubfield(new CFitFieldProfile("target_power_zone", &Uint32Type, 4, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->getField(4)->addSubfield(new CFitFieldProfile("repeat_steps", &Uint32Type, 4, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->getField(4)->addSubfield(new CFitFieldProfile("repeat_time", &Uint32Type, 4, 1000, 0, "s"));
    profiles[MesgNumWorkoutStep]->getField(4)->addSubfield(new CFitFieldProfile("repeat_distance", &Uint32Type, 4, 100, 0, "m"));
    profiles[MesgNumWorkoutStep]->getField(4)->addSubfield(new CFitFieldProfile("repeat_calories", &Uint32Type, 4, 0, 0, "calories"));
    profiles[MesgNumWorkoutStep]->getField(4)->addSubfield(new CFitFieldProfile("repeat_hr", &EnumType, 4, 0, 0, "% or bpm"));
    profiles[MesgNumWorkoutStep]->getField(4)->addSubfield(new CFitFieldProfile("repeat_power", &EnumType, 4, 0, 0, "% or watts"));
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("custom_target_value_low", &Uint32Type, 5, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->getField(5)->addSubfield(new CFitFieldProfile("custom_target_speed_low", &Uint32Type, 5, 1000, 0, "m/s"));
    profiles[MesgNumWorkoutStep]->getField(5)->addSubfield(new CFitFieldProfile("custom_target_heart_rate_low", &EnumType, 5, 0, 0, "% or bpm"));
    profiles[MesgNumWorkoutStep]->getField(5)->addSubfield(new CFitFieldProfile("custom_target_cadence_low", &Uint32Type, 5, 0, 0, "rpm"));
    profiles[MesgNumWorkoutStep]->getField(5)->addSubfield(new CFitFieldProfile("custom_target_power_low", &EnumType, 5, 0, 0, "% or watts"));
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("custom_target_value_high", &Uint32Type, 6, 0, 0, ""));
    profiles[MesgNumWorkoutStep]->getField(6)->addSubfield(new CFitFieldProfile("custom_target_speed_high", &Uint32Type, 6, 1000, 0, "m/s"));
    profiles[MesgNumWorkoutStep]->getField(6)->addSubfield(new CFitFieldProfile("custom_target_heart_rate_high", &EnumType, 6, 0, 0, "% or bpm"));
    profiles[MesgNumWorkoutStep]->getField(6)->addSubfield(new CFitFieldProfile("custom_target_cadence_high", &Uint32Type, 6, 0, 0, "rpm"));
    profiles[MesgNumWorkoutStep]->getField(6)->addSubfield(new CFitFieldProfile("custom_target_power_high", &EnumType, 6, 0, 0, "% or watts"));
    profiles[MesgNumWorkoutStep]->addField(new CFitFieldProfile("intensity", &EnumType, 7, 0, 0, ""));

    profiles[MesgNumSchedule] = new CFitProfile("schedule", MesgNumSchedule);
    profiles[MesgNumSchedule]->addField(new CFitFieldProfile("manufacturer", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumSchedule]->addField(new CFitFieldProfile("product", &Uint16Type, 1, 0, 0, ""));
    profiles[MesgNumSchedule]->addField(new CFitFieldProfile("serial_number", &Uint32zType, 2, 0, 0, ""));
    profiles[MesgNumSchedule]->addField(new CFitFieldProfile("time_created", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumSchedule]->addField(new CFitFieldProfile("completed", &EnumType, 4, 0, 0, ""));
    profiles[MesgNumSchedule]->addField(new CFitFieldProfile("type", &EnumType, 5, 0, 0, ""));
    profiles[MesgNumSchedule]->addField(new CFitFieldProfile("scheduled_time", &EnumType, 6, 0, 0, ""));

    profiles[MesgNumTotals] = new CFitProfile("totals", MesgNumTotals);
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("message_index", &EnumType, 254, 0, 0, ""));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("timer_time", &Uint32Type, 0, 0, 0, "s"));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("distance", &Uint32Type, 1, 0, 0, "m"));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("calories", &Uint32Type, 2, 0, 0, "kcal"));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("sport", &EnumType, 3, 0, 0, ""));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("elapsed_time", &Uint32Type, 4, 0, 0, "s"));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("sessions", &Uint16Type, 5, 0, 0, ""));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("active_time", &Uint32Type, 6, 0, 0, "s"));
    profiles[MesgNumTotals]->addField(new CFitFieldProfile("sport_index", &Uint8Type, 9, 0, 0, ""));

    profiles[MesgNumWeightScale] = new CFitProfile("weight_scale", MesgNumWeightScale);
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("weight", &EnumType, 0, 100, 0, "kg"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("percent_fat", &Uint16Type, 1, 100, 0, "%"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("percent_hydration", &Uint16Type, 2, 100, 0, "%"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("visceral_fat_mass", &Uint16Type, 3, 100, 0, "kg"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("bone_mass", &Uint16Type, 4, 100, 0, "kg"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("muscle_mass", &Uint16Type, 5, 100, 0, "kg"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("basal_met", &Uint16Type, 7, 4, 0, "kcal/day"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("physique_rating", &Uint8Type, 8, 0, 0, ""));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("active_met", &Uint16Type, 9, 4, 0, "kcal/day"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("metabolic_age", &Uint8Type, 10, 0, 0, "years"));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("visceral_fat_rating", &Uint8Type, 11, 0, 0, ""));
    profiles[MesgNumWeightScale]->addField(new CFitFieldProfile("user_profile_index", &EnumType, 12, 0, 0, ""));

    profiles[MesgNumBloodPressure] = new CFitProfile("blood_pressure", MesgNumBloodPressure);
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("systolic_pressure", &Uint16Type, 0, 0, 0, "mmHg"));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("diastolic_pressure", &Uint16Type, 1, 0, 0, "mmHg"));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("mean_arterial_pressure", &Uint16Type, 2, 0, 0, "mmHg"));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("map_3_sample_mean", &Uint16Type, 3, 0, 0, "mmHg"));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("map_morning_values", &Uint16Type, 4, 0, 0, "mmHg"));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("map_evening_values", &Uint16Type, 5, 0, 0, "mmHg"));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("heart_rate", &Uint8Type, 6, 0, 0, "bpm"));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("heart_rate_type", &EnumType, 7, 0, 0, ""));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("status", &EnumType, 8, 0, 0, ""));
    profiles[MesgNumBloodPressure]->addField(new CFitFieldProfile("user_profile_index", &EnumType, 9, 0, 0, ""));

    profiles[MesgNumMonitoringInfo] = new CFitProfile("monitoring_info", MesgNumMonitoringInfo);
    profiles[MesgNumMonitoringInfo]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumMonitoringInfo]->addField(new CFitFieldProfile("local_timestamp", &EnumType, 0, 0, 0, "s"));
    profiles[MesgNumMonitoringInfo]->addField(new CFitFieldProfile("activity_type", &EnumType, 1, 0, 0, ""));
    profiles[MesgNumMonitoringInfo]->addField(new CFitFieldProfile("cycles_to_distance", &Uint16Type, 3, 5000, 0, "m/cycle"));
    profiles[MesgNumMonitoringInfo]->addField(new CFitFieldProfile("cycles_to_calories", &Uint16Type, 4, 5000, 0, "kcal/cycle"));
    profiles[MesgNumMonitoringInfo]->addField(new CFitFieldProfile("resting_metabolic_rate", &Uint16Type, 5, 0, 0, "kcal / day"));

    profiles[MesgNumMonitoring] = new CFitProfile("monitoring", MesgNumMonitoring);
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("timestamp", &EnumType, 253, 0, 0, "s"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("device_index", &EnumType, 0, 0, 0, ""));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("calories", &Uint16Type, 1, 0, 0, "kcal"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("distance", &Uint32Type, 2, 100, 0, "m"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("cycles", &Uint32Type, 3, 2, 0, "cycles"));
    profiles[MesgNumMonitoring]->getField(3)->addSubfield(new CFitFieldProfile("steps", &Uint32Type, 3, 1, 0, "steps"));
    profiles[MesgNumMonitoring]->getField(3)->addSubfield(new CFitFieldProfile("strokes", &Uint32Type, 3, 2, 0, "strokes"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("active_time", &Uint32Type, 4, 1000, 0, "s"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("activity_type", &EnumType, 5, 0, 0, ""));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("activity_subtype", &EnumType, 6, 0, 0, ""));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("activity_level", &EnumType, 7, 0, 0, ""));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("distance_16", &Uint16Type, 8, 0, 0, "100 * m"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("cycles_16", &Uint16Type, 9, 0, 0, "2 * cycles (steps)"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("active_time_16", &Uint16Type, 10, 0, 0, "s"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("local_timestamp", &EnumType, 11, 0, 0, ""));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("temperature", &Sint16Type, 12, 100, 0, "C"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("temperature_min", &Sint16Type, 14, 100, 0, "C"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("temperature_max", &Sint16Type, 15, 100, 0, "C"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("activity_time", &Uint16Type, 16, 0, 0, "minutes"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("active_calories", &Uint16Type, 19, 0, 0, "kcal"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("current_activity_type_intensity", &ByteType, 24, 0, 0, ""));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("timestamp_min_8", &Uint8Type, 25, 0, 0, "min"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("timestamp_16", &Uint16Type, 26, 0, 0, "s"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("heart_rate", &Uint8Type, 27, 0, 0, "bpm"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("intensity", &Uint8Type, 28, 10, 0, ""));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("duration_min", &Uint16Type, 29, 0, 0, "min"));
    profiles[MesgNumMonitoring]->addField(new CFitFieldProfile("duration", &Uint32Type, 30, 0, 0, "s"));

    profiles[MesgNumMemoGlob] = new CFitProfile("memo_glob", MesgNumMemoGlob);
    profiles[MesgNumMemoGlob]->addField(new CFitFieldProfile("part_index", &Uint32Type, 250, 0, 0, ""));
    profiles[MesgNumMemoGlob]->addField(new CFitFieldProfile("memo", &ByteType, 0, 0, 0, ""));
    profiles[MesgNumMemoGlob]->addField(new CFitFieldProfile("message_number", &Uint16Type, 1, 0, 0, ""));
    profiles[MesgNumMemoGlob]->addField(new CFitFieldProfile("message_index", &EnumType, 2, 0, 0, ""));


// ----------- end generated code -----------

    profiles[GlobalMesgNrInvalid] = new CFitProfile();
    profiles[GlobalMesgNrInvalid]->addField(new CFitFieldProfile());
    return profiles;
}

QMap<uint16_t, CFitProfile*> allProfiles = initProfiles();


CFitProfile* CFitProfileLockup::getProfile(uint16_t globalMesgNr)
{
    if (allProfiles.contains(globalMesgNr))
    {
        return allProfiles[globalMesgNr];
    }
    //qDebug() << "unknown profile " << globalMesgNr << " " << " (glob msg)";
    return allProfiles[GlobalMesgNrInvalid];
}

CFitFieldProfile * CFitProfileLockup::getFieldForProfile(uint16_t globalMesgNr, uint8_t fieldDefNr)
{
    CFitFieldProfile* f = nullptr;
    if (allProfiles.contains(globalMesgNr)) {
        return  allProfiles[globalMesgNr]->getField(fieldDefNr);
    }
    //if(!f)
    //    qDebug() << "unknown profile " << globalMesgNr << " " << fieldDefNr << " (glob msg, field def)";
    return allProfiles[GlobalMesgNrInvalid]->getField(FieldDefNrInvalid);
}