INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty)

SET(SRCS
	CreateRSS.cpp
	CreateRSS.h

	RSSGeneratorClass.cpp
	RSSGeneratorClass.h
)

ADD_LIBRARY(MetadataToRSS ${SRCS})
