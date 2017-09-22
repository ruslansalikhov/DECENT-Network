cmake_minimum_required(VERSION 3.5)

project(ipfs-cache)

# Find libEvent
find_package(libevent2)


# include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(ipfs-cache STATIC IMPORTED)

set_property(TARGET ipfs-cache PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libipfs-cache.a" )
set_property(TARGET ipfs-cache PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include" )

# set_target_properties(ipfs-cache PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib" )  # libipfs-cache.a
# set_target_properties(ipfs-cache PROPERTIES INCLUDE_DIRECTORIES )
