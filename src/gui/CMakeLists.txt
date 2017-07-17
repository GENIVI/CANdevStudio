

set(srcs
    main.cpp
    mainwindow.cpp
    toolbar/cdstoolbar.cpp
)

add_executable(CANdevStudio ${srcs})
target_link_libraries(CANdevStudio Qt5::Widgets candevice canrawview cansignalview canrawsender cansignalsender cds-common)
target_compile_definitions(CANdevStudio PRIVATE $<$<CONFIG:Debug>:CDS_DEBUG=true> $<$<NOT:$<CONFIG:Debug>>:CDS_DEBUG=false>)
