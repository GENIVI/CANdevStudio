cmake_minimum_required(VERSION 3.0.0)

project(CanDevStudio CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

option(WITH_COVERAGE OFF)

find_package(Qt5Core REQUIRED)

add_definitions(-Wall -Wextra)

if(WITH_COVERAGE)
    set(CMAKE_CXX_FLAGS "-g -O0 -Wwrite-strings -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS="-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS="-fprofile-arcs -ftest-coverage")
endif()

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(src/gui)

enable_testing()
add_subdirectory(3rdParty/fakeit)
add_subdirectory(tests)
