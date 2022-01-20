CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )

MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBIITY)

PROJECT(login-service)

#include(grpc/cmake/common.cmake)
#message(STATUS "===================Using protobuf ${Protobuf_VERSION}")

SET (${PROJECT_NAME}_MAJOR_VERSION 1)
SET (${PROJECT_NAME}_MINOR_VERSION 0)
SET (${PROJECT_NAME}_PATCH_VERSION 0)

FILE(GLOB Hdrs ${PROJECT_SOURCE_DIR}/include/*.hpp)
FILE(GLOB Impl ${PROJECT_SOURCE_DIR}/src/*.cpp)
SET (SOURCES ${Hdrs} ${Impl})
	
#FIND_PACKAGE(Openssl REQUIRED)
#FIND_PACKAGE(MariadbConnector REQUIRED)
#FIND_PACKAGE(Json11 REQUIRED)
#FIND_PACKAGE(Pistache REQUIRED)
#FIND_PACKAGE(Googletest REQUIRED)

SET(3RD_ROOT "${PROJECT_SOURCE_DIR}/../3rd")

#SET(Openssl_ROOT "${3RD_ROOT}/openssl-3.0.1")
SET(Boost_ROOT "${3RD_ROOT}/boost_1_78_0")
SET(MariadbConnector_ROOT "${3RD_ROOT}/mariadb-connector-c-3.2.5-src")
SET(Json11_ROOT "${3RD_ROOT}/json11-1.0.0")
SET(Pistache_ROOT "${3RD_ROOT}/pistache/")
SET(Googletest_ROOT "${3RD_ROOT}/googletest-release-1.11.0")
SET(Yamlcpp_ROOT "${3RD_ROOT}/yaml-cpp-yaml-cpp-0.7.0")
SET(GSL_ROOT "${3RD_ROOT}/GSL-3.1.0")

SET(LIB_MARIADB_ACCESS_ROOT "${PROJECT_SOURCE_DIR}/../lib-mariadb-access")

SET(GRPC_INCLUDE "grpc/include")
#SET(Openssl_INCLUDE "${Openssl_ROOT}/include")
SET(Boost_INCLUDE "${Boost_ROOT}")
SET(MariadbConnector_INCLUDE "${MariadbConnector_ROOT}/include")
SET(Json11_INCLUDE "${Json11_ROOT}")
SET(Pistache_INCLUDE "${Pistache_ROOT}/include")
SET(Googletest_INCLUDE "${Googletest_ROOT}/googletest/include
	${Googletest_ROOT}/googlemock/include")
SET(Yamlcpp_INCLUDE "${Yamlcpp_ROOT}/include")
SET(GSL_INCLUDE "${GSL_ROOT}/include")

SET(LIB_MARIADB_ACCESS_INCLUDE "${LIB_MARIADB_ACCESS_ROOT}/include")

#SET(Openssl_LIBRARY_DIR "${Openssl_ROOT}/src")
SET(Boost_LIBRARY_DIR "${Boost_ROOT}/stage/lib")
SET(MariadbConnector_LIBRARY_DIR "${MariadbConnector_ROOT}/libmariadb/")
SET(Json11_LIBRARY_DIR "${Json11_ROOT}")
SET(Pistache_LIBRARY_DIR "${Pistache_ROOT}/src")
SET(Googletest_LIBRARY_DIR "${Googletest_ROOT}/lib")
SET(Yamlcpp_LIBRARY_DIR "${Yamlcpp_ROOT}/lib")

MESSAGE("########################## ${PROJECT_SOURCE_DIR}/../build/bin")

#SET(Openssl_LIBRARY "${Openssl_LIBRARY_DIR}/lib")
SET(Boost_LIBRARY "libboost_log.so")
SET(MariadbConnector_LIBRARY "libmariadb.so")
SET(Json11_LIBRARY "${Json11_ROOT}/libjson11.a")
SET(Pistache_LIBRARY "libpistache.so")
SET(Googletest_LIBRARY )
SET(Yamlcpp_LIBRARY "${Yamlcpp_ROOT}/libyaml-cpp.a")
SET(GRPC_LIBRARY "${PROJECT_SOURCE_DIR}/../build/bin/libhw_grpc_proto.a")

SET(LIB_MARIADB_ACCESS_LIBRARY "${PROJECT_SOURCE_DIR}/../build/bin/libmariadb_access.so")

SET(GRPC_DLL_LIBRARY "${PROJECT_SOURCE_DIR}/../build/bin/liblogin_service_grpc.so")

SET(CMAKE_DEBUG_POSTFIX d) 

INCLUDE_DIRECTORIES(${Openssl_INCLUDE} ${Boost_INCLUDE}
	${MariadbConnector_INCLUDE} ${Json11_INCLUDE} ${Pistache_INCLUDE}
	${Googletest_INCLUDE} ${Yamlcpp_INCLUDE} ${GRPC_INCLUDE}
	${LIB_MARIADB_ACCESS_INCLUDE} ${GSL_INCLUDE}
	${PROJECT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR})

LINK_DIRECTORIES(${Openssl_LIBRARY_DIR} ${Boost_LIBRARY_DIR}
	${MariadbConnector_LIBRARY_DIR} ${Json11_LIBRARY_DIR}
	${Pistache_LIBRARY_DIR} ${Googletest_LIBRARY_DIR}
	${Yamlcpp_LIBRARY_DIR})

SET(CMAKE_DEBUG_POSTFIX d) 
SET (LIBRARIES_TO_IMPORT_DEBUG)
SET (LIBRARIES_TO_IMPORT_OPTIMIZED ${Openssl_LIBRARY} ${Boost_LIBRARY}
	${MariadbConnector_LIBRARY} ${Json11_LIBRARY} ${Pistache_LIBRARY}
	${Googletest_LIBRARY} ${Yamlcpp_LIBRARY}
	${GRPC_DLL_LIBRARY} ${GRPC_LIBRARY}
	${LIB_MARIADB_ACCESS_LIBRARY} -pthread)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} )
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES_TO_IMPORT}
	debug ${LIBRARIES_TO_IMPORT_DEBUG}
	optimized ${LIBRARIES_TO_IMPORT_OPTIMIZED})

GET_PROPERTY(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	PROPERTY INCLUDE_DIRECTORIES)

foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin COMPONENT applications)

ADD_SUBDIRECTORY (${PROJECT_SOURCE_DIR}/grpc)
TARGET_COMPILE_FEATURES(${PROJECT_NAME} PRIVATE cxx_std_17)