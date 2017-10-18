cmake_minimum_required (VERSION 3.5)
set(GLOB BOOST_VERSION 1.55)
include(ExternalProject)

set(CMAKE_VERBOSE_MAKEFILE ON)

# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-3")
# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -gdwarf-3")

################################################################################
# TODO(peterj): Address sanitizer seems to cause segmentation faults on exit
#               from main(?).
#set(SANITIZE "-fsanitize=address")
################################################################################

set(BINDING_DIR "${CMAKE_BINARY_DIR}/ipfs_bindings")

project(ipfs-binding)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wall -ggdb ${SANITIZE}")

add_custom_command(
        OUTPUT ${BINDING_DIR}/ipfs_bindings.h
               ${BINDING_DIR}/ipfs_bindings.a
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ipfs_bindings.go
        COMMAND mkdir -p ${BINDING_DIR}
                && cd ${BINDING_DIR}
                && go build --buildmode=c-archive ${CMAKE_CURRENT_SOURCE_DIR}/ipfs_bindings.go)

add_library(ipfs-binding STATIC IMPORTED)
set_target_properties(ipfs-binding PROPERTIES
        IMPORTED_LOCATION "${BINDING_DIR}/ipfs_bindings.a"
        INTERFACE_INCLUDE_DIRECTORIES "${BINDING_DIR}"
        )

################################################################################
project(ipfs-wrapper)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -Wall -fobjc-link-runtime -framework Security  -ggdb ${SANITIZE}")

include_directories(
    "${BINDING_DIR}"
    "${CMAKE_SOURCE_DIR}/include"
)

set(IPFS_WRAPPER_FILES
    ipfs_wrapper.cpp
    ipfs_wrapper.hpp
)

add_library(ipfs-wrapper STATIC
    ${IPFS_WRAPPER_FILES}
    ${BINDING_DIR}/ipfs_bindings.h
)

add_dependencies(ipfs-wrapper ipfs-binding nlohmann_json)

target_include_directories(ipfs-wrapper PRIVATE
        ipfs-binding)

target_include_directories(ipfs-wrapper PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(ipfs-wrapper PUBLIC
        ipfs-binding
        nlohmann_json
        graphene_utilities
)

target_link_libraries(ipfs-wrapper INTERFACE "-fobjc-link-runtime -framework Security")

################################################################################
project(ipfs-transfer)

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fobjc-link-runtime -framework Security")

include_directories(
        "${BINDING_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_SOURCE_DIR}/libraries/encrypt/include"
)

add_library(ipfs-transfer STATIC
        ipfs_transfer.cpp
        ipfs_transfer.hpp
)

add_dependencies(ipfs-transfer ipfs-wrapper)

target_link_libraries(ipfs-transfer ipfs-wrapper)


###############################################################################
#project(test_client)
# -framework CoreFoundation  -fobjc-link-runtime
# -mmacosx-version-min=10.10

#
#find_package(Boost ${BOOST_VERSION} COMPONENTS program_options REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -fobjc-link-runtime -framework Security ${SANITIZE}")
#
#include_directories(
#    "${BINDING_DIR}"
#    "${CMAKE_SOURCE_DIR}/include")
#
#file(GLOB sources
#  "${CMAKE_SOURCE_DIR}/test/main.cpp")
#
#add_custom_command(
#        OUTPUT ${BINDING_DIR}/ipfs_bindings.h
#        ${BINDING_DIR}/ipfs_bindings.a
#        DEPENDS ${CMAKE_SOURCE_DIR}/src/ipfs_bindings.go
#        COMMAND mkdir -p ${BINDING_DIR}
#        && cd ${BINDING_DIR}
#        && go build --buildmode=c-archive ${CMAKE_SOURCE_DIR}/src/ipfs_bindings.go)
#
# add_executable(test_client ${sources}
#                ${BINDING_DIR}/ipfs_bindings.h)
#
#target_link_libraries(test_client
#    event
#    event_pthreads
#    gmp
#    ${BINDING_DIR}/ipfs_bindings.a
#    ${Boost_LIBRARIES})
#
################################################################################
