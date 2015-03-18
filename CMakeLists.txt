cmake_minimum_required(VERSION 2.8.4)
project(Emc__)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set(SOURCE_FILES main.cpp)
add_executable(Emc__ ${SOURCE_FILES})
