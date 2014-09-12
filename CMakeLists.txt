# TODO
# features requires/conflicts
# 3rd party external builds
# help
# cross compile
# consume config file


cmake_minimum_required(VERSION 2.8.12)

#
# Global compiler flags
#
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()
message("build type ${CMAKE_BUILD_TYPE}")
set(_CFLAGS_WARN "-Wall -Wextra -Wno-unused-parameter -Wwrite-strings -Wformat=2")
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG   "-O0 -DDEBUG")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${_CFLAGS_WARN}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_CFLAGS_WARN} -fno-rtti")
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${_CFLAGS_DBG}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_CFLAGS_DBG}")
endif()

#
# Global paths
#
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_DIR "${ROOT_DIR}/third-party")

#
# Global variables
#

#
# Features
#
# Features turned on by the user are TRUE.
# Features turned off by the user are FALSE.
# Features implicitly turned on are 1.
# Features implicitly turned off are 0.
#

include(${ROOT_DIR}/cmake/config.cmake)
#include(${ROOT_DIR}/cmake/features/xxx)

if(NOT CMAKE_CROSSCOMPILING)
enable_testing()
endif()

add_subdirectory(third-party/mxml)
add_subdirectory(third-party/zlib)
add_subdirectory(third-party/Signals)
add_subdirectory(third-party/libev)
add_subdirectory(third-party/freetype)
add_subdirectory(third-party/libwapcaplet)
add_subdirectory(third-party/libparserutils)
add_subdirectory(third-party/libcss)
add_subdirectory(third-party/libhubbub)

add_subdirectory(clc)
add_subdirectory(ocher)
