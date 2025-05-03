/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023 Garmin International, Inc.
// Licensed under the Flexible and Interoperable Data Transfer (FIT) Protocol License; you
// may not use this file except in compliance with the Flexible and Interoperable Data
// Transfer (FIT) Protocol License.
/////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(FIT_HR_TO_RECORD_MESSAGE_PLUGIN_HPP)
#define FIT_HR_TO_RECORD_MESSAGE_PLUGIN_HPP

#include "fit.hpp"
#include "fit_date_time.hpp"
#include "fit_mesg_broadcast_plugin.hpp"
#include <vector>

namespace fit
{

class HRToRecordMesgBroadcastPlugin : public fit::MesgBroadcastPlugin
{
   public:
      HRToRecordMesgBroadcastPlugin();
      ~HRToRecordMesgBroadcastPlugin();
      void OnBroadcast(std::vector<Mesg>& mesgs);
      void OnIncomingMesg(const Mesg& mesg);
      // Plugins are not designed to be copied
      HRToRecordMesgBroadcastPlugin &operator=(const HRToRecordMesgBroadcastPlugin&) = delete;
      HRToRecordMesgBroadcastPlugin(const HRToRecordMesgBroadcastPlugin&) = delete;

      FIT_UINT32 GetHRMesgs() { return hr_mesgs; };
      FIT_UINT32 GetRecordMesgs() { return record_mesgs; };

   private:
      DateTime *record_range_start_time;
      FIT_SINT32 hr_start_index;
      FIT_SINT32 hr_start_sub_index;
      FIT_BOOL isActivityFile;
      FIT_UINT32 mesg_count;
      const FIT_SINT32 INVALID_INDEX = -1;
      FIT_UINT32 hr_mesgs;
      FIT_UINT32 record_mesgs;
};

} // namespace fit

#endif // defined(FIT_HR_TO_RECORD_MESSAGE_PLUGIN_HPP)
