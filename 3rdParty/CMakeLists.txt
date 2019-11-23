
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
target_compile_definitions(rapidjson INTERFACE RAPIDJSON_HAS_STDSTRING=1 RAPIDJSON_HAS_CXX11_RVALUE_REFS=1)

add_library(fakeit INTERFACE)
target_include_directories(fakeit INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fakeit/single_header/catch ${CMAKE_CURRENT_SOURCE_DIR}/fakeit/config/catch ${CMAKE_CURRENT_SOURCE_DIR}/catch/single_include/catch2)
target_compile_options(fakeit INTERFACE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize -Wno-cast-function-type>)

add_library(rwqueue INTERFACE)
target_include_directories(rwqueue INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/readerwriterqueue)

add_subdirectory(nodeeditor)


add_subdirectory(CANdb EXCLUDE_FROM_ALL)

if(UNIX AND NOT APPLE)
    add_subdirectory(qtCannelloniCanBus)

    install(PROGRAMS ${CMAKE_BINARY_DIR}/3rdParty/qtCannelloniCanBus/libqtCannelloniCanBusPlugin.so 
        DESTINATION ${INSTALL_DEST_PLUG}
    )
endif()
