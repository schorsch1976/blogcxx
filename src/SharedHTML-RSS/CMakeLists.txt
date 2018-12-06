############################################################################
# Plugins
############################################################################
OPTION(WITH_PLUGINS "With plugins" OFF)
IF (WITH_PLUGINS)
	ADD_DEFINITIONS(-DWITH_PLUGINS)

	SET(SRCS ${SRCS}
		PluginClass.cpp
	)
	SET(TP_SRCS ${TP_SRCS}
		../3rdparty/duktape/duktape/src/duktape.c
	)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/duktape/duktape/src)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/duktape/dukglue/include)
ENDIF (WITH_PLUGINS)

SET(SRCS ${SRCS}
	CleanupDirectory.cpp
	CleanupDirectory.h

	EmojiParserClass.cpp
	EmojiParserClass.h

	ExcerptExtractorClass.cpp
	ExcerptExtractorClass.h

	MarkdownParserClass.cpp
	MarkdownParserClass.h

	MarkdownReturnClass.h

	MarkdownWrapperClass.cpp
	MarkdownWrapperClass.h

	MediaEmbedderClass.cpp
	MediaEmbedderClass.h

	PluginClass.cpp
	PluginClass.h

	Slug.cpp
	Slug.h

	TextParseAll.cpp
	TextParseAll.h
)

IF(MARKDOWN_BLOGCPP)
	ADD_DEFINITIONS(-DUSE_MARKDOWN_API=1)
ENDIF(MARKDOWN_BLOGCPP)

IF(MARKDOWN_CMARK)
	ADD_DEFINITIONS(-DUSE_MARKDOWN_API=2)
ENDIF(MARKDOWN_CMARK)

ADD_LIBRARY(SharedHTML-RSS	${SRCS})
