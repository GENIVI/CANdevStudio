cmake_minimum_required(VERSION 3.0.0)

project(CanDevStudio CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)

add_definitions(-Wall -Wextra)

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(src/gui)
