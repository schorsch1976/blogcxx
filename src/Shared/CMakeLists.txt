SET(SRCS
	constants.h
	AsyncWorker.h

	ConfigCollection.cpp
	ConfigCollection.h

	Debug.cpp
	DebugImpl.h
	Debug.h

	filesystem.h

	Helpers.cpp
	Helpers.h

	Metadata.cpp
	Metadata.h

	SingleItem.h

	PrintVersion.cpp
	PrintVersion.h
)

ADD_LIBRARY(Shared ${SRCS})
