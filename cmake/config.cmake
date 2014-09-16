# function config_requires(FEATURE FEATURE1 ...)
# function config_conflicts(FEATURE FEATURE1 ...)

set(OCHER_TARGET sdl CACHE STRING "Target flavor: sdl cygwin minwg kobo haiku")

# TODO
set(OCHER_KOBO FALSE CACHE BOOL "Use Kobo?")
set(OCHER_SDL FALSE CACHE BOOL "Use SDL?")

### Build settings
set(OCHER_MAJOR, 0)
set(OCHER_MINOR, 1)
set(OCHER_PATCH, 0)

### Debugging
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(OCHER_AIRBAG_FD_DEF TRUE)
else()
	set(OCHER_AIRBAG_FD_DEF FALSE)
endif()
set(OCHER_AIRBAG_FD ${OCHER_AIRBAG_FD_DEF} CACHE BOOL "Use airbagfd crash handler (only for unix)")

### eBook formats
set(OCHER_FMT_EPUB TRUE CACHE BOOL "Support EPUB books")
set(OCHER_FMT_TEXT TRUE CACHE BOOL "Support plain text books")
set(OCHER_FMT_HTML TRUE CACHE BOOL "Support HTML books")
set(OCHER_FMT_JPEG TRUE CACHE BOOL "Support JPEG images")

### Output devices
set(OCHER_UI_FD TRUE CACHE BOOL "File descriptor user interface")
set(OCHER_UI_CDK FALSE CACHE BOOL "CDK (ncurses) based user interface")
##ifneq ($(OCHER_TARGET),cygwin)
##    OCHER_UI_SDL?=1
##endif
#OCHER_UI_CDK=1
#OCHER_UI_MX50?=0
#ifeq ($(OCHER_TARGET),kobo)
#    OCHER_UI_MX50=1
#    OCHER_UI_CDK=0
#    OCHER_UI_SDL=0
#endif
#
##OCHER_FREETYPE
##built-in fonts
##generic keyboard
