SET(SRCS
	CreateHTMLAndRSS.cpp
	CreateHTMLAndRSS.h

	CreateArchive.cpp
	CreateArchive.h

	CreatePage.cpp
	CreatePage.h

	CreatePost.cpp
	CreatePost.h

	CreateIndex.cpp
	CreateIndex.h

	CreateRSS.cpp
	CreateRSS.h

	TemplateWrapper.cpp
	TemplateWrapper.h
)

ADD_LIBRARY(MetadataToHTMLAndRSS ${SRCS})
