IF(WIN32)
	IF(MSVC)
		cmake_minimum_required(VERSION 3.13) # because MSVC and C++17
	ENDIF(MSVC)
	IF(MSYS)
		cmake_minimum_required(VERSION 3.12)
	ENDIF(MSYS)
ELSE(WIN32)
	cmake_minimum_required(VERSION 3.7)
ENDIF(WIN32)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

PROJECT(blogcxx C CXX)

#######################################################################
# Include needed Macros
#######################################################################
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMake)
INCLUDE (ConfigureMSVCRuntime)
INCLUDE (GENERATE_CXX)
INCLUDE (ExternalProject)

######################################################################
# OS
######################################################################
IF (UNIX)
	MESSAGE(STATUS "This is UNIX")
	ADD_DEFINITIONS(-DUNIX)

	############################################################################
	# CPP FLAGS
	############################################################################
	SET(CMAKE_CXX_FLAGS   "-Wall -Wno-deprecated-declarations ${CMAKE_CXX_FLAGS} -Werror=switch")

	############################################################################
	# RPATH
	# https://cmake.org/Wiki/CMake_RPATH_handling
	############################################################################
	# use, i.e. don't skip the full RPATH for the build tree
	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

	# when building, don't use the install RPATH already
	# (but later on when installing)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

	SET(CMAKE_INSTALL_RPATH "${TARGET_LIB_PATH}")

	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


	# the RPATH to be used when installing, but only if it's not a system directory
	LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${TARGET_LIB_PATH}" isSystemDir)
	IF("${isSystemDir}" STREQUAL "-1")
		SET(CMAKE_INSTALL_RPATH "${TARGET_LIB_PATH}")
	ENDIF("${isSystemDir}" STREQUAL "-1")

	IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        MESSAGE(STATUS "This is Linux")
		ADD_DEFINITIONS(-DLINUX)
		
		SET(LINUX TRUE)
		SET(FREEBSD FALSE)
		SET(DARWIN FALSE)

	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	IF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
        MESSAGE(STATUS "This is FreeBSD")
		ADD_DEFINITIONS(-DFREEBSD)
		
		SET(LINUX FALSE)
		SET(FREEBSD TRUE)
		SET(DARWIN FALSE)

	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        MESSAGE(STATUS "This is Darwin")
		ADD_DEFINITIONS(-DDARWIN)

        SET(LINUX FALSE)
		SET(FREEBSD FALSE)
		SET(DARWIN TRUE)

	ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
ENDIF (UNIX)
 
IF (WIN32)
IF (MSVC)
	MESSAGE(STATUS "This is Windows: Microsoft Visual Studio")

	############################################################################
	# CPP FLAGS
	############################################################################
	SET(LIB_CXX_FLAGS "")

	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

	############################################################################
	# PATHS for targets
	############################################################################
	SET(TARGET_BIN_PATH "${CMAKE_INSTALL_PREFIX}/")
	SET(TARGET_LIB_PATH "${CMAKE_INSTALL_PREFIX}/")
ENDIF (MSVC)
IF(MSYS)
	MESSAGE(STATUS "This is Windows: MSYS")
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations)
ENDIF(MSYS)
ENDIF (WIN32)

############################################################################
# Linkage
############################################################################
OPTION(STATIC_LINKING "Link everything static" OFF)
IF (STATIC_LINKING)
	ConfigureMSVCRuntime(static)

	SET(Boost_USE_STATIC_LIBS		ON)
	SET(Boost_USE_STATIC_RUNTIME	ON)
ELSE (STATIC_LINKING)
	ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK=1)
ENDIF (STATIC_LINKING)

############################################################################
# Test on older compilers
############################################################################
OPTION(TEST_OLDER_COMPILERS "Test on older compilers" OFF)
IF (TEST_OLDER_COMPILERS)
	ADD_DEFINITIONS(-DTEST_OLDER_COMPILERS)
ENDIF (TEST_OLDER_COMPILERS)

############################################################################
# markdown
############################################################################
SET(MARKDOWN_API "libcmark-gfm" CACHE STRING "Use this markdown implementation")
SET_PROPERTY(CACHE MARKDOWN_API PROPERTY STRINGS "blogcpp" "libcmark" "libcmark-gfm")

IF (MARKDOWN_API STREQUAL "blogcpp")
	ADD_DEFINITIONS(-DUSE_MARKDOWN_API=1)

	SET(MARKDOWN_BLOGCPP ON)
	SET(MARKDOWN_CMARK OFF)
	SET(MARKDOWN_CMARK_GFM OFF)
ENDIF (MARKDOWN_API STREQUAL "blogcpp")

# libcmark
IF (MARKDOWN_API STREQUAL "libcmark")
	ADD_DEFINITIONS(-DUSE_MARKDOWN_API=2)

	SET(MARKDOWN_BLOGCPP OFF)
	SET(MARKDOWN_CMARK ON)
	SET(MARKDOWN_CMARK_GFM OFF)

ENDIF (MARKDOWN_API STREQUAL "libcmark")

# libcmark-gfm
IF (MARKDOWN_API STREQUAL "libcmark-gfm")
	ADD_DEFINITIONS(-DUSE_MARKDOWN_API=3)

	SET(MARKDOWN_BLOGCPP OFF)
	SET(MARKDOWN_CMARK OFF)
	SET(MARKDOWN_CMARK_GFM ON)
ENDIF (MARKDOWN_API STREQUAL "libcmark-gfm")

############################################################################
# variant
############################################################################
SET(VARIANT_API "boost::variant" CACHE STRING "Use this variant API")
SET_PROPERTY(CACHE VARIANT_API PROPERTY STRINGS "std::variant" "boost::variant")

IF (VARIANT_API STREQUAL "std::variant")
	ADD_DEFINITIONS(-DUSE_VARIANT_API=1)
ENDIF (VARIANT_API STREQUAL "std::variant")

IF (VARIANT_API STREQUAL "boost::variant")
	ADD_DEFINITIONS(-DUSE_VARIANT_API=2)
ENDIF (VARIANT_API STREQUAL "boost::variant")

############################################################################
# filesystem
############################################################################
SET(FILESYSTEM_API "boost::filesystem" CACHE STRING "Use this filesystem API")
SET_PROPERTY(CACHE FILESYSTEM_API PROPERTY STRINGS "std::filesystem" "std::experimental::filesystem" "boost::filesystem")

IF (FILESYSTEM_API STREQUAL "std::filesystem")
	ADD_DEFINITIONS(-DUSE_FILESYSTEM_API=1)
	SET(STD_FILESYSTEM ON)
	SET(STD_EX_FILESYSTEM OFF)
	SET(BOOST_FILESYSTEM OFF)
ENDIF (FILESYSTEM_API STREQUAL "std::filesystem")

IF (FILESYSTEM_API STREQUAL "std::experimental::filesystem")
	ADD_DEFINITIONS(-DUSE_FILESYSTEM_API=2)
	SET(STD_FILESYSTEM OFF)
	SET(STD_EX_FILESYSTEM ON)
	SET(BOOST_FILESYSTEM OFF)
ENDIF (FILESYSTEM_API STREQUAL "std::experimental::filesystem")

IF (FILESYSTEM_API STREQUAL "boost::filesystem")
	ADD_DEFINITIONS(-DUSE_FILESYSTEM_API=3)
	SET(STD_FILESYSTEM OFF)
	SET(STD_EX_FILESYSTEM OFF)
	SET(BOOST_FILESYSTEM ON)
ENDIF (FILESYSTEM_API STREQUAL "boost::filesystem")

############################################################################
# Use as much boost as we can to reduce the 3rdparty library count
############################################################################
SET(BOOST_VERSION "1.62.0")

# boost log uses boost::filesystem internaly
SET(Boost_USE_MULTITHREADED		ON)
FIND_PACKAGE(Boost ${BOOST_VERSION} COMPONENTS
	date_time
	locale
	filesystem
	program_options
	system
	log
	log_setup

	REQUIRED
)

ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

SET(BOOST_LIBS
	${Boost_ATOMIC_LIBRARY}
	${Boost_CHRONO_LIBRARY}
	${Boost_FILESYSTEM_LIBRARY}
	${Boost_REGEX_LIBRARY}
	${Boost_LOG_LIBRARY}
	${Boost_LOG_SETUP_LIBRARY}
	${Boost_DATE_TIME_LIBRARY}
	${Boost_SYSTEM_LIBRARY}
	${Boost_THREAD_LIBRARY}
	${Boost_LOCALE_LIBRARY}
	${Boost_PROGRAM_OPTIONS_LIBRARY}
)

IF(LINUX)
	# on Linux, the locale backend is ICU
	FIND_PACKAGE(ICU COMPONENTS
		uc i18n io lx tu
		REQUIRED
	)
	SET(BOOST_LIBS    ${BOOST_LIBS}
		${ICU_LIBRARIES}
	)
ENDIF(LINUX)


IF(WIN32)
	IF(MSVC)
		# if Boost log fail to link, you probaly need to compile your boost with
		# the same BOOST_USE_WINAPI_VERSION as here. You must also use
		# BOOST_ALL_DYN_LINK=1
		SET(BOOST_LOG_API "BOOST_WINAPI_VERSION_WIN7" CACHE STRING "Boost Log Backend API")
		SET_PROPERTY(CACHE BOOST_LOG_API PROPERTY STRINGS BOOST_WINAPI_VERSION_WINXP BOOST_WINAPI_VERSION_WIN7)

		ADD_DEFINITIONS(-DBOOST_USE_WINAPI_VERSION=${BOOST_LOG_API})
		ADD_DEFINITIONS(-D${Boost_LIB_DIAGNOSTICDEFINITIONS})
	ENDIF(MSVC)
ENDIF(WIN32)

############################################################################
# Combine the CXX Libs
############################################################################
IF(MARKDOWN_CMARK)
	SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
		cmark
	)
ENDIF(MARKDOWN_CMARK)

IF(MARKDOWN_CMARK_GFM)
	SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
		cmark-gfm-extensions
		cmark-gfm
	)
ENDIF(MARKDOWN_CMARK_GFM)

IF(UNIX)
	IF (LINUX)
		SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
			${BOOST_LIBS}
			stdc++
			stdc++fs
		)
		IF (STD_FILESYSTEM)
			SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
				stdc++fs
			)
		ENDIF (STD_FILESYSTEM)
	ENDIF (LINUX)

	IF(FREEBSD)
		OPTION(USE_CLANG_STDCPP_LIB "Use libc++" ON)
		IF (USE_CLANG_STDCPP_LIB)
			SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
				${BOOST_LIBS}
				c++
			)
			IF (STD_EX_FILESYSTEM)
				SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
					c++experimental
				)
			ENDIF (STD_EX_FILESYSTEM)
		ELSE(USE_CLANG_STDCPP_LIB)
			SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
				${BOOST_LIBS}
				stdc++
			)
		ENDIF (USE_CLANG_STDCPP_LIB)
		SET(BLOG_CXX_LIBS ${BLOG_CXX_LIBS}
			thr
		)
	ENDIF(FREEBSD)

    # common in unix
	SET(BLOG_CXX_LIBS ${BLOG_CXX_LIBS}
		pthread
	)

ENDIF(UNIX)

IF(WIN32)
	IF(MSVC)
		SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
			${BOOST_LIBS}
		)
	ENDIF(MSVC)

IF(MSYS)
		FIND_PACKAGE(Iconv REQUIRED)
		SET(BLOG_CXX_LIBS    ${BLOG_CXX_LIBS}
			${BOOST_LIBS}
			${Iconv_LIBRARIES}
		)
ENDIF(MSYS)

ENDIF(WIN32)


ADD_SUBDIRECTORY(3rdparty)

INCLUDE_DIRECTORIES(${CMARK_INCLUDE_DIR})

ADD_SUBDIRECTORY(src)
