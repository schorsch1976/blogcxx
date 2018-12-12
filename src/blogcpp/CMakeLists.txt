############################################################################
# blogcpp sources
############################################################################
SET(BLOGCPP_SRCS ${BLOGCPP_SRCS}
	blogcpp.cpp

	CmdNewArticle.cpp
	CmdNewArticle.h

	CmdNewPage.cpp
	CmdNewPage.h

	CmdGenerateMetadata.cpp
	CmdGenerateMetadata.h

	ContentsGenerator.cpp
	ContentsGenerator.h
)

ADD_EXECUTABLE(blogcpp ${BLOGCPP_SRCS})

INSTALL(TARGETS			blogcpp		DESTINATION bin)

############################################################################
# embed the default template as cpp source for the howto page generator
############################################################################
GENERATE_CXX(blogcpp template templates/default/archives.txt)
GENERATE_CXX(blogcpp template templates/default/footer.txt)
GENERATE_CXX(blogcpp template templates/default/header.txt)
GENERATE_CXX(blogcpp template templates/default/index.txt)
GENERATE_CXX(blogcpp template templates/default/page.txt)
GENERATE_CXX(blogcpp template templates/default/post.txt)
GENERATE_CXX(blogcpp template templates/default/sidebar.txt)

GENERATE_CXX(blogcpp template templates/default/static/blogcpp.js)
GENERATE_CXX(blogcpp template templates/default/static/favicon.ico)
GENERATE_CXX(blogcpp template templates/default/static/logo.svg)
GENERATE_CXX(blogcpp template templates/default/static/madeincpp.png)
GENERATE_CXX(blogcpp template templates/default/static/manifest.json)
GENERATE_CXX(blogcpp template templates/default/static/styles.css)
GENERATE_CXX(blogcpp template templates/default/static/vanillajs.png)

GENERATE_CXX(blogcpp template README.md)

############################################################################
# common
############################################################################
SET(LIBS
	FilesToMetadata
	MetadataToHTML
	MetadataToRSS
	SharedHTML-RSS
	Shared
	3rdparty
	${BLOG_CXX_LIBS}
)

MESSAGE(STATUS "Linking these libraries: ${LIBS}")

TARGET_LINK_LIBRARIES(blogcpp  ${LIBS})
