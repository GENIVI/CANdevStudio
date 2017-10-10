set(COMPONENT_NAME projectconfig)

set(SRC
    projectconfig.ui
    projectconfig.cpp    
    canrawviewmodel.cpp
    canrawsendermodel.cpp
    candevicemodel.cpp
    dragwidget.cpp
    flowviewwrapper.cpp
	../../../CANdevResources.qrc
    configmgr.cpp
    palmgr.cpp
)

add_library(${COMPONENT_NAME} ${SRC})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(${COMPONENT_NAME} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes candevice canrawview canrawsender propertyeditor cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(TARGET ${COMPONENT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/config.cds ${CMAKE_CURRENT_BINARY_DIR}/config.cds)
