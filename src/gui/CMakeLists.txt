
find_package(Qt5Widgets REQUIRED)

set(srcs
    main.cpp
)

add_executable(CanDevStudio ${srcs})
target_link_libraries(CanDevStudio Qt5::Widgets)
