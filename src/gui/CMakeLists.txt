

set(srcs
    main.cpp
    mainwindow.cpp
)

add_executable(CanDevStudio ${srcs})
target_link_libraries(CanDevStudio Qt5::Widgets candevice canrawview cansignalview canrawsender cansignalsender cds-common)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(CanDevStudio PRIVATE CDS_DEBUG)
endif()

