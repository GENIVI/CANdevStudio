
set(SRC
    cansignalview.cpp
)

add_library(cansignalview SHARED ${SRC})
target_link_libraries(cansignalview Qt5::Widgets Qt5::Core Qt5::SerialBus)

