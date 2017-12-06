cmake_minimum_required( VERSION 2.8.12 FATAL_ERROR )

project( video_info )

find_package( FFmpeg REQUIRED)
find_package( FreeImage REQUIRED)

# Find "JSON for Modern C++" (nlohmann/json.hpp)
find_path(JSON_FOR_MODERN_CXX_INCLUDE_DIR NAMES nlohmann/json.hpp)
mark_as_advanced(JSON_FOR_MODERN_CXX_INCLUDE_DIR)
if(NOT JSON_FOR_MODERN_CXX_INCLUDE_DIR)
    message(FATAL_ERROR "JSON for Modern C++ (nlohmann/json.hpp) not found. Go fetch it from https://github.com/nlohmann/json/blob/develop/src/json.hpp and set tell CMake where it is: 'cmake -DJSON_FOR_MODERN_CXX_INCLUDE_DIR:PATH=/path/to/include' (assuming the file is in /path/to/include/nlohmann/json.hpp)")
endif()
include_directories(${JSON_FOR_MODERN_CXX_INCLUDE_DIR})



set(CMAKE_VERBOSE_MAKEFILE OFF)

file( GLOB HEADERS "*.hpp" "include/decent/video/*.hpp" )

set(VIDEO_INFO_FILES
        video_info.cpp
        ${HEADERS}
)

add_library(video_info
        ${VIDEO_INFO_FILES}
)

target_include_directories(video_info PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${FFMPEG_INCLUDE_DIR}
        ${FREEIMAGE_INCLUDE_DIRS}
        nlohmann_json
        )

target_include_directories( video_info
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
        )


target_link_libraries(video_info PUBLIC
          ${FFMPEG_LIBRARIES}
          ${FREEIMAGE_LIBRARIES}
          fc
          ${CMAKE_DL_LIBS}
          ${PLATFORM_SPECIFIC_LIBS}
          )


