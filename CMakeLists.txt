CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(connPool)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

option(WITH_REDIS "compile redis" OFF)

IF (UNIX)
	SET(CMAKE_CXX_COMPILER "g++")
	SET(CMAKE_C_COMPILER "gcc")
	SET(CMAKE_CXX_FLAGS "-std=c++11 -g -w -Wall -Wl,--rpath=./ -fPIC -fpermissive")
else()
	SET(CMAKE_CXX_FLAGS "-DDBAPI_EXPORTS")
ENDIF()

IF(WITH_REDIS)
	ADD_COMPILE_OPTIONS(-DWITH_REDIS)
	INCLUDE_DIRECTORIES(${srcDir}/3rd/inc/hiredis/)
	ADD_LINK_OPTIONS(-lhiredis)
ENDIF()

set (srcDir ${PROJECT_SOURCE_DIR})

# Add the source in project root directory
aux_source_directory(. DIRSRCS)
aux_source_directory(${srcDir}/dbfield DIRSRCS)

# Add header file include directories
INCLUDE_DIRECTORIES(${srcDir} ${srcDir}/dbfield)
INCLUDE_DIRECTORIES(${srcDir}/3rd/inc/)

LINK_DIRECTORIES(${srcDir}/3rd/libs)

# libraries
if (UNIX)
set(ADDLIBS ${ADDLIBS} sqlapi pthread dl rt)
else()
set(ADDLIBS ${ADDLIBS} sqlapi ws2_32)
endif()
add_executable ( connPool ${DIRSRCS})
TARGET_LINK_LIBRARIES( connPool ${ADDLIBS})

install( TARGETS connPool
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
