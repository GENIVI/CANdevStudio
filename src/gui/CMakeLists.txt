include_directories(${CMAKE_SOURCE_DIR}/src/components)

set(srcs
    main.cpp
    mainwindow.cpp
)

add_executable(CanDevStudio ${srcs})
target_link_libraries(CanDevStudio Qt5::Widgets candevice canrawview cansignalcoder)
