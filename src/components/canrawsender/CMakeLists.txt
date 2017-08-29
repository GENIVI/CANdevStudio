set(COMPONENT_NAME canrawsender)

set(SRC
    canrawsender.ui
    canrawsender.cpp
    canrawsender_p.cpp
    canrawsendermodel.cpp
    newlinemanager.cpp
)

add_library(${COMPONENT_NAME} ${SRC})
target_link_libraries(${COMPONENT_NAME} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

