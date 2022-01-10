CMAKE_MINIMUM_REQUIRED( VERSION 3.0 )
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBIITY)
PROJECT(login-service)
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

SET(Openssl_ROOT "${3RD_ROOT}/openssl-3.0.1")
SET(MariadbConnector_ROOT "${3RD_ROOT}/SDL2-2.0.18")
SET(Json11_ROOT "${3RD_ROOT}/json11-1.0.0")
SET(Pistache_ROOT "${3RD_ROOT}/pistache")
SET(Googletest_ROOT "${3RD_ROOT}/googletest-release-1.11.0")

SET(Openssl_INCLUDE "${Openssl_ROOT}/include")
SET(MariadbConnector_INCLUDE "${MariadbConnector_ROOT}/include")
SET(Json11_INCLUDE "${Json11_ROOT}/include")
SET(Pistache_INCLUDE "${Pistache_ROOT}/include")
SET(Googletest_INCLUDE "${Googletest_ROOT}/include")

SET(Openssl_LIBRARY_DIR "${Openssl_ROOT}/lib")
SET(MariadbConnector_LIBRARY_DIR "${MariadbConnector_ROOT}/lib")
SET(Json11_LIBRARY_DIR "${Json11_ROOT}/lib")
SET(Pistache_LIBRARY_DIR "${Pistache_ROOT}/lib")
SET(Googletest_LIBRARY_DIR "${Googletest_ROOT}/lib")

SET(Openssl_LIBRARY "${Openssl_LIBRARY_DIR}/lib")
SET(MariadbConnector_LIBRARY "${MariadbConnector_LIBRARY_DIR}/lib")
SET(Json11_LIBRARY "${Json11_LIBRARY_DIR}/lib")
SET(Pistache_LIBRARY "${Pistache_LIBRARY_DIR}/lib")
SET(Googletest_LIBRARY "${Googletest_LIBRARY_DIR}/lib")

LINK_DIRECTORIES(${Openssl_LIBRARY} ${MariadbConnector_LIBRARY} ${Json11_LIBRARY} ${Pistache_LIBRARY} ${Googletest_LIBRARY})
SET(CMAKE_DEBUG_POSTFIX d) 

INCLUDE_DIRECTORIES(${Openssl_INCLUDE} ${MariadbConnector_INCLUDE} ${Json11_INCLUDE} ${Pistache_INCLUDE} ${Googletest_INCLUDE}
			${PROJECT_SOURCE_DIR}/include
			${CMAKE_BINARY_DIR})

LINK_DIRECTORIES(${Openssl_LIBRARY_DIR} ${MariadbConnector_LIBRARY_DIR} ${Json11_LIBRARY_DIR} ${Pistache_LIBRARY_DIR} ${Googletest_LIBRARY_DIR})

SET(CMAKE_DEBUG_POSTFIX d) 
SET (LIBRARIES_TO_IMPORT_DEBUG)
SET (LIBRARIES_TO_IMPORT_OPTIMIZED ${Openssl_LIBRARY} ${MariadbConnector_LIBRARY} ${Json11_LIBRARY} ${Pistache_LIBRARY} ${Googletest_LIBRARY})

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} )
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES_TO_IMPORT} debug ${LIBRARIES_TO_IMPORT_DEBUG} optimized ${LIBRARIES_TO_IMPORT_OPTIMIZED})

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION build COMPONENT applications)
