
set(SRC
    canrawview.cpp
)

add_library(canrawview ${SRC})
target_link_libraries(canrawview Qt5::Widgets Qt5::Core Qt5::SerialBus)

