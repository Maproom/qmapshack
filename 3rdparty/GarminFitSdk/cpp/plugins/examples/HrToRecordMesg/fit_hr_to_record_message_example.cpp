////////////////////////////////////////////////////////////////////////////////
// The following FIT Protocol software provided may be used with FIT protocol
// devices only and remains the copyrighted property of Garmin Canada Inc.
// The software is being provided on an "as-is" basis and as an accommodation,
// and therefore all warranties, representations, or guarantees of any kind
// (whether express, implied or statutory) including, without limitation,
// warranties of merchantability, non-infringement, or fitness for a particular
// purpose, are specifically disclaimed.
//
// Copyright 2015 Garmin Canada Inc.
////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>

#include "fit_decode.hpp"
#include "fit_buffered_mesg_broadcaster.hpp"
#include "fit_hr_to_record_message_plugin.hpp"

class Listener : public fit::RecordMesgListener, public fit::HrMesgListener
{
public:

   void OnMesg(fit::RecordMesg& mesg)
   {
      FIT_UINT8 localNum = mesg.GetLocalNum();
      FIT_FLOAT32 distance = mesg.GetDistance();
      FIT_FLOAT32 speed = mesg.GetSpeed();
      FIT_FLOAT32 enhancedSpeed = mesg.GetEnhancedSpeed();

      fprintf(fileDescriptor, "Data,%d,record,", localNum);
      if (mesg.GetTimestamp() != FIT_DATE_TIME_INVALID)
      {
         fprintf(fileDescriptor, "timestamp,%d,s,", mesg.GetTimestamp());
      }
      if (memcmp(&distance, fit::baseTypeInvalids[float32Num], sizeof(FIT_FLOAT32)) != 0)
      {
         fprintf(fileDescriptor, "distance, %.1f, m,", distance);
      }
      if (memcmp(&speed, fit::baseTypeInvalids[float32Num], sizeof(FIT_FLOAT32)) != 0)
      {
         fprintf(fileDescriptor, "speed,%.3f,m/s,", speed);
      }
      if (mesg.GetCadence() != FIT_UINT8_INVALID)
      {
         fprintf(fileDescriptor, "cadence,%d,rpm,", mesg.GetCadence());
      }
      if (memcmp(&enhancedSpeed, fit::baseTypeInvalids[float32Num], sizeof(FIT_FLOAT32)) != 0)
      {
         fprintf(fileDescriptor, "enhanced_speed,%.3f,m/s,", enhancedSpeed);
      }
      if (mesg.GetHeartRate() != FIT_UINT8_INVALID)
      {
         fprintf(fileDescriptor, "heart_rate,%d,bpm,", mesg.GetHeartRate());
      }
      fprintf(fileDescriptor, "\n");
   }

   void OnMesg(fit::HrMesg& mesg)
   {
      FIT_UINT8 localNum = mesg.GetLocalNum();
      FIT_FLOAT32 fractionalTs = mesg.GetFractionalTimestamp();

      int count;

      fprintf(fileDescriptor, "Data,%d,hr,", localNum);
      if (mesg.GetTimestamp() != FIT_DATE_TIME_INVALID)
      {
         fprintf(fileDescriptor, "timestamp,%d,,", mesg.GetTimestamp());
      }
      if ((mesg.GetNumFilteredBpm() != FIT_UINT8_INVALID) &&
         (mesg.GetNumFilteredBpm() > 0))
      {
         fprintf(fileDescriptor, "filtered_bpm,");
         count = mesg.GetNumFilteredBpm();
         for (FIT_UINT8 i = 0; i < count; i++)
         {
            fprintf(fileDescriptor, "%d", mesg.GetFilteredBpm(i));
            if (i < count - 1)
            {
               fprintf(fileDescriptor, "|");
            }
         }
         fprintf(fileDescriptor, ",bpm,");
      }
      if ((mesg.GetNumEventTimestamp12() != FIT_UINT8_INVALID) &&
         (mesg.GetNumEventTimestamp12() > 0))
      {
         fprintf(fileDescriptor, "event_timestamp_12,");
         count = mesg.GetNumEventTimestamp12();
         for (FIT_UINT8 i = 0; i < count; i++)
         {
            fprintf(fileDescriptor, "%d", mesg.GetEventTimestamp12(i));
            if (i < count - 1)
            {
               fprintf(fileDescriptor, "|");
            }
         }
         fprintf(fileDescriptor, ",,");
      }
      if ((mesg.GetNumEventTimestamp() != FIT_UINT8_INVALID) &&
         (mesg.GetNumEventTimestamp() > 0))
      {
         fprintf(fileDescriptor, "event_timestamp,");
         count = mesg.GetNumEventTimestamp();
         for (FIT_UINT8 i = 0; i < count; i++)
         {
            fprintf(fileDescriptor, "%f", mesg.GetEventTimestamp(i));
            if (i < count - 1)
            {
               fprintf(fileDescriptor, "|");
            }
         }
         fprintf(fileDescriptor, ",s,");
      }
      if (memcmp(&fractionalTs, fit::baseTypeInvalids[float32Num], sizeof(FIT_FLOAT32)) != 0)
      {
          fprintf(fileDescriptor, "fractional_timestamp,%f,s,", fractionalTs);
      }
      fprintf(fileDescriptor, "\n");
   }

   void SetOutputFileName( std::string file )
   {
      outputfile = file;
   }

   void OpenFile()
   {
      if (fileDescriptor == NULL)
      {
#pragma warning ( suppress : 4996 ) // warning C4996: 'fopen': This function or variable may be unsafe. Consider using fopen_s instead.
         fileDescriptor = fopen(outputfile.c_str(), "w");
         //Write the csv header out
         fprintf(fileDescriptor, "Type,Local Number,Message,Field 1,Value 1,Units 1,Field 2,Value 2,Units 2,Field 3,Value 3,Units 3,Field 4,Value 4,Units 4,Field 5,Value 5,Units 5,Field 6,Value 6,Units 6\n");
      }
   }

   void CloseFile()
   {
      fclose(fileDescriptor);
      fileDescriptor = NULL;
   }

   Listener()
   {
      fileDescriptor = NULL;
   }

private:
   const FIT_UINT8 float32Num = FIT_BASE_TYPE_FLOAT32 & FIT_BASE_TYPE_NUM_MASK;

   std::string outputfile;
   FILE *fileDescriptor;
};

int main(int argc, char* argv[])
{
   fit::Decode decode;
   fit::HRToRecordMesgBroadcastPlugin recordConvert;
   fit::BufferedMesgBroadcaster mesgBroadcaster;
   Listener listener;
   std::fstream file;
   std::string outputFile;

   if (argc != 2)
   {
      printf("Usage: HRToRecordMessageExample.exe <input file>.fit\n");
      return -1;
   }

   file.open(argv[1], std::ios::in | std::ios::binary);

   if (!file.is_open())
   {
      printf("Error opening file %s\n", argv[1]);
      return -1;
   }

   std::string fitFile = argv[1];
   int index = fitFile.find_last_of(".");
   outputFile = fitFile.substr(0, index) + ".csv";

   //Set the output file for the listener
   listener.SetOutputFileName(outputFile);
   listener.OpenFile();

   if (!decode.CheckIntegrity(file))
   {
      printf("FIT file integrity failed.\nAttempting to decode...\n");
   }
   mesgBroadcaster.RegisterMesgBroadcastPlugin(&recordConvert);
   mesgBroadcaster.AddListener((fit::HrMesgListener &)listener);
   mesgBroadcaster.AddListener((fit::RecordMesgListener &)listener);
   //mesgBroadcaster.AddListener((fit::MesgListener &)listener);

   try
   {
      mesgBroadcaster.Run(file);
      mesgBroadcaster.Broadcast();
   }
   catch (const fit::RuntimeException& e)
   {
      listener.CloseFile();
      printf("Exception decoding file: %s\n", e.what());
      return -1;
   }

   listener.CloseFile();
   return 0;
}