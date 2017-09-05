set(COMPONENT_NAME projectconfiguration)

set(SRC
    projectconfiguration.ui
    projectconfiguration.cpp    
)

add_library(${COMPONENT_NAME} ${SRC})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(${COMPONENT_NAME} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes candevice canrawview canrawsender cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


