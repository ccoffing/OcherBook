project(hubbub)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/libhubbub/include
	${CMAKE_CURRENT_SOURCE_DIR}/libhubbub/src
	${PARSERUTILS_INC_DIR}
	)
set(HUBBUB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libhubbub/src PARENT_SCOPE)
set(HUBBUB_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libhubbub/include PARENT_SCOPE)

list(APPEND SOURCES
	libhubbub/src/charset/detect.c
	libhubbub/src/tokeniser/tokeniser.c
	libhubbub/src/tokeniser/entities.c
	libhubbub/src/parser.c
	libhubbub/src/utils/string.c
	libhubbub/src/utils/errors.c
	libhubbub/src/treebuilder/in_caption.c
	libhubbub/src/treebuilder/in_table.c
	libhubbub/src/treebuilder/in_foreign_content.c
	libhubbub/src/treebuilder/treebuilder.c
	libhubbub/src/treebuilder/after_after_body.c
	libhubbub/src/treebuilder/generic_rcdata.c
	libhubbub/src/treebuilder/in_select.c
	libhubbub/src/treebuilder/after_frameset.c
	libhubbub/src/treebuilder/in_column_group.c
	libhubbub/src/treebuilder/in_head.c
	libhubbub/src/treebuilder/in_table_body.c
	libhubbub/src/treebuilder/in_body.c
	libhubbub/src/treebuilder/in_frameset.c
	libhubbub/src/treebuilder/after_body.c
	libhubbub/src/treebuilder/after_head.c
	libhubbub/src/treebuilder/initial.c
	libhubbub/src/treebuilder/in_head_noscript.c
	libhubbub/src/treebuilder/after_after_frameset.c
	libhubbub/src/treebuilder/in_row.c
	libhubbub/src/treebuilder/before_html.c
	libhubbub/src/treebuilder/in_cell.c
	libhubbub/src/treebuilder/before_head.c
	libhubbub/src/treebuilder/in_select_in_table.c
	)

add_library(hubbub STATIC
	${SOURCES})

