
file( GLOB HEADERS "*.hpp" "include/decent/package/*.hpp" )

add_library( package_manager
             package.cpp
             detail.cpp
             #torrent_transfer.cpp
             ipfs_transfer.cpp
             ${HEADERS} local.cpp local.hpp)

target_link_libraries( package_manager fc decent_encrypt  nlohmann_json ipfs-api graphene_utilities )
target_include_directories( package_manager
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../contrib/cpp-ipfs-api/include")

# A test/sanbox dev-only executable.
add_executable( package_manager_sandbox sandbox.cpp ${HEADERS} )

target_link_libraries( package_manager_sandbox package_manager fc decent_encrypt nlohmann_json ipfs-api graphene_utilities)

if( WIN32 )
  target_link_libraries( package_manager_sandbox ${GMP_LIBRARIES} )
else()
  target_link_libraries( package_manager_sandbox gmp )
endif()

#install( TARGETS
#   package_manager
#
#   RUNTIME DESTINATION bin
#   LIBRARY DESTINATION lib
#   ARCHIVE DESTINATION lib
#)
