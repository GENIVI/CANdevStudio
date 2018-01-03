find_package(Threads REQUIRED)

add_library(cds-common INTERFACE)
target_include_directories(cds-common INTERFACE ${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include)
target_link_libraries(cds-common INTERFACE ${CMAKE_THREAD_LIBS_INIT})
