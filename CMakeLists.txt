cmake_minimum_required(VERSION 2.8.4)
project(Emc__)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

find_package(Boost 1.57.0 COMPONENTS system filesystem REQUIRED)

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif()

add_executable(BoostTest main.cpp)

if(Boost_FOUND)
    set(SOURCE_FILES main.cpp)
    add_executable(Emc__ ${SOURCE_FILES} pretty_printer.h)
endif()