
set(SRC
    cansignalcoder.cpp
)

add_library(cansignalcoder SHARED ${SRC})
target_link_libraries(cansignalcoder Qt5::Widgets Qt5::Core Qt5::SerialBus)

