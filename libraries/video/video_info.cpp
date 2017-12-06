
#include <decent/video/video_info.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libswscale/swscale.h>
}

#include <FreeImage.h>
#include <cstdlib>

#include "json.hpp"


static int64_t get_duration_in_sec(AVFormatContext* ic)
{
   if (ic->duration != AV_NOPTS_VALUE) {
      int secs;

      int64_t duration = ic->duration + (ic->duration <= INT64_MAX - 5000 ? 5000 : 0);
      return (duration / (int64_t) AV_TIME_BASE);
   }

   return -1;
}

static std::string get_duration(AVFormatContext* ic)
{

   if (ic->duration != AV_NOPTS_VALUE) {
      int hours, mins, secs, us;
      int64_t duration = ic->duration + (ic->duration <= INT64_MAX - 5000 ? 5000 : 0);
      secs  = duration / (int64_t)AV_TIME_BASE;
      us    = duration % (int64_t)AV_TIME_BASE;
      mins  = secs / 60;
      secs %= 60;
      hours = mins / 60;
      mins %= 60;

      char buffer[256];
      snprintf(buffer, 256, "%02d:%02d:%02d.%02d", hours, mins, secs,
               (100 * us) / AV_TIME_BASE);

      return std::string(buffer);
   }

   return std::string("N/A");
}

int getAVInfo(const std::string& filename, std::string& output_info)
{
   AVFormatContext *pFormatCtx = NULL;
   int             i, videoStream, ret;

   // Register all formats and codecs
   av_register_all();

   // Open video file
   if (avformat_open_input(&pFormatCtx, filename.c_str(), NULL, NULL) != 0)
      return -1; // Couldn't open file

   // Retrieve stream information
   if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
      return -1; // Couldn't find stream information

   nlohmann::json root;
   nlohmann::json video_info = nlohmann::json::array();
   nlohmann::json audio_info = nlohmann::json::array();
   nlohmann::json subtitle_info = nlohmann::json::array();

//   // Dump information about file onto standard error
//   av_dump_format(pFormatCtx, 0, filename.c_str(), 0);

   // Find the first video stream
   videoStream=-1;
   for(i=0; i<pFormatCtx->nb_streams; i++) {
      AVStream* st = pFormatCtx->streams[i];

      AVDictionaryEntry *lang = av_dict_get(st->metadata, "language", NULL, 0);
      if (lang && strcmp(lang->value, "und") == 0) {
         lang = nullptr;
      }

      const char* codec_name = avcodec_get_name(st->codecpar->codec_id);
      const char* profile_name = avcodec_profile_name(st->codecpar->codec_id, st->codecpar->profile);

      if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
         videoStream++;

         nlohmann::json videoJson;
         videoJson["resolution"] = std::to_string(st->codecpar->width) + "x" + std::to_string(st->codecpar->height);

         std::string codec_name_text(codec_name);
         if (profile_name) {
            codec_name_text += "(" + std::string(profile_name) + ")";
         }
         videoJson["codec"] = codec_name_text;


         AVPixelFormat format = (AVPixelFormat)st->codecpar->format;
         const char* format_name = av_get_pix_fmt_name(format);
         if (format_name) {
            videoJson["format"] = format_name;
         }


         videoJson["bitrate"] = std::to_string(st->codecpar->bit_rate / 1000) + " kb/s";

         videoJson["duration"] = get_duration(pFormatCtx);

         // st->duration  (int64)

         video_info.push_back(videoJson);
      }
      else if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {

         nlohmann::json audioJson;
         const char* format_name = av_get_sample_fmt_name((AVSampleFormat) st->codecpar->format);
         if (format_name) {
            audioJson["format"] = format_name;
         }

         std::string codec_name_text(codec_name);
         if (profile_name) {
            codec_name_text += "(" + std::string(profile_name) + ")";
         }
         audioJson["codec"] = codec_name_text;

         audioJson["channels"] = st->codecpar->channels;
         audioJson["sample_rate"] = st->codecpar->sample_rate;

         if (lang) {
            audioJson["lang"] = lang->value;
         }

         audioJson["bitrate"] = std::to_string(st->codecpar->bit_rate / 1000) + " kb/s";

         audio_info.push_back(audioJson);
      }
      else if (st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE) {
         //lang ??

         if (lang) {
            nlohmann::json subtitleJson;
            subtitleJson["lang"] = lang->value;

            subtitle_info.push_back(subtitleJson);
         }
      }

   }

   root["video"] = video_info;
   root["audio"] = audio_info;
   if (!subtitle_info.empty()) {
      root["subtitle"] = subtitle_info;
   }

   if(videoStream == -1)
      return -1; // Didn't find a video stream

   output_info = root.dump();

   // Close the video file & free context
   avformat_close_input(&pFormatCtx);

   return 0;
}

static int find_video_stream(AVFormatContext* pFormatCtx)
{
   for(int i=0; i<pFormatCtx->nb_streams; i++) {
      AVStream *st = pFormatCtx->streams[i];

      if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
         return i;
      }
   }

   return -1;
}

static int ffmpeg_decode_packet(AVCodecContext *avctx, AVFrame *frame, int *got_frame, AVPacket *pkt)
{
   int ret;

   *got_frame = 0;

   if (pkt) {
      ret = avcodec_send_packet(avctx, pkt);
      // In particular, we don't expect AVERROR(EAGAIN), because we read all
      // decoded frames with avcodec_receive_frame() until done.
      if (ret < 0)
         return ret == AVERROR_EOF ? 0 : ret;
   }

   ret = avcodec_receive_frame(avctx, frame);
   if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
      return ret;
   if (ret >= 0)
      *got_frame = 1;

   return 0;
}

int initialize_scale_context(AVCodecContext* pCodecCtx, struct SwsContext** sws_ctx, int width, int height)
{
   if (width >= 0 || height >= 0) {
      width  = pCodecCtx->width;
      height = pCodecCtx->height;
   }

   // initialize SWS context for software scaling
   *sws_ctx = sws_getContext(pCodecCtx->width,
                             pCodecCtx->height,
                             pCodecCtx->pix_fmt,
                             width,
                             height,
                             AV_PIX_FMT_BGR24,
                             SWS_BILINEAR,
                             NULL,
                             NULL,
                             NULL);
   if (*sws_ctx == NULL) {
      return -1;
   }

   return 0;
}

void finalize_scale_context(struct SwsContext** sws_ctx)
{
   sws_freeContext(*sws_ctx);
   sws_ctx = NULL;
}

//  AVCodecContext *pCodecCtx
int save_frame_as_jpeg(AVCodecContext* pCodecCtx, AVFrame *pFrame, struct SwsContext* sws_ctx, int dst_width, int dst_height, const char* filename)
{
   int ret;
   uint8_t *video_dst_data[4] = {NULL};
   int      video_dst_linesize[4];
   int      video_dst_bufsize;

   if (dst_width == 0 || dst_height == 0) {
      dst_width  = pFrame->width;
      dst_height = pFrame->height;
   }

   ret = av_image_alloc(video_dst_data, video_dst_linesize,
                        dst_width, dst_height,
                        AV_PIX_FMT_RGB24, 1);
   if (ret < 0)
      return -1;

   video_dst_bufsize = ret;

   // Convert the image from its native format to RGB
   ret = sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                   pFrame->linesize, 0, pCodecCtx->height,
                   video_dst_data, video_dst_linesize);

   FIBITMAP* fib = FreeImage_AllocateT(FIT_BITMAP, dst_width, dst_height, 24,
                                       0x00ff0000,
                                       0x0000ff00,
                                       0x000000ff);

   unsigned char* src_data;
   unsigned int uiLineSize = FreeImage_GetLine(fib);

   //copy image to destination and flip it up side down
   uint8_t* buffer = FreeImage_GetBits(fib);
   for(int y = 0; y < dst_height; y++) {

      src_data = video_dst_data[0] + ((dst_height - y) * uiLineSize);
      memcpy(buffer, src_data, uiLineSize);

      src_data += uiLineSize;
      buffer   += uiLineSize;
   }

   ret = 0;
   if (!FreeImage_Save(FIF_JPEG, fib, filename, JPEG_DEFAULT)) {
      ret = -1;
   }

   FreeImage_Unload(fib);
   av_free(video_dst_data[0]);

   return ret;
}

int get_frame_image_for_time(AVFormatContext *pFormatCtx, int streamIndex, int64_t seekTimePos)
{
   int ret;
   int64_t seek_target = seekTimePos; //av_rescale_q(seekTimePos, AV_TIME_BASE_Q,
                                      // pFormatCtx->streams[streamIndex]->time_base);

   return av_seek_frame(pFormatCtx, streamIndex, seek_target, AVSEEK_FLAG_ANY);
}

int generate_thumbnails(const std::string& filename, int size_width, int size_height, int time_interval, int number_of_images, const std::string& dir_name)
{
   if ((time_interval == 0 && number_of_images == 0) ||
       (time_interval > 0 && number_of_images > 0)) {
      return -1;
   }

   AVFormatContext *pFormatCtx = NULL;
   int             i, videoStream, ret;
   AVCodecContext* pCodecCtx = NULL;
   struct SwsContext *sws_ctx = NULL;
   AVFrame* frame = NULL;
   std::string error_text;
   int frame_index = 0;

   do //exception emulation
   {
      /* register codecs and formats and other lavf/lavc components*/
      av_register_all();

      // Open video file
      if (avformat_open_input(&pFormatCtx, filename.c_str(), NULL, NULL) != 0) {
         error_text = "avformat_open_input - Couldn't open file";
         break;
      }

      // Retrieve stream information
      if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
         error_text = "avformat_find_stream_info - Couldn't find stream information";
         break;
      }

      int64_t durration = get_duration_in_sec(pFormatCtx);
      if (durration == 0) {
         error_text = "get_duration_in_sec - duration too short";
         break;
      }

      int64_t myTimeInterval = 0;

      if (time_interval == 0 && number_of_images > 0) {
         myTimeInterval = durration / number_of_images;
      }
      else if (number_of_images == 0 && time_interval > 0) {
         myTimeInterval = time_interval;
      }

      int streamIndex = find_video_stream(pFormatCtx);
      if(streamIndex == -1) {
         error_text = "find_video_stream - video stream not found";
         break;
      }

      AVCodec *pCodec = NULL;

      // Find the decoder for the video stream
      AVStream* st = pFormatCtx->streams[streamIndex];
      pCodec = avcodec_find_decoder(st->codecpar->codec_id);
      if (pCodec == NULL) {
         error_text = "avcodec_find_decoder - Codec not found";
         break;
      }

      // Copy context
      pCodecCtx = avcodec_alloc_context3(pCodec);
      if (avcodec_parameters_to_context(pCodecCtx, st->codecpar) !=0) {
         error_text = "avcodec_parameters - Couldn't copy codec context";
         break;
      }

      AVDictionary* options = NULL;

      // Open codec
      if (avcodec_open2(pCodecCtx, pCodec, &options) < 0) {
         error_text = "avcodec_open - Could not open codec";
         break;
      }

      if (initialize_scale_context(pCodecCtx, &sws_ctx, size_width, size_height) < 0) {
         error_text = "initialize_scale_context failed!";
         break;
      }

      frame = av_frame_alloc();
      if(!frame) {
         error_text = "av_frame_alloc failed!";
         break;
      }

      AVPacket data_packet;
      av_init_packet(&data_packet);
      data_packet.data = NULL;
      data_packet.size = 0;

      int64_t next_time_interval = 0;

      int got_packet;
      while ((ret = av_read_frame(pFormatCtx, &data_packet)) >= 0) {

         if (data_packet.stream_index != streamIndex)
            continue;

         //frame time in AV_TIME_BASE units
         int64_t cur_frame_time = av_rescale_q(data_packet.pts, st->time_base, AV_TIME_BASE_Q);

         ret = ffmpeg_decode_packet(pCodecCtx, frame, &got_packet, &data_packet);

         if (got_packet && cur_frame_time >= (next_time_interval * (int64_t)AV_TIME_BASE)) {

            char buffer[64];
            sprintf(buffer, "thumbnail_%04d.jpg", frame_index);

            std::string out_filename = dir_name + std::string(buffer);
            ret = save_frame_as_jpeg(pCodecCtx, frame, sws_ctx, size_width, size_height, out_filename.c_str());

            frame_index++;
            next_time_interval += myTimeInterval;
         }
      }

      if (ret < 0 && ret != AVERROR_EOF) {
         char buffer[512];
         av_make_error_string(buffer, sizeof(buffer), ret);

         error_text = std::string("av_read_frame error: ") + buffer;
      }

      av_packet_unref(&data_packet);

   } while(false);

   if (frame)
      av_frame_free(&frame);

   if (sws_ctx)
      finalize_scale_context(&sws_ctx);

   if (pCodecCtx)
      avcodec_close(pCodecCtx);

   if (pFormatCtx) {
      // Close the video file & free context
      avformat_close_input(&pFormatCtx);
   }

   if(!error_text.empty()) {
      throw std::runtime_error(error_text);
   }

   return frame_index;
}



