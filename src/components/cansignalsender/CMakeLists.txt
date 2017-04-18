
set(SRC
    cansignalsender.cpp
)

add_library(cansignalsender SHARED ${SRC})
target_link_libraries(cansignalsender Qt5::Widgets Qt5::Core Qt5::SerialBus)

