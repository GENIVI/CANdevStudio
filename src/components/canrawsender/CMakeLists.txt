
set(SRC
    canrawsender.cpp
)

add_library(canrawsender SHARED ${SRC})
target_link_libraries(canrawsender Qt5::Widgets Qt5::Core Qt5::SerialBus)

