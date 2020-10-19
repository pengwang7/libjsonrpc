cmake_minimum_required(VERSION 3.9.6)
project(jsonrpc C CXX)

include(FindThreads)
#include(FindProtobuf)

#protobuf_generate_cpp(PROTO_SRC PROTO_HEADER rpc.proto echo_server.proto)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(
    /home/pengwang/cloud_platform/framework/3rd
    /home/pengwang/cloud_platform/project/github/libjsonrpc/src/jsonrpc/server
)
#-O0 -pg -pipe -Wall -g -W
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -O2 -pg -pipe -Wall -W -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -lpthread -luuid -lgflags -lcurl -lprotoc -lssl -lcrypto -lglog -ldl -lz")
set(CMAKE_C_FLAGS   "${CMAKE_CPP_FLAGS} -O2 -pg -fsanitize=address -pipe -Wall -W -fPIC -fstrict-aliasing -Wno-unused-parameter -fno-omit-frame-pointer")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

macro(use_cxx11)
if(CMAKE_VERSION VERSION_LESS "3.1.3")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif()
else()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
endmacro(use_cxx11)

use_cxx11()

set(BUTIL_SOURCES
    ${PROJECT_SOURCE_DIR}/src/jsonrpc/server/connection.cpp
    ${PROJECT_SOURCE_DIR}/src/jsonrpc/server/connection_manager.cpp
    ${PROJECT_SOURCE_DIR}/src/jsonrpc/server/io_service_pool.cpp
    ${PROJECT_SOURCE_DIR}/src/jsonrpc/server/transport_server.cpp
    ${PROJECT_SOURCE_DIR}/examples/example.cpp
)

set(DYNAMIC_LIB
    ${GFLAGS_LIBRARY}
    #${PROTOBUF_LIBRARIES}
    #${PROJECT_SOURCE_DIR}/examples/test.cpp
)

set(DYNAMIC_LIB
    ${GFLAGS_LIBRARY}
    #${PROTOBUF_LIBRARIES}
    #${SSL_LIB}
    #${CRYPTO_LIB}
    #${THRIFT_LIB}
    #${THRIFTNB_LIB}
    #dl
)

#add_executable(${PROJECT_NAME} ${BUTIL_SOURCES} ${PROTO_SRC})
add_executable(${PROJECT_NAME} ${BUTIL_SOURCES})
target_link_libraries(jsonrpc /home/pengwang/cloud_platform/target/3rd/jsoncpp/libjsoncpp.a)
#target_link_libraries(libatp /home/pengwang/cloud_platform/target/3rd/libevent/libevent.a)
#target_link_libraries(libatp /home/pengwang/cloud_platform/target/3rd/libevent/libevent_openssl.a)
target_link_libraries(jsonrpc /home/pengwang/cloud_platform/target/3rd/gflags/libgflags.so)
target_link_libraries(jsonrpc /home/pengwang/cloud_platform/target/3rd/glog/libglog.a)
target_link_libraries(jsonrpc ${DYNAMIC_LIB})

INSTALL(FILES ${HEADERS} DESTINATION ${PROJECT_BINARY_DIR}/)
