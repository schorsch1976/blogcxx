SET(SRCS ${SRCS}
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

IF(MARKDOWN_CMARK)
	SET(SRCS    ${SRCS}
		TextParseAllCMark.cpp
	)
ENDIF(MARKDOWN_CMARK)

IF(MARKDOWN_CMARK_GFM)
	SET(SRCS    ${SRCS}
		TextParseAllCMarkGfm.cpp
	)
ENDIF(MARKDOWN_CMARK_GFM)

ADD_LIBRARY(SharedHTML-RSS	${SRCS})
