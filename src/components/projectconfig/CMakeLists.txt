set(COMPONENT_NAME projectconfig)

set(SRC
    projectconfig.ui
    projectconfig.cpp    
    canrawviewmodel.cpp
    canrawsendermodel.cpp
    candevicemodel.cpp
    canrawplayermodel.cpp
    canrawloggermodel.cpp
    canloadmodel.cpp
    canrawfiltermodel.cpp
    dragwidget.cpp
    flowviewwrapper.cpp
    projectconfigvalidator.cpp
    propertyeditordialog.cpp
    propertyeditordialog.ui
	../../../CANdevResources.qrc
)

add_library(${COMPONENT_NAME} STATIC ${SRC})
target_link_libraries(${COMPONENT_NAME} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes candevice canrawview canrawsender cds-common canrawplayer canrawlogger canload canrawfilter rapidjson)
target_include_directories(${COMPONENT_NAME} 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
