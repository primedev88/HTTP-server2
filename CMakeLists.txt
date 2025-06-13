cmake_minimum_required(VERSION 3.22)
project(HTTP_server2)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(HTTP_server2 ${SOURCES})

