set(COMPONENT_NAME canrawplayer)

set(SRC
    canrawplayer.cpp
    canrawplayer_p.cpp
    canrawplayermodel.cpp
)

add_library(${COMPONENT_NAME} ${SRC})
target_link_libraries(${COMPONENT_NAME} cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
