SET(SRCS ${SRCS}
	bin2cxx.cpp
)

ADD_EXECUTABLE(bin2cxx	${SRCS})
TARGET_LINK_LIBRARIES(bin2cxx  Shared Log ${BLOG_CXX_LIBS})
