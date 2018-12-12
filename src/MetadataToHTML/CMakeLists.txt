SET(SRCS
	CommentIntegrationClass.cpp
	CommentIntegrationClass.h

	CreateHTML.cpp
	CreateHTML.h

	CreateArchive.cpp
	CreateArchive.h

	CreatePage.cpp
	CreatePage.h

	CreatePost.cpp
	CreatePost.h

	CreateIndex.cpp
	CreateIndex.h

	TemplateWrapper.cpp
	TemplateWrapper.h
)

ADD_LIBRARY(MetadataToHTML ${SRCS})
