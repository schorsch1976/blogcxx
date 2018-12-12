############################################################################
# Objects embedded
############################################################################
OPTION(WITH_OEMBED "With objects embedded" OFF)
if (WITH_OEMBED)
	ADD_DEFINITIONS(-DWITH_OEMBED)

	find_package(CURL)
	IF(NOT ${CURL_FOUND})
		MESSAGE(FATAL_ERROR "CURL not Found: Set in in the ADVANCED SETTINGS")
	ENDIF(NOT ${CURL_FOUND})
	INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIRS})

	SET(SRCS ${SRCS}
		curl/HTTPDownloader.cpp
	)

	SET(LIBS ${LIBS}
		${CURL_LIBRARY}
	)
ENDIF(WITH_OEMBED)

############################################################################
# 3rdParty includes provided by blogcxx
############################################################################
SET(SRCS ${SRCS}
	XML/tinyxml2.cpp
	XML/tinyxml2.h
)

IF(MARKDOWN_CMARK)
	set(PROJECT_VERSION_MAJOR 0)
	set(PROJECT_VERSION_MINOR 28)
	set(PROJECT_VERSION_PATCH 3)
	set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )

	SET(CMARK_DIR cmark-0.28.3/src/)

	# adopted from the cmake file from cmark
	CONFIGURE_FILE(
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_DIR}/config.h.in
		${CMAKE_CURRENT_BINARY_DIR}/config.h)

	CONFIGURE_FILE(
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_DIR}/cmark_version.h.in
		${CMAKE_CURRENT_BINARY_DIR}/cmark_version.h)

	CONFIGURE_FILE(
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_DIR}/libcmark.pc.in
		${CMAKE_CURRENT_BINARY_DIR}/libcmark.pc @ONLY)


	INCLUDE_DIRECTORIES(${CMARK_DIR})
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

	SET(CMARK_SRCS ${CMARK_SRCS}
		${CMARK_DIR}cmark.c
		${CMARK_DIR}node.c
		${CMARK_DIR}iterator.c
		${CMARK_DIR}blocks.c
		${CMARK_DIR}inlines.c
		${CMARK_DIR}scanners.c
		${CMARK_DIR}scanners.re
		${CMARK_DIR}utf8.c
		${CMARK_DIR}buffer.c
		${CMARK_DIR}references.c
		${CMARK_DIR}render.c
		${CMARK_DIR}man.c
		${CMARK_DIR}xml.c
		${CMARK_DIR}html.c
		${CMARK_DIR}commonmark.c
		${CMARK_DIR}latex.c
		${CMARK_DIR}houdini_href_e.c
		${CMARK_DIR}houdini_html_e.c
		${CMARK_DIR}houdini_html_u.c
		${CMARK_DIR}cmark_ctype.c
	)

	include (GenerateExportHeader)
	ADD_LIBRARY(cmark ${CMARK_SRCS})
	generate_export_header(cmark)
	target_compile_definitions(cmark PRIVATE -Dtrue=1 -Dfalse=0)
	target_compile_definitions(cmark PRIVATE -DCMARK_STATIC_DEFINE)

	SET(CMARK_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_DIR}
		${CMAKE_CURRENT_BINARY_DIR}
	PARENT_SCOPE)
ENDIF(MARKDOWN_CMARK)

IF(MARKDOWN_CMARK_GFM)
	set(PROJECT_VERSION_MAJOR 0)
	set(PROJECT_VERSION_MINOR 28)
	set(PROJECT_VERSION_PATCH 3)
	set(PROJECT_VERSION_GFM 19)
	set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} )

	SET(CMARK_DIR cmark-gfm-0.28.3.gfm.19/src/)
	SET(CMARK_EXT cmark-gfm-0.28.3.gfm.19/extensions/)

	# adopted from the cmake file from cmark
	CONFIGURE_FILE(
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_DIR}/config.h.in
		${CMAKE_CURRENT_BINARY_DIR}/config.h)

	CONFIGURE_FILE(
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_DIR}/cmark-gfm_version.h.in
		${CMAKE_CURRENT_BINARY_DIR}/cmark-gfm_version.h)

	CONFIGURE_FILE(
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_DIR}/libcmark-gfm.pc.in
		${CMAKE_CURRENT_BINARY_DIR}/libcmark-gfm.pc @ONLY)


	INCLUDE_DIRECTORIES(${CMARK_DIR})
	INCLUDE_DIRECTORIES(${CMARK_EXT})
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

	SET(CMARK_SRCS ${CMARK_SRCS}
		${CMARK_DIR}cmark.c
		${CMARK_DIR}node.c
		${CMARK_DIR}iterator.c
		${CMARK_DIR}blocks.c
		${CMARK_DIR}inlines.c
		${CMARK_DIR}scanners.c
		${CMARK_DIR}scanners.re
		${CMARK_DIR}utf8.c
		${CMARK_DIR}buffer.c
		${CMARK_DIR}references.c
		${CMARK_DIR}footnotes.c
		${CMARK_DIR}map.c
		${CMARK_DIR}render.c
		${CMARK_DIR}man.c
		${CMARK_DIR}xml.c
		${CMARK_DIR}html.c
		${CMARK_DIR}commonmark.c
		${CMARK_DIR}plaintext.c
		${CMARK_DIR}latex.c
		${CMARK_DIR}houdini_href_e.c
		${CMARK_DIR}houdini_html_e.c
		${CMARK_DIR}houdini_html_u.c
		${CMARK_DIR}cmark_ctype.c
		${CMARK_DIR}arena.c
		${CMARK_DIR}linked_list.c
		${CMARK_DIR}syntax_extension.c
		${CMARK_DIR}registry.c
		${CMARK_DIR}plugin.c
	)

	SET(CMARK_EXT_SRCS
		${CMARK_EXT}core-extensions.c
		${CMARK_EXT}table.c
		${CMARK_EXT}strikethrough.c
		${CMARK_EXT}autolink.c
		${CMARK_EXT}tagfilter.c
		${CMARK_EXT}ext_scanners.c
		${CMARK_EXT}ext_scanners.re
		${CMARK_EXT}ext_scanners.h
	)

	include (GenerateExportHeader)
	ADD_LIBRARY(cmark-gfm ${CMARK_SRCS})
	generate_export_header(cmark-gfm)

	ADD_LIBRARY(cmark-gfm-extensions ${CMARK_EXT_SRCS})
	generate_export_header(cmark-gfm-extensions)

	target_compile_definitions(cmark-gfm PRIVATE -Dtrue=1 -Dfalse=0)
	target_compile_definitions(cmark-gfm PRIVATE -DCMARK_STATIC_DEFINE)

	target_compile_definitions(cmark-gfm-extensions PRIVATE -Dtrue=1 -Dfalse=0)
	target_compile_definitions(cmark-gfm-extensions PRIVATE -DCMARK_STATIC_DEFINE)
	target_compile_definitions(cmark-gfm-extensions PRIVATE -DCMARKEXTENSIONS_STATIC_DEFINE)

	SET(CMARK_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/${CMARK_EXT}
		${CMAKE_CURRENT_BINARY_DIR}
	PARENT_SCOPE)

ENDIF(MARKDOWN_CMARK_GFM)


ADD_LIBRARY(3rdparty	${SRCS})
