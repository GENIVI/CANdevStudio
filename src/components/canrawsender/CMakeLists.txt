set(COMPONENT_NAME canrawsender)

set(SRC
    gui/canrawsender.ui
    gui/crsgui.h
    canrawsender.cpp
    canrawsender_p.cpp
    newlinemanager.cpp
    canrawsendermodel.cpp
)

add_library(${COMPONENT_NAME} STATIC ${SRC})
target_link_libraries(${COMPONENT_NAME} Qt5::Widgets Qt5::Core Qt5::SerialBus cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

