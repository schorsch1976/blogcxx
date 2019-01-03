INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

############################################################################
# FreeBSD and Darwin
############################################################################
IF (${FREEBSD} OR ${DARWIN})
  # FreeBSD/Darwin could use /usr/local/
  INCLUDE_DIRECTORIES(/usr/local/include)
  link_directories(/usr/local/lib)
ENDIF(${FREEBSD} OR ${DARWIN})


ADD_SUBDIRECTORY(Log)
ADD_SUBDIRECTORY(FilesToMetadata)
ADD_SUBDIRECTORY(MetadataToHTMLAndRSS)
ADD_SUBDIRECTORY(Shared)
ADD_SUBDIRECTORY(html-template++)

ADD_SUBDIRECTORY(bin2cxx)
ADD_SUBDIRECTORY(blogcxx)

