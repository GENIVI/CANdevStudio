set(COMPONENT_NAME canrawlogger)

set(SRC
    canrawlogger.cpp
    canrawlogger_p.cpp
    canrawloggermodel.cpp
)

add_library(${COMPONENT_NAME} ${SRC})
target_link_libraries(${COMPONENT_NAME} cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
