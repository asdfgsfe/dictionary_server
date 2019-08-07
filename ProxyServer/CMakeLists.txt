project(proxy_server C CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)

set(CXX_FLAGS
         -g
         -fPIC
         -Wall
         -Wno-unused-parameter
         -Wno-unused-function
         -Wunused-variable
         -Wunused-value
         -Wshadow
         -Wcast-qual
         -Wcast-align
         -Wwrite-strings
         -Wsign-compare
         -Winvalid-pch
         -fms-extensions
         -Wfloat-equal
         -Wextra
         -std=c++11
    )

include_directories(${PROJECT_SOURCE_DIR})

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)
add_subdirectory(DAL)
add_subdirectory(Message)
add_subdirectory(HashKit)
aux_source_directory(. SRC_LIST)
#include_directories(${PROJECT_SOURCE_DIR}/3rdparty/muduo/include)
#link_directories(${PROJECT_SOURCE_DIR}/3rdparty/muduo/lib)

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}
                      ${PROTOBUF_LIBRARY}
                      query
                      dal
                      yaml-cpp
                      hiredis
                      muduo_base
                      muduo_net
                      z #zlib
                      pthread)

