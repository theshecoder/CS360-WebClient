cmake_minimum_required(VERSION 3.3)
project(WebClient)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#server
set(SERVER_FILES server.cpp)
add_executable(server ${SERVER_FILES})

#client
set(DOWNLOAD_FILES download.cpp)
add_executable(download ${DOWNLOAD_FILES})