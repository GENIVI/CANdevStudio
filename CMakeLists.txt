cmake_minimum_required(VERSION 3.0.0)

project(CANdevStudio CXX)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 6)
set(VERSION_PATCH 0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

option(WITH_COVERAGE "Build with coverage" OFF)
option(WITH_TOOLS "Build tools directory" OFF)
option(DEV_BUILD "Development build will include GIT hash and branch to versioning" ON)
option(STANDALONE "Builds self containing package including Qt libs" OFF)

if(NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif()
message(STATUS "CMAKE_INSTALL_LIBDIR: '${CMAKE_INSTALL_LIBDIR}'")

if(UNIX)
    option(WITH_TESTS "Build with test" ON)
    if(STANDALONE)
        set(INSTALL_DEST_LIB CANdevStudio)
        set(INSTALL_DEST_BIN CANdevStudio)
        set(INSTALL_DEST_PLUG CANdevStudio/canbus)
    else()
        set(INSTALL_DEST_LIB ${CMAKE_INSTALL_LIBDIR}/CANdevStudio)
        set(INSTALL_DEST_BIN bin)
        set(INSTALL_DEST_PLUG ${CMAKE_INSTALL_LIBDIR}/qt/plugins/canbus)
    endif()
else()
    option(WITH_TESTS "Build with test" OFF)

    set(GUI_TYPE WIN32)
    set(INSTALL_DEST_LIB CANdevStudio)
    set(INSTALL_DEST_BIN CANdevStudio)
endif()

set(QT_REQUIRED_VERSION "5.6")
find_package(Qt5Core ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5SerialBus ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5Widgets ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5Test ${QT_REQUIRED_VERSION} REQUIRED)
find_package(Qt5Svg ${QT_REQUIRED_VERSION} REQUIRED)
message(STATUS "Qt Version found: ${Qt5Core_VERSION}")

set_property(TARGET Qt5::SerialBus PROPERTY INTERFACE_COMPILE_FEATURES "")
set_property(TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_FEATURES "")

# Limit TYPESTRING to 128 (i.e. 2^7). 64 causes VS2015 internal error.
if(NOT MSVC)
    add_definitions(-Wall -Wextra -DUSE_TYPESTRING=7)
else()
    add_definitions( /bigobj /D USE_TYPESTRING=7)
endif()

include(CompilerVersion)

if(WITH_COVERAGE)
    include(CodeCoverage)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_COVERAGE})
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS_COVERAGE})
    setup_target_for_coverage(NAME coverage EXECUTABLE ctest DEPENDENCIES candevice_test)
endif()

if(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "")
set(BUILD_EXAMPLES OFF CACHE BOOL "")
set(CANDB__WITH_TESTS OFF CACHE BOOL "")

# Version handling
if(EXISTS "${CMAKE_SOURCE_DIR}/.git" AND DEV_BUILD)
  execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(GIT_BRANCH "")
  set(GIT_COMMIT_HASH "")
endif()

message(STATUS "Git current branch: ${GIT_BRANCH}")
message(STATUS "Git commit hash: ${GIT_COMMIT_HASH}")

message(STATUS "Generating version.h")

configure_file(
  ${CMAKE_SOURCE_DIR}/src/common/version.h.in
  ${CMAKE_BINARY_DIR}/generated/version.h
)
# end of version Handling

if(WITH_TESTS OR WITH_COVERAGE)
    enable_testing()
endif()

add_subdirectory(3rdParty)
add_subdirectory(src/common)
add_subdirectory(src/components)
add_subdirectory(src/gui)

if(WITH_TOOLS)
    find_package(Boost COMPONENTS system filesystem REQUIRED)
    add_subdirectory(tools)
endif()

if(STANDALONE)
    if(UNIX)
        set(SYS_LIBS
            libQt5Core.so.5
            libQt5Gui.so.5
            libQt5Network.so.5
            libQt5SerialBus.so.5
            libQt5SerialPort.so.5
            libQt5Svg.so.5
            libQt5Widgets.so.5
            libQt5OpenGL.so.5
            libQt5XcbQpa.so.5
            libQt5DBus.so.5
            libQt5Core.so.${Qt5Core_VERSION}
            libQt5Gui.so.${Qt5Core_VERSION}
            libQt5Network.so.${Qt5Core_VERSION}
            libQt5SerialBus.so.${Qt5Core_VERSION}
            libQt5SerialPort.so.${Qt5Core_VERSION}
            libQt5Svg.so.${Qt5Core_VERSION}
            libQt5Widgets.so.${Qt5Core_VERSION}
            libQt5OpenGL.so.${Qt5Core_VERSION}
            libQt5XcbQpa.so.${Qt5Core_VERSION}
            libQt5DBus.so.${Qt5Core_VERSION}
        )

        install(PROGRAMS files/CANdevStudio.sh DESTINATION ${INSTALL_DEST_BIN})
    else()
        set(SYS_LIBS
            libgcc_s_dw2-1.dll
            libstdc++-6.dll
            libwinpthread-1.dll
            Qt5Core.dll
            Qt5Gui.dll
            Qt5Network.dll
            Qt5SerialBus.dll
            Qt5SerialPort.dll
            Qt5Svg.dll
            Qt5Widgets.dll
        )
    endif()

    get_target_property(QtCore_location Qt5::Core LOCATION)
    get_filename_component(QtPath ${QtCore_location} DIRECTORY)

    foreach(SYS_LIB ${SYS_LIBS})
        set(SYS_LIBS_ABS ${SYS_LIBS_ABS} ${QtPath}/${SYS_LIB})
    endforeach()

    install(PROGRAMS ${SYS_LIBS_ABS}
        DESTINATION ${INSTALL_DEST_LIB}
    )

    # execute the command "qmake -query QT_INSTALL_PLUGINS" to get the path of plugins dir.
    if(NOT QT_QMAKE_EXECUTABLE)
        set(QT_QMAKE_EXECUTABLE qmake)
    endif()
    execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
        OUTPUT_VARIABLE QT_PLUGINS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(QT_PLUGINS_DIR)
        message(STATUS "Qt5 plugin directory: " "${QT_PLUGINS_DIR}")
    else()
        message(WARNING "Qt5 plugin directory cannot be detected.")
        if(WIN32)
            set(QT_PLUGINS_DIR C:/Qt/${Qt5Core_VERSION}/mingw53_32/plugins)
            message(WARNING "Setting to default Appveyor path: ${QT_PLUGINS_DIR}")
        endif()
    endif()

    install(DIRECTORY
                ${QT_PLUGINS_DIR}/platforms
                ${QT_PLUGINS_DIR}/platforminputcontexts
                ${QT_PLUGINS_DIR}/imageformats
                ${QT_PLUGINS_DIR}/canbus
                ${QT_PLUGINS_DIR}/generic
                ${QT_PLUGINS_DIR}/iconengines
            DESTINATION ${INSTALL_DEST_LIB}
            USE_SOURCE_PERMISSIONS
    )
endif()

include("packaging.cmake")
