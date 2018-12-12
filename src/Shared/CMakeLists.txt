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

	CleanupDirectory.cpp
	CleanupDirectory.h

	CopyDirectory.cpp
	CopyDirectory.h

	EmojiParserClass.cpp
	EmojiParserClass.h

	ExcerptExtractorClass.cpp
	ExcerptExtractorClass.h

	MediaEmbedderClass.cpp
	MediaEmbedderClass.h

	Slug.cpp
	Slug.h

	TextParseAll.h
)

ADD_LIBRARY(Shared ${SRCS})
