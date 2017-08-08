

set(srcs
    CanDeviceManager.cpp
    main.cpp
    mainwindow.cpp
    mainwindow.ui
)

add_executable(CANdevStudio ${srcs})
target_link_libraries(CANdevStudio Qt5::Widgets candevice canrawview canrawsender cds-common)
target_compile_definitions(CANdevStudio PRIVATE $<$<CONFIG:Debug>:CDS_DEBUG=true> $<$<NOT:$<CONFIG:Debug>>:CDS_DEBUG=false>)
