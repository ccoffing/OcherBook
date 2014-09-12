cmake_minimum_required(VERSION 2.8.12)

project(clc)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	)

set(CLC_MAJOR_VERSION 0)
set(CLC_MINOR_VERSION 1)
set(CLC_PATCH_VERSION 0)
set(CLC_VERSION ${CLC_MAJOR_VERSION}.${CLC_MINOR_VERSION}.${CLC_PATCH_VERSION})


list(APPEND CLC_SOURCES
	clc/algorithm/Random.cpp
	clc/crypto/MurmurHash2.cpp
	clc/data/Buffer.cpp
	clc/data/Hashtable.cpp
	clc/data/List.cpp
	clc/data/Set.cpp
	clc/data/StrUtil.cpp
	clc/os/Clock.cpp
	clc/os/Lock.cpp
	clc/os/RWLock.cpp
	clc/os/Monitor.cpp
	clc/os/Thread.cpp
	clc/storage/DirIter.cpp
	clc/storage/File.cpp
	clc/storage/Path.cpp
	clc/support/Debug.cpp
	clc/support/Logger.cpp
	)

add_library(clc STATIC
	${CLC_SOURCES})
target_link_libraries(clc pthread)

export(PACKAGE clc)


add_subdirectory(test)
