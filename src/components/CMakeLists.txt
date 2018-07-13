
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

dirlist(components ${CMAKE_CURRENT_SOURCE_DIR})

foreach(component ${components})
    add_subdirectory(${component})
endforeach()
