set(COMPONENT_NAME candevice)

set(SRC
    candevice.cpp
    candevicemodel.cpp
)

add_library(${COMPONENT_NAME} STATIC ${SRC})
target_link_libraries(${COMPONENT_NAME} cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

