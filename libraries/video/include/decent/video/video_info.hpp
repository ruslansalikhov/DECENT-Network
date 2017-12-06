#ifndef DECENT_VIDEO_INFO_H
#define DECENT_VIDEO_INFO_H

#include <string>

/**
 * extracts informations from video file
 * @param filename - filename to get info from
 * @param output_info - output informations in json string
 * @return 0 on success otherwise error
 */
int getAVInfo(const std::string& filename, std::string& output_info);

/**
 * generates thumbnails from video into given folder
 * @param filename - filename of video
 * @param size_width - resulting size of thumbnails, zero means the same as video
 * @param size_height - resulting size of thumbnails, zero means the same as video
 * @param dir_name - output folder
 * @return number of generated thumbnails or negative number on error
 *         also throws an exception on error.
 */
int generate_thumbnails(const std::string& filename, int size_width, int size_height, int time_interval, int number_of_images, const std::string& dir_name);


#endif //DECENT_VIDEO_INFO_H
