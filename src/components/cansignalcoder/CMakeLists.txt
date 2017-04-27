
set(SRC
    cansignalcoder.cpp
    can-signals/genivi_demo_signals.c
)

add_library(cansignalcoder SHARED ${SRC})
target_link_libraries(cansignalcoder Qt5::Widgets Qt5::Core Qt5::SerialBus)

