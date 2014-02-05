project(wapcaplet)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/libwapcaplet/include
	${CMAKE_CURRENT_SOURCE_DIR}/libwapcaplet/src
	)
set(WAPCAPLET_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libwapcaplet/src PARENT_SCOPE)
set(WAPCAPLET_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libwapcaplet/include PARENT_SCOPE)

list(APPEND SOURCES
	libwapcaplet/src/libwapcaplet.c
	)

add_library(wapcaplet STATIC
	${SOURCES})

