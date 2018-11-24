############################################################################
# CURL
find_package(CURL)
IF(NOT ${CURL_FOUND})
	MESSAGE(FATAL_ERROR "CURL not Found: Set in in the ADVANCED SETTINGS")
ENDIF(NOT ${CURL_FOUND})
include_directories(${CURL_INCLUDE_DIRS})

############################################################################
# FreeBSD and Darwin
############################################################################
IF (${FREEBSD} OR ${DARWIN})
  # FreeBSD/Darwin could use /usr/local/
  include_directories(/usr/local/include)
  link_directories(/usr/local/lib)
ENDIF(${FREEBSD} OR ${DARWIN})

############################################################################
# 3rdParty includes provided by blogcpp
############################################################################
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/icu/include)

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/duktape/duktape/src)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/duktape/dukglue/include)

IF(CMAKE_CL_64)
	link_directories(${CMAKE_SOURCE_DIR}/3rdparty/icu/lib64)
ELSE(CMAKE_CL_64)
	link_directories(${CMAKE_SOURCE_DIR}/3rdparty/icu/lib)
ENDIF(CMAKE_CL_64)

############################################################################
# 3rd Party Sources from blogcpp
############################################################################
SET(TP_SRCS
    # included in src
    ../3rdparty/duktape/duktape/src/duktape.c
    ../3rdparty/curl/HTTPDownloader.cpp
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
SET(LIBS 
    ${CURL_LIBRARY}
)

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

