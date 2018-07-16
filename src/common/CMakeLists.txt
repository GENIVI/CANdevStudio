find_package(Threads REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include)

add_library(cds-common STATIC componentmodel.h)
target_include_directories(cds-common INTERFACE ${CMAKE_SOURCE_DIR}/src/common ${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include)
target_link_libraries(cds-common nodes ${CMAKE_THREAD_LIBS_INIT})
