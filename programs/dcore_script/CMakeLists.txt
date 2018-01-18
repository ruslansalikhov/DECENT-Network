cmake_minimum_required( VERSION 2.8.12 FATAL_ERROR )

project( dcore_script )

set(CMAKE_VERBOSE_MAKEFILE OFF)

set(DCORE_SCRIPT_FILES
        main.cpp
        cmd_interpret.cpp
        cmd_interpret.h
        script_func.cpp
        script_func.h
        json_path.cpp
        json_path.h
)

add_executable(dcore_script MACOSX_BUNDLE
        ${DCORE_SCRIPT_FILES}
)

target_include_directories(dcore_script PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        graphene_utilities
        graphene_wallet
        graphene_app
        )

target_link_libraries(dcore_script PUBLIC
          graphene_utilities
          graphene_wallet
          graphene_app
          graphene_chain
          ${FFMPEG_LIBRARIES}
          ${FREEIMAGE_LIBRARIES}

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
  target_link_libraries(dcore_script PUBLIC update msi)

endif()

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

