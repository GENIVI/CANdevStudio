cmake_minimum_required(VERSION 3.0.0)

project(CANdevStudio CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

option(WITH_COVERAGE "Build with coverage" OFF)
option(WITH_TESTS "Build with test" ON)

set(QT_REQUIRED_VERSION "5.8")
find_package(Qt5Core ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5SerialBus ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5Widgets ${QT_REQUIRED_VERSION} REQUIRED)

set_property(TARGET Qt5::SerialBus PROPERTY INTERFACE_COMPILE_FEATURES "")
set_property(TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_FEATURES "")

if(NOT MSVC)
    add_definitions(-Wall -Wextra)
endif()

include(CompilerVersion)

if(WITH_COVERAGE)
    set(CMAKE_CXX_FLAGS "-g -O0 -Wwrite-strings -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS="-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS="-fprofile-arcs -ftest-coverage")
endif()

set(SPDLOG_BUILD_TESTING CACHE FORCE OFF)
add_subdirectory(3rdParty/spdlog)
add_subdirectory(src/common)
add_subdirectory(src/gui)
add_subdirectory(src/components)

if(WITH_TESTS OR WITH_COVERAGE)
    enable_testing()
    add_subdirectory(3rdParty/fakeit)
    add_subdirectory(tests)
endif()
