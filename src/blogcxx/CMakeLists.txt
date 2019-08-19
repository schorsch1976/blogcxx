############################################################################
# blogcxx sources
############################################################################
SET(BLOGCPP_SRCS ${BLOGCPP_SRCS}
	blogcxx.cpp

	CmdNewArticle.cpp
	CmdNewArticle.h

	CmdNewPage.cpp
	CmdNewPage.h

	CmdGenerateMetadata.cpp
	CmdGenerateMetadata.h

	ContentsGenerator.cpp
	ContentsGenerator.h
)

ADD_EXECUTABLE(blogcxx ${BLOGCPP_SRCS})

INSTALL(TARGETS			blogcxx		DESTINATION bin)

############################################################################
# embed the default template as cpp source for the howto page generator
############################################################################
GENERATE_CXX(blogcxx template templates/default/archives.txt)
GENERATE_CXX(blogcxx template templates/default/footer.txt)
GENERATE_CXX(blogcxx template templates/default/header.txt)
GENERATE_CXX(blogcxx template templates/default/index.txt)
GENERATE_CXX(blogcxx template templates/default/page.txt)
GENERATE_CXX(blogcxx template templates/default/post.txt)
GENERATE_CXX(blogcxx template templates/default/sidebar.txt)

GENERATE_CXX(blogcxx template templates/default/static/blogcxx.js)
GENERATE_CXX(blogcxx template templates/default/static/favicon.ico)
GENERATE_CXX(blogcxx template templates/default/static/logo.svg)
GENERATE_CXX(blogcxx template templates/default/static/madeincpp.png)
GENERATE_CXX(blogcxx template templates/default/static/manifest.json)
GENERATE_CXX(blogcxx template templates/default/static/styles.css)
GENERATE_CXX(blogcxx template templates/default/static/vanillajs.png)

GENERATE_CXX(blogcxx template templates/RSS.txt)

GENERATE_CXX(blogcxx comments comments/commento.txt)
GENERATE_CXX(blogcxx comments comments/discourse.txt)
GENERATE_CXX(blogcxx comments comments/disqus.txt)
GENERATE_CXX(blogcxx comments comments/hashover.txt)
GENERATE_CXX(blogcxx comments comments/isso.txt)

GENERATE_CXX(blogcxx template README.md)

############################################################################
# common
############################################################################
SET(LIBS
	FilesToMetadata
	MetadataToHTMLAndRSS
	Shared
	Log
	htmltemplate++
	${BLOG_CXX_LIBS}
)

MESSAGE(STATUS "Linking these libraries: ${LIBS}")

TARGET_LINK_LIBRARIES(blogcxx  ${LIBS})
