INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

############################################################################
# inja inja-1.1.0-pre + json
############################################################################
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/inja-1.1.0-pre/src)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/3rdparty/inja-1.1.0-pre/src/nlohmann)

############################################################################
# FreeBSD and Darwin
############################################################################
IF (${FREEBSD} OR ${DARWIN})
  # FreeBSD/Darwin could use /usr/local/
  INCLUDE_DIRECTORIES(/usr/local/include)
  link_directories(/usr/local/lib)
ENDIF(${FREEBSD} OR ${DARWIN})


ADD_SUBDIRECTORY(FilesToMetadata)
ADD_SUBDIRECTORY(MetadataToHTML)
ADD_SUBDIRECTORY(MetadataToRSS)
ADD_SUBDIRECTORY(SharedHTML-RSS)
ADD_SUBDIRECTORY(Shared)

ADD_SUBDIRECTORY(bin2cxx)
ADD_SUBDIRECTORY(blogcpp)






