cmake_minimum_required( VERSION 2.8.12 FATAL_ERROR )

project( decentgo_uploader )

find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Svg REQUIRED )

find_package( FFmpeg REQUIRED)

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(MACOSX_BUNDLE_ICON_FILE desktop)
set(MACOSX_BUNDLE_BUNDLE_NAME DECENT)

set(GUI_WALLET_ICON ${CMAKE_CURRENT_SOURCE_DIR}/desktop.icns)
set_source_files_properties(${GUI_WALLET_ICON} PROPERTIES
       MACOSX_PACKAGE_LOCATION "Resources")

set(DECENTGO_UPLOADER_FILES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
        src/wallet_wrapper.cpp
        src/wallet_wrapper.h
        src/video_info.cpp
        src/video_info.h
)

add_executable(decentgo_uploader MACOSX_BUNDLE
        ${DECENTGO_UPLOADER_FILES}
)

target_include_directories(decentgo_uploader PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        graphene_utilities
        graphene_wallet
        graphene_app
        ${FFMPEG_INCLUDE_DIR}
        )

target_link_libraries(decentgo_uploader PUBLIC
          Qt5::Core Qt5::Widgets Qt5::Svg
          graphene_utilities
          graphene_wallet
          graphene_app
          graphene_chain
          ${FFMPEG_LIBRARIES}

#          graphene_app
#          graphene_net
#          graphene_chain
#          graphene_utilities
#          graphene_wallet
#          wallet_utility
#          decent_messaging
#          package_manager
#          decent_encrypt
#          pbc
#          gmp
#          graphene_account_history
#          decent_seeding
#          graphene_miner
#          graphene_debug_miner
#          graphene_egenesis_decent
          fc
          ${CMAKE_DL_LIBS}
          ${PLATFORM_SPECIFIC_LIBS}
          )

if( WIN32 )
  target_link_libraries(decentgo_uploader PUBLIC update msi)

else()
#  if( UPDATE_MANAGER )
#    target_link_libraries(decentgo_uploader PUBLIC update)
#  endif()
endif()

# temporary off
# set_target_properties(DECENT PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "DECENT")
# set_target_properties(DECENT PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in")
#

#
#install( TARGETS
#   DECENT
#
#   RUNTIME DESTINATION bin
#   BUNDLE DESTINATION bin
#   LIBRARY DESTINATION lib
#   ARCHIVE DESTINATION lib
#)
#

