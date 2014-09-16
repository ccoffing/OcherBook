# TODO get_propery INCLUDE_DIRECTORIES of various third_party dirs

include_directories(
	${ROOT_DIR}
	${ROOT_DIR}/clc
	${ZLIB_INCLUDE_DIR}
	${LIBEV_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIR}
	${THIRD_PARTY_DIR}
	)
if(OCHER_KOBO)
	include_directories(${CMAKE_SOURCE_DIR}/linux)
endif()
if(OCHER_SDL OR OCHER_UI_CDK)
	include_directories(/usr/local/include)
	link_directories(/usr/local/lib)
endif()
if(OCHER_FMT_EPUB)
	include_directories(
		${HUBBUB_INC_DIR}
		${LIBCSS_INC_DIR}
		${WAPCAPLET_INC_DIR}
		${PARSERUTILS_INC_DIR}
		${MXML_INCLUDE_DIR}
		)
endif()

list(APPEND LIB_SOURCES
	#ux/fd/BrowseFd.cpp
	#ux/fd/FactoryFd.cpp
	#ux/fd/RendererFd.cpp
	Container.cpp
	device/Battery.cpp
	device/Device.cpp
	device/Filesystem.cpp
	fmt/Format.cpp
	fmt/Layout.cpp
	fmt/minizip/ioapi.c
	fmt/minizip/unzip.c
	resources/Bitmaps.cpp
	settings/Settings.cpp
	shelf/Meta.cpp
	shelf/Shelf.cpp
	ux/Activity.cpp
	ux/Controller.cpp
	ux/Event.cpp
	ux/Pagination.cpp
	ux/PowerSaver.cpp
	ux/Renderer.cpp
	ux/fb/BatteryIcon.cpp
	ux/fb/BootActivityFb.cpp
	ux/fb/ClockIcon.cpp
	ux/fb/FontEngine.cpp
	ux/fb/FrameBuffer.cpp
	ux/fb/FreeType.cpp
	ux/fb/HomeActivityFb.cpp
	ux/fb/LibraryActivityFb.cpp
	ux/fb/NavBar.cpp
	ux/fb/ReadActivityFb.cpp
	ux/fb/RendererFb.cpp
	ux/fb/RleBitmap.cpp
	ux/fb/SettingsActivityFb.cpp
	ux/fb/SleepActivityFb.cpp
	ux/fb/SyncActivityFb.cpp
	ux/fb/SystemBar.cpp
	ux/fb/UxControllerFb.cpp
	ux/fb/Widgets.cpp
	)
if(OCHER_UI_CDK)
	list(APPEND LIB_SOURCES
		ux/cdk/BrowseActivityCdk.cpp
		ux/cdk/RendererCdk.cpp
		ux/cdk/UxControllerCdk.cpp
		)
	list(APPEND OCHER_DEFS UX_CDK)
endif()
if(OCHER_KOBO)
	list(APPEND LIB_SOURCES
		ux/fb/mx50/FrameBufferMx50.cpp
		device/kobo/KoboEvents.cpp
		)
	list(APPEND OCHER_DEFS UX_FB UX_FB_MX50)
endif()
if(OCHER_SDL)
	list(APPEND LIB_SOURCES
		ux/fb/sdl/FrameBufferSdl.cpp
		ux/fb/sdl/SdlThread.cpp
		)
	list(APPEND OCHER_DEFS UX_FB UX_FB_SDL)
endif()
if(OCHER_UI_FD)
	list(APPEND OCHER_DEFS UX_FD)
endif()

if(OCHER_FMT_EPUB)
	list(APPEND LIB_SOURCES
		#fmt/epub/Css.cpp
		#fmt/epub/ParseHtml.c
		fmt/epub/LayoutEpub.cpp
		fmt/epub/UnzipCache.cpp
		fmt/epub/Epub.cpp
		)
	list(APPEND OCHER_DEFS OCHER_EPUB)
endif()
if(OCHER_FMT_TEXT)
	list(APPEND LIB_SOURCES
		fmt/text/Text.cpp
		fmt/text/LayoutText.cpp
		)
	list(APPEND OCHER_DEFS OCHER_TEXT)
endif()
if(OCHER_FMT_JPEG)
	list(APPEND LIB_SOURCES
		ux/fb/JPEG.cpp
		)
	list(APPEND OCHER_DEFS OCHER_JPEG)
endif()

add_library(ocherbook STATIC
	${LIB_SOURCES}
	)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	list(APPEND OCHER_DEFS CLC_LOG_LEVEL=5)
else()
	list(APPEND OCHER_DEFS CLC_LOG_LEVEL=2)
endif()

target_compile_definitions(ocherbook
	PRIVATE
		${OCHER_DEFS}
	)

target_link_libraries(ocherbook
	freetype
	libev
	zlib
	mxml
	clc
	pthread
	bz2  # TODO freetype (may!) want this
	)
if(OCHER_FMT_EPUB)
	target_link_libraries(ocherbook
		hubbub
		#css
		parserutils
		wapcaplet
		)
endif()
if(OCHER_SDL)
	target_link_libraries(ocherbook
		SDL
		)
endif()
if(OCHER_UI_CDK)
	target_link_libraries(ocherbook
		cdk
		)
endif()

add_executable(ocher
	ocher.cpp
	${BIN_SOURCES}
	)
target_link_libraries(ocher
	ocherbook
	)


include(ProcessorCount)
ProcessorCount(NUM_CPUS)
if(NOT NUM_CPUS EQUAL 0)
	set(CPPCHECK_FLAGS -j${NUM_CPUS})
endif()
get_target_property(OCHER_INCS ocherbook INCLUDE_DIRECTORIES)
foreach(INC ${OCHER_INCS})
	list(APPEND CPPCHECK_INCS "-I${INC}")
endforeach()
add_custom_target(cppcheck
	#COMMAND cppcheck --check-config $(INCS) ocher/ 2> $(BUILD_DIR)/cppcheck.log
	COMMAND cppcheck --max-configs=100 ${CPPCHECK_FLAGS} --enable=all ${CPPCHECK_INCS} ${CMAKE_CURRENT_SOURCE_DIR} 2> ${CMAKE_CURRENT_BINARY_DIR}/cppcheck.log
	)


configure_file(${PROJECT_SOURCE_DIR}/doc/Doxyfile
	${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

add_custom_target(doxygen
	COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)

add_subdirectory(test)
