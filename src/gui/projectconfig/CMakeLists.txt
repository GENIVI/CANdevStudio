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

add_definitions(-DRAPIDJSON_HAS_STDSTRING=1 -DRAPIDJSON_HAS_CXX11_RVALUE_REFS=1)

message(STATUS ${PLUGIN_LIBRARIES})
add_library(${COMPONENT_NAME} STATIC ${SRC})
include_directories("${CMAKE_SOURCE_DIR}/3rdParty/rapidjson/include")
target_link_libraries(${COMPONENT_NAME} ${PLUGIN_LIBRARIES})
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_BINARY_DIR})

