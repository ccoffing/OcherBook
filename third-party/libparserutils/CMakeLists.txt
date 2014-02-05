project(parserutils)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/libparserutils/include
	${CMAKE_CURRENT_SOURCE_DIR}/libparserutils/src
	)
set(PARSERUTILS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libparserutils/src PARENT_SCOPE)
set(PARSERUTILS_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libparserutils/include PARENT_SCOPE)

list(APPEND SOURCES
	libparserutils/src/utils/vector.c
	libparserutils/src/utils/buffer.c
	libparserutils/src/utils/stack.c
	libparserutils/src/utils/errors.c
	libparserutils/src/input/inputstream.c
	libparserutils/src/input/filter.c
	libparserutils/src/charset/codec.c
	libparserutils/src/charset/encodings/utf8.c
	libparserutils/src/charset/encodings/utf16.c
	libparserutils/src/charset/aliases.c
	libparserutils/src/charset/codecs/codec_utf8.c
	libparserutils/src/charset/codecs/codec_utf16.c
	libparserutils/src/charset/codecs/codec_8859.c
	libparserutils/src/charset/codecs/codec_ascii.c
	libparserutils/src/charset/codecs/codec_ext8.c
	)

add_library(parserutils STATIC
	${SOURCES})

