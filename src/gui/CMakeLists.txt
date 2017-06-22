

set(srcs
    main.cpp
    mainwindow.cpp
)

add_executable(CanDevStudio ${srcs})
target_link_libraries(CanDevStudio Qt5::Widgets candevice canrawview cansignalview canrawsender cansignalsender cds-common)
target_compile_definitions(CanDevStudio PRIVATE $<$<CONFIG:Debug>:CDS_DEBUG=true> $<$<NOT:$<CONFIG:Debug>>:CDS_DEBUG=false>)
