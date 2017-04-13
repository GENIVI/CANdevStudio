
set(SRC
    candevice.cpp
)

add_library(candevice SHARED ${SRC})
target_link_libraries(candevice Qt5::Core Qt5::SerialBus)

