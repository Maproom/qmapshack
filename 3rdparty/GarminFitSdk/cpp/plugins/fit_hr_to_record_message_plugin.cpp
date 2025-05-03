/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023 Garmin International, Inc.
// Licensed under the Flexible and Interoperable Data Transfer (FIT) Protocol License; you
// may not use this file except in compliance with the Flexible and Interoperable Data
// Transfer (FIT) Protocol License.
/////////////////////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "fit_file_id_mesg.hpp"
#include "fit_hr_mesg.hpp"
#include "fit_record_mesg.hpp"
#include "fit_runtime_exception.hpp"
#include "fit_session_mesg.hpp"
#include "fit_hr_to_record_message_plugin.hpp"

namespace fit
{
	/**
	 * Provides functionality to backfill record messages with hr data from HR mesg.
	 * <p>
	 * The plugin matches the timestamp of record messages with the timestamps hr data
	 * contained in the HR mesg.
	 * <p>
	 * Requirements for correct operation:
	 *    - HR data must be in the order of increasing timestamp
	 *    - Record data must be in the order of increasing timestamp
	 *    - The order of incoming HR and record mesgs may be independent of each other
	 *    - The first HR mesg must contain a timestamp (with optional fractional_timestamp)
	 *      which, in combination with the event_timestamps that mark the bpm data, provide the
	 *      time that is used to match record data.
	 *    - There must be an equal number of filtered_bpm fields and event_timestamp fields in each
	 *      HR mesg; this number may change from message to message.
	 *
	 */
	HRToRecordMesgBroadcastPlugin::HRToRecordMesgBroadcastPlugin()
	{
		record_range_start_time = new DateTime((FIT_DATE_TIME)0);
		hr_start_index = INVALID_INDEX;
		hr_start_sub_index = INVALID_INDEX;
		isActivityFile = FIT_FALSE;
		mesg_count = 0;

		hr_mesgs = 0;
		record_mesgs = 0;
	}

	HRToRecordMesgBroadcastPlugin::~HRToRecordMesgBroadcastPlugin()
	{
		if (record_range_start_time != NULL)
			delete record_range_start_time;
	}
	/**
   * Peeks messages as they are being added to the buffer
   *
   * @param mesg the message that has just been buffered by BufferedMesgBroadcaster
   */
	void HRToRecordMesgBroadcastPlugin::OnIncomingMesg(const Mesg& mesg)
	{
		switch (mesg.GetNum())
		{

		case FIT_MESG_NUM_FILE_ID:
		{
			// Check to see if we are processing an activity file.
			FileIdMesg fileIdMesg = FileIdMesg(mesg);
			if (fileIdMesg.GetType() == FIT_FILE_ACTIVITY)
				isActivityFile = true;
		}
		break;

		case FIT_MESG_NUM_SESSION:
		{
			// Obtain session start time to mark the start of the first
			// record message's time range
			SessionMesg sessionMesg = SessionMesg(mesg);
			record_range_start_time = new DateTime(sessionMesg.GetStartTime());
		}
		break;

		case FIT_MESG_NUM_HR:
			hr_mesgs++;
			if (hr_start_index == INVALID_INDEX)
			{
				// Mark the first appearance of an HR message
				hr_start_index = mesg_count;
				hr_start_sub_index = 0;
			}
			break;

		case FIT_MESG_NUM_RECORD:
			record_mesgs++;
			break;

		default:
			break;

		} // switch

		mesg_count++;
	}
	/**
	 * Matches record time ranges with all time matching HR mesgs and updates the
	 * message stream for later broadcast to listeners.
	 *
	 * @param mesgs the message list that is about to be broadcast to all MesgListeners.  \
					Note: The List is 'final' but the references within the list are not, \
					therefore editing Mesg objects within mesgs will alter the messages   \
					that are broadcast to listeners.
	 *
	 * DO NOT add or remove any messages to mesgs
	 */
	void HRToRecordMesgBroadcastPlugin::OnBroadcast(std::vector<Mesg>& mesgs)
	{
		// Check if we have an activity file and have received HR messages
		if (isActivityFile && (hr_start_index != INVALID_INDEX))
		{
			FIT_FLOAT32 hr_anchor_event_timestamp = 0.0;
			DateTime hr_anchor_timestamp = DateTime((FIT_DATE_TIME)0);
			FIT_BOOL hr_anchor_set = FIT_FALSE;
			FIT_UINT8 last_valid_hr = 0;
			DateTime last_valid_hr_time = DateTime((FIT_DATE_TIME)0);

			for (FIT_UINT32 mesgCounter = 0; mesgCounter < mesgs.size(); ++mesgCounter)
			{
				Mesg &mesg = mesgs.at(mesgCounter);

				// Process any record messages we encounter
				if (mesg.GetNum() == FIT_MESG_NUM_RECORD)
				{
					FIT_FLOAT32 hrSum = 0;
					FIT_FLOAT32 hrSumCount = 0;

					// Obtain the time for which the record message is valid
					FIT_DATE_TIME timestamp = mesg.GetFieldUINT32Value(253, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
					DateTime record_range_end_time = DateTime(timestamp);

					// Need to determine timestamp range which applies to this record
					FIT_BOOL findingInRangeHrMesgs = FIT_TRUE;

					// Start searching HR mesgs where we left off
					FIT_UINT32 hr_mesg_counter = hr_start_index;
					FIT_UINT32 hr_sub_mesg_counter = hr_start_sub_index;

					while (findingInRangeHrMesgs && (hr_mesg_counter < mesgs.size()))
					{
						// Skip over any non HR messages
						if (mesgs.at(hr_mesg_counter).GetNum() == FIT_MESG_NUM_HR)
						{
							HrMesg hrMesg = HrMesg(mesgs.at(hr_mesg_counter));

							// Update HR timestamp anchor, if present
							if (hrMesg.GetTimestamp() != FIT_UINT32_INVALID)
							{
								hr_anchor_timestamp = DateTime(hrMesg.GetTimestamp());
								hr_anchor_set = FIT_TRUE;

								FIT_FLOAT32 fractionalTimestamp = hrMesg.GetFractionalTimestamp();
								if (memcmp(&fractionalTimestamp, &FIT_FLOAT32_INVALID, sizeof(FIT_FLOAT32)) != 0)
									hr_anchor_timestamp.add(fractionalTimestamp);

								if (hrMesg.GetNumEventTimestamp() == 1)
								{
									hr_anchor_event_timestamp = hrMesg.GetEventTimestamp(0);
								}
								else
								{
									throw RuntimeException("FIT HrToRecordMesgBroadcastPlugin Error: Anchor HR mesg must have 1 event_timestamp");
								}
							}

							if (hr_anchor_set == FIT_FALSE)
							{
								// We cannot process any HR messages if we have not received a timestamp anchor
								throw RuntimeException("FIT HrToRecordMesgBroadcastPlugin Error: No anchor timestamp received in a HR mesg before diff HR mesgs");
							}
							else if (hrMesg.GetNumEventTimestamp() != hrMesg.GetNumFilteredBpm())
							{
								throw RuntimeException("FIT HrToRecordMesgBroadcastPlugin Error: HR mesg with mismatching event timestamp and filtered bpm");
							}

							for (FIT_UINT8 j = (FIT_UINT8)hr_sub_mesg_counter; j < hrMesg.GetNumEventTimestamp(); j++)
							{
								// Build up timestamp for each message using the anchor and event_timestamp
								DateTime hrMesgTime = DateTime(hr_anchor_timestamp);
								FIT_FLOAT32 event_timestamp = hrMesg.GetEventTimestamp(j);

								// Deal with roll over case
								if (event_timestamp < hr_anchor_event_timestamp)
								{
									if ((hr_anchor_event_timestamp - event_timestamp) > (1 << 21))
									{
										event_timestamp += (1 << 22);
									}
									else
									{
										throw RuntimeException("FIT HrToRecordMesgBroadcastPlugin Error: Anchor event_timestamp is greater than subsequent event_timestamp. This does not allow for correct delta calculation.");
									}
								}

								hrMesgTime.add(event_timestamp - hr_anchor_event_timestamp);

								// Check if hrMesgTime is gt record start time
								// and if hrMesgTime is lte to record end time
								if ((hrMesgTime.CompareTo(*record_range_start_time) > 0) &&
									(hrMesgTime.CompareTo(record_range_end_time) <= 0))
								{
									hrSum += hrMesg.GetFilteredBpm(j);
									hrSumCount++;
									last_valid_hr_time = hrMesgTime;
								}
								// check if hrMesgTime exceeds the record time
								else if (hrMesgTime.CompareTo(record_range_end_time) > 0)
								{
									// Remember where we left off
									hr_start_index = hr_mesg_counter;
									hr_start_sub_index = j;
									findingInRangeHrMesgs = false;

									if (hrSumCount > 0)
									{
										// Update record heart rate
										last_valid_hr = (FIT_UINT8)round(((float)hrSum) / hrSumCount);
										mesg.SetFieldUINT8Value(3, last_valid_hr, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
									}
									// If no stored HR is available, fill in record messages with the
									// last valid filtered hr for a maximum of 5 seconds
									else if ((record_range_start_time->CompareTo(last_valid_hr_time) > 0) &&
										((record_range_start_time->GetTimeStamp() - last_valid_hr_time.GetTimeStamp()) < 5))
									{
										mesg.SetFieldUINT8Value(3, last_valid_hr, 0, FIT_SUBFIELD_INDEX_MAIN_FIELD);
									}

									// Reset HR average
									hrSum = 0;
									hrSumCount = 0;

									record_range_start_time = new DateTime(record_range_end_time);

									// Breaks out of looping within the event_timestamp array
									break;
								}
							}
						}
						hr_mesg_counter++;
						hr_sub_mesg_counter = 0;
					} // while
				}
			}// for
		}
	}

} // namespace fit
