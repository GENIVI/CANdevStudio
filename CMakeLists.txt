cmake_minimum_required(VERSION 3.0.0)

#project(CanDevStudio CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

option(WITH_COVERAGE OFF)

find_package(Qt5Core REQUIRED)
find_package(Qt5SerialBus REQUIRED)
find_package(Qt5Widgets REQUIRED)

set_property(TARGET Qt5::SerialBus PROPERTY INTERFACE_COMPILE_FEATURES "")
set_property(TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_FEATURES "")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_definitions(-Wall -Wextra -std=c++1z)

if(WITH_COVERAGE)
    set(CMAKE_CXX_FLAGS "-g -O0 -Wwrite-strings -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS="-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS="-fprofile-arcs -ftest-coverage")
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src/gui)
add_subdirectory(src/components)

enable_testing()
add_subdirectory(3rdParty/fakeit)
add_subdirectory(tests)
