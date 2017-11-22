
#include "video_info.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

VideoInfo::VideoInfo()
{
}

VideoInfo::~VideoInfo()
{
}

void VideoInfo::open(const std::string& filename)
{
}

int getAVInfo(const std::string& filename)
{
   AVFormatContext *pFormatCtx = NULL;
   int             i, videoStream;

   // Register all formats and codecs
   av_register_all();

   // Open video file
   if (avformat_open_input(&pFormatCtx, filename.c_str(), NULL, NULL) != 0)
      return -1; // Couldn't open file

   // Retrieve stream information
   if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
      return -1; // Couldn't find stream information

   // Dump information about file onto standard error
   av_dump_format(pFormatCtx, 0, filename.c_str(), 0);

   // Find the first video stream
   videoStream=-1;
   for(i=0; i<pFormatCtx->nb_streams; i++)
      if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
         videoStream=i;
         break;
      }
   if(videoStream==-1)
      return -1; // Didn't find a video stream



   //TODO:





   // Close the video file
   avformat_close_input(&pFormatCtx);

   return 0;
}