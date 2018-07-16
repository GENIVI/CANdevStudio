set(COMPONENT_NAME canrawview)

set(SRC
    gui/canrawview.ui
    gui/crvgui.h
    canrawview.cpp
    crvsortmodel.cpp    
    canrawviewmodel.cpp
)

add_library(${COMPONENT_NAME} STATIC ${SRC})
target_link_libraries(${COMPONENT_NAME} cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

