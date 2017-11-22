#ifndef DECENT_VIDEO_INFO_H
#define DECENT_VIDEO_INFO_H

#include <string>

// class for using FFmpeg library

class VideoInfo
{
public:
   VideoInfo();
   ~VideoInfo();

   void open(const std::string& filename);

   //get information...

private:

};

int getAVInfo(const std::string& filename);




#endif //DECENT_VIDEO_INFO_H
