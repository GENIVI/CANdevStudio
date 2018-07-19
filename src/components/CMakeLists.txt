
macro(dirlist result currentDir)
    file(GLOB subdirs RELATIVE ${currentDir} ${currentDir}/*)
    set(dirlist "")
    foreach(entry ${subdirs})
        set(currentSubDir "${CMAKE_CURRENT_SOURCE_DIR}/${entry}" CACHE STRING "" FORCE)
        if(IS_DIRECTORY ${currentSubDir} AND EXISTS "${currentSubDir}/CMakeLists.txt")
            list(APPEND dirlist ${entry})
        endif()
        set(currentDir "")
    endforeach()
    set(${result} ${dirlist})
endmacro()

macro(plugin_list result)
    file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
    foreach(entry ${subdirs})
        set(currentSubDir "${CMAKE_CURRENT_SOURCE_DIR}/${entry}" CACHE STRING "" FORCE)
        if(IS_DIRECTORY ${currentSubDir} AND EXISTS "${currentSubDir}/${entry}plugin.h")
            list(APPEND ${result} "${entry}")
        endif()
    endforeach()
endmacro()

macro(get_plugin_type plugin result section)
    set(pluginName "" CACHE STRING "" FORCE)
    file(STRINGS ${plugin}/${plugin}plugin.h 
        test 
        REGEX "^(struct)|(class).*"
    )
    string(REPLACE "struct" "" test ${test})
    string(REPLACE "class" "" test ${test})
    string(REPLACE "{" "" test ${test})
    string(STRIP ${test} test)
    message(STATUS "Found Plugin: ${test}")
    set(${result} ${test})

    file(STRINGS ${plugin}/${plugin}plugin.h 
        secTest
        REGEX "PluginType"
    )

    string(REPLACE "PluginType" "" secTest ${secTest})
    string(REPLACE "using" "" secTest ${secTest})
    string(REPLACE "=" "" secTest ${secTest})
    string(REPLACE ";" "" secTest ${secTest})
    string(STRIP ${secTest} secTest)
    set(${section} ${secTest})
endmacro()

plugin_list(plugins)
set(PLUGIN_LIBRARIES ${plugins} CACHE INTERNAL "Libraries to link against for plugins")
set(PLUGINS_IMPL "")
set(PLUGIN_INCLUDES "")
foreach(component ${plugins})
    set(PLUGIN_INCLUDES "${PLUGIN_INCLUDES}#include <${CMAKE_CURRENT_SOURCE_DIR}/${component}/${component}plugin.h>\n")
    get_plugin_type(${component} pluginType sectionType)
    list(APPEND PLUGINS_IMPL ${pluginType})
    list(APPEND SECTIONS ${sectionType})
endforeach()

list(REMOVE_DUPLICATES SECTIONS)
message(STATUS "Sections: ${SECTIONS}")

set(PLUGINS_IMPL_MSG "")
foreach(plugin_str ${PLUGINS_IMPL})
    set(PLUGINS_IMPL_MSG "${PLUGINS_IMPL_MSG}${plugin_str}, ")
endforeach()
string(STRIP ${PLUGINS_IMPL_MSG} PLUGINS_IMPL_MSG)
string(REGEX REPLACE "[,].?$" ""  PLUGINS_IMPL_STR ${PLUGINS_IMPL_MSG})

set(SECTIONS_IMPL "")
foreach(sections_str ${SECTIONS})
    set(SECTIONS_IMPL "${SECTIONS_IMPL}${sections_str}, ")
endforeach()
string(REGEX REPLACE "[,].?$" ""  SECTIONS_IMPL ${SECTIONS_IMPL})

configure_file(${CMAKE_SOURCE_DIR}/src/common/plugins.hpp.in
    ${CMAKE_BINARY_DIR}/plugins.hpp)

dirlist(components ${CMAKE_CURRENT_SOURCE_DIR})
foreach(component ${components})
    add_subdirectory(${component})
endforeach()
