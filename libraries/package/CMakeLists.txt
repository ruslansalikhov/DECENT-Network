
add_subdirectory( ipfs )

file( GLOB HEADERS "*.hpp" "include/decent/package/*.hpp" )

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-3")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -gdwarf-3")


add_library( package_manager
             package.cpp
             detail.cpp
             #torrent_transfer.cpp
             ${HEADERS} local.cpp local.hpp)

add_dependencies(package_manager ipfs-transfer)

target_link_libraries( package_manager
        fc
        decent_encrypt
        nlohmann_json
        graphene_utilities
        ipfs-transfer
)

target_include_directories( package_manager
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
#                            PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../contrib/cpp-ipfs-api/include"
#                            PRIVATE "${CMAKE_SOURCE_DIR}/libraries/contrib/ipfs-cache/include"
)

# link_directories("${CMAKE_SOURCE_DIR}/libraries/contrib/ipfs-cache/lib"  )

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fobjc-link-runtime -framework Security")

# A test/sanbox dev-only executable.
add_executable( package_manager_sandbox sandbox.cpp ${HEADERS} )

if( WIN32 )
  target_link_libraries( package_manager_sandbox package_manager fc decent_encrypt nlohmann_json graphene_utilities ${GMP_LIBRARIES} )
else()
  target_link_libraries( package_manager_sandbox package_manager fc decent_encrypt nlohmann_json graphene_utilities gmp )
endif()

#install( TARGETS
#   package_manager
#
#   RUNTIME DESTINATION bin
#   LIBRARY DESTINATION lib
#   ARCHIVE DESTINATION lib
#)
