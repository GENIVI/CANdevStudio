
set(SRC
    canscripter.cpp
)

add_library(canscripter SHARED ${SRC})
target_link_libraries(canscripter Qt5::Widgets Qt5::Core Qt5::SerialBus)

