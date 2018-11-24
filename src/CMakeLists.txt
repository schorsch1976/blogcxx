############################################################################
# Options
############################################################################
OPTION(WITH_DEBUG_LOG "Use the Debug Log" OFF)
if (WITH_DEBUG_LOG)
	ADD_DEFINITIONS(-DWITH_DEBUGLOG)
endif (WITH_DEBUG_LOG)

############################################################################
# Plugins
############################################################################
OPTION(WITH_PLUGINS "With plugins" OFF)
IF (WITH_PLUGINS)
	ADD_DEFINITIONS(-DWITH_PLUGINS)

	SET(TP_SRCS ${TP_SRCS}
		../3rdparty/duktape/duktape/src/duktape.c
	)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/duktape/duktape/src)
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/duktape/dukglue/include)
ENDIF (WITH_PLUGINS)

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

	SET(TP_SRCS ${TP_SRCS}
		../3rdparty/curl/HTTPDownloader.cpp
	)

	SET(LIBS ${LIBS}
		${CURL_LIBRARY}
	)

ENDIF(WITH_OEMBED)

############################################################################
# FreeBSD and Darwin
############################################################################
IF (${FREEBSD} OR ${DARWIN})
  # FreeBSD/Darwin could use /usr/local/
  INCLUDE_DIRECTORIES(/usr/local/include)
  link_directories(/usr/local/lib)
ENDIF(${FREEBSD} OR ${DARWIN})

############################################################################
# 3rdParty includes provided by blogcpp
############################################################################
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/icu/include)


IF(CMAKE_CL_64)
	link_directories(${CMAKE_SOURCE_DIR}/3rdparty/icu/lib64)
ELSE(CMAKE_CL_64)
	link_directories(${CMAKE_SOURCE_DIR}/3rdparty/icu/lib)
ENDIF(CMAKE_CL_64)

############################################################################
# 3rd Party Sources from blogcpp that are always needed
############################################################################
SET(TP_SRCS ${TP_SRCS}
    # included in src
    ../3rdparty/XML/tinyxml2.cpp
)

############################################################################
# blogcpp sources
############################################################################
SET(BLOGCPP_SRCS
    DebugClass.cpp
    ContentsGeneratorClass.cpp
    EmojiParserClass.cpp
    ExcerptExtractorClass.cpp
    MarkdownParserClass.cpp
    MediaEmbedderClass.cpp
    CommentIntegrationClass.cpp
    PluginClass.cpp
    RSSGeneratorClass.cpp
    MarkdownWrapperClass.cpp
    TemplateWrapperClass.cpp
    blogcpp.cpp
)

add_executable(blogcpp ${BLOGCPP_SRCS} ${TP_SRCS})

############################################################################
# Target specific libraries
############################################################################

# common

IF(UNIX)
    OPTION(USE_CLANG_STDCPP_LIB "Use libc++" ON)
ENDIF(UNIX)

IF (FREEBSD OR DARWIN)
    SET(LIBS    ${LIBS}
        icui18n 
        icuuc 

        thr
    )
    IF (USE_CLANG_STDCPP_LIB)
        SET(LIBS    ${LIBS}
            c++ 
            c++experimental
        )
    ELSE(USE_CLANG_STDCPP_LIB)
        SET(LIBS    ${LIBS}
            stdc++
            stdc++fs
        )
    ENDIF (USE_CLANG_STDCPP_LIB)
ENDIF (FREEBSD OR DARWIN)

IF (${LINUX})
    SET(LIBS    ${LIBS}
        icui18n 
        icuuc 
        stdc++ 
        stdc++fs 
        pthread
    )    
ENDIF (${LINUX})

IF (WIN32)
    SET(LIBS    ${LIBS}
        icuin 
        icuuc
    )    
ENDIF (WIN32)

MESSAGE(STATUS "Linking these libraries: ${LIBS}")

TARGET_LINK_LIBRARIES(blogcpp  ${LIBS})

