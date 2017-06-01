
set(SRC
    cansignalsender.cpp
)

add_library(cansignalsender ${SRC})
target_link_libraries(cansignalsender Qt5::Widgets Qt5::Core Qt5::SerialBus)

