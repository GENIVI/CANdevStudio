set(COMPONENT_NAME canrawfilter)

set(SRC
    gui/canrawfilter.ui
    gui/canrawfilterguiimpl.h
    canrawfilter.cpp
    canrawfilter_p.cpp
    canrawfiltermodel.cpp
)

add_library(${COMPONENT_NAME} ${SRC})
target_link_libraries(${COMPONENT_NAME} cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
