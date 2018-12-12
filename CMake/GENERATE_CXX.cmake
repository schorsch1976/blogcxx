############################################################################
# embed a file as cpp source
############################################################################
SET(BIN2CXX	${CMAKE_BINARY_DIR}/src/bin2cxx/${CMAKE_CFG_INTDIR}/bin2cxx${CMAKE_EXECUTABLE_SUFFIX})

MACRO(GENERATE_CXX target prefix file)
	STRING(REPLACE "/" "-" s1 ${file})
	STRING(REPLACE "." "-" stemout ${s1})


	SET (IN_FILE "${CMAKE_SOURCE_DIR}/${file}")
	SET (OUT_FILE_H "${CMAKE_CURRENT_BINARY_DIR}/${prefix}-${stemout}.h")
	SET (OUT_FILE_CXX "${CMAKE_CURRENT_BINARY_DIR}/${prefix}-${stemout}.cpp")

	message(STATUS "GENERATE_CXX ${target} : ${IN_FILE}->${OUT_FILE_H}")
	message(STATUS "GENERATE_CXX ${target} : ${IN_FILE}->${OUT_FILE_CXX}")

	ADD_CUSTOM_COMMAND(
		OUTPUT	"${OUT_FILE_H}"
		COMMAND	"${BIN2CXX}" "--in='${IN_FILE}'" "--mode=1" "--out='${OUT_FILE_H}'"
		DEPENDS		bin2cxx "${IN_FILE}"
	)

	ADD_CUSTOM_COMMAND(
		OUTPUT	"${OUT_FILE_CXX}"
		COMMAND	"${BIN2CXX}" "--in='${IN_FILE}'" "--mode=0" "--out='${OUT_FILE_CXX}'"
		DEPENDS		bin2cxx "${IN_FILE}"
	)


	TARGET_INCLUDE_DIRECTORIES(${target} BEFORE PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
	target_sources(${target} PRIVATE ${OUT_FILE_H})
	target_sources(${target} PRIVATE ${OUT_FILE_CXX})
ENDMACRO(GENERATE_CXX)