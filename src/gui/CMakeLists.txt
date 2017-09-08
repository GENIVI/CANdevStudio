

set(srcs
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    PropertyModel.cpp
    PropertyEditor.cpp
)

add_executable(CANdevStudio ${srcs})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../components/")
target_link_libraries(CANdevStudio Qt5::Widgets candevice canrawview canrawsender cds-common nodes projectconfiguration)
target_compile_definitions(CANdevStudio PRIVATE $<$<CONFIG:Debug>:CDS_DEBUG=true> $<$<NOT:$<CONFIG:Debug>>:CDS_DEBUG=false>)
