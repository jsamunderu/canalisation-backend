cmake_minimum_required(VERSION 3.5.1)

project(login C CXX)

include(cmake/common.cmake)



SET(3RD_ROOT "${PROJECT_SOURCE_DIR}/../../3rd")

SET(Boost_ROOT "${3RD_ROOT}/boost_1_78_0")
SET(MariadbConnector_ROOT "${3RD_ROOT}/mariadb-connector-c-3.2.5-src")
SET(LIB_MARIADB_ACCESS_ROOT "${PROJECT_SOURCE_DIR}/../../lib-mariadb-access")
SET(GSL_ROOT "${3RD_ROOT}/GSL-3.1.0")

SET(Boost_INCLUDE "${Boost_ROOT}")
message("@@@@@@@@@@@@@@@@@@@@@@ ${3RD_ROOT}")
SET(MariadbConnector_INCLUDE "${MariadbConnector_ROOT}/include")
SET(LIB_MARIADB_ACCESS_INCLUDE "${LIB_MARIADB_ACCESS_ROOT}/include")
SET(GSL_INCLUDE "${GSL_ROOT}/include")

SET(Boost_LIBRARY_DIR "${Boost_ROOT}/stage/lib")
SET(MariadbConnector_LIBRARY_DIR "${MariadbConnector_ROOT}/libmariadb/")

SET(Boost_LIBRARY "libboost_log.so")
SET(MariadbConnector_LIBRARY "libmariadb.so")
SET(LIB_MARIADB_ACCESS_LIBRARY "${PROJECT_SOURCE_DIR}/../../build/bin/libmariadb_access.so")



get_filename_component(hw_proto "proto/login.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/login.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/login.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/login.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/login.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

FILE(GLOB Hdrs include/*.hpp)
FILE(GLOB Impl src/*.cpp)
SET (SOURCES ${Hdrs} ${Impl})
include_directories("${CMAKE_CURRENT_BINARY_DIR}" "include"
	${Boost_INCLUDE} ${LIB_MARIADB_ACCESS_INCLUDE}
	${MariadbConnector_INCLUDE} ${GSL_INCLUDE})

add_library(hw_grpc_proto
  ${hw_grpc_srcs}
  ${hw_grpc_hdrs}
  ${hw_proto_srcs}
  ${hw_proto_hdrs}
  ${Hdrs} ${Impl}
  include
  ${Boost_LIBRARY_DIR}
  ${MariadbConnector_LIBRARY_DIR})

target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

foreach(_target
  login_service_grpc)
  ADD_LIBRARY(${_target} SHARED ${SOURCES})
  target_link_libraries(${_target}
    hw_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    ${Boost_LIBRARY}
    ${MariadbConnector_LIBRARY}
    ${LIB_MARIADB_ACCESS_LIBRARY} -pthread)
    TARGET_COMPILE_FEATURES(${_target} PRIVATE cxx_std_17)
endforeach()
