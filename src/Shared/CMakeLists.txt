SET(SRCS
	AsyncWorker.h

	ConfigCollection.cpp
	ConfigCollection.h

	filesystem.h
	regex.h
	variant.h

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

	EmojiParser.cpp
	EmojiParser.h

	ExcerptExtractor.cpp
	ExcerptExtractor.h

	MediaEmbedderClass.cpp
	MediaEmbedderClass.h

	Slug.cpp
	Slug.h

	TextParseAll.h
)

IF(MARKDOWN_BLOGCPP)
	SET(SRCS    ${SRCS}
		MarkdownParserClass.cpp
		MarkdownParserClass.h
		MarkdownReturnClass.h
		MarkdownWrapperClass.cpp
		MarkdownWrapperClass.h

		TextParseAllBlogCpp.cpp
	)
ENDIF(MARKDOWN_BLOGCPP)

IF(MARKDOWN_CMARK_GFM)
	SET(SRCS    ${SRCS}
		TextParseAllCMarkGfm.cpp
	)
ENDIF(MARKDOWN_CMARK_GFM)

ADD_LIBRARY(Shared ${SRCS})
