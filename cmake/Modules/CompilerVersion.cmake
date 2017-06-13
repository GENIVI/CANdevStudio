set(GCC_MINIMAL_VERSION "5.3.0")
set(CLANG_MINIMAL_VERSION "3.5.0")
set(MSVC_MINIMAL_VERSION "19.0")

set(selected_compiler_version "")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(selected_compiler_version ${GCC_MINIMAL_VERSION})
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(selected_compiler_version ${CLANG_MINIMAL_VERSION})
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(selected_compiler_version ${MSVC_MINIMAL_VERSION})
endif()

if(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS "${selected_compiler_version}")
    message(WARNING "\nWARNING: Compiler version may be too low. Minimal version of ${CMAKE_CXX_COMPILER_ID} is ${selected_compiler_version}\n")
endif()
