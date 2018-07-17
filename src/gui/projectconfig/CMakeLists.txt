set(COMPONENT_NAME projectconfig)

set(SRC
    projectconfig.ui
    projectconfig.cpp    
    dragwidget.cpp
    flowviewwrapper.cpp
    projectconfigvalidator.cpp
    propertyeditordialog.cpp
    propertyeditordialog.ui
    ${CMAKE_SOURCE_DIR}/CANdevResources.qrc
)

add_library(${COMPONENT_NAME} STATIC ${SRC})
target_link_libraries(${COMPONENT_NAME} ${PLUGIN_LIBRARIES} rapidjson)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_BINARY_DIR})

