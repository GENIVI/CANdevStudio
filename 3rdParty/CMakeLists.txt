
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
target_compile_definitions(rapidjson INTERFACE RAPIDJSON_HAS_STDSTRING=1 RAPIDJSON_HAS_CXX11_RVALUE_REFS=1)

add_subdirectory(nodeeditor)

if(WITH_TESTS OR WITH_COVERAGE)
    add_subdirectory(fakeit)
endif()

if(LINUX)
    add_subdirectory(3rdParty/qtCannelloniCanBus)

    install(PROGRAMS ${CMAKE_BINARY_DIR}/3rdParty/qtCannelloniCanBus/libqtCannelloniCanBusPlugin.so 
        DESTINATION ${INSTALL_DEST_PLUG}
    )
endif()
