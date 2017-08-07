

set(srcs
    
    mainwindow.cpp
    mainwindow.ui
)

add_library(mainwindow ${srcs})
target_link_libraries(mainwindow Qt5::Widgets candevice canrawview canrawsender cds-common)
 
add_executable(CANdevStudio main.cpp)
target_link_libraries(CANdevStudio mainwindow)
target_compile_definitions(CANdevStudio PRIVATE $<$<CONFIG:Debug>:CDS_DEBUG=true> $<$<NOT:$<CONFIG:Debug>>:CDS_DEBUG=false>)
