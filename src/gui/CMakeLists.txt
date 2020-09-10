add_subdirectory(projectconfig)

set(srcs
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    toolbar.ui
    aboutdialog.ui
)

add_executable(CANdevStudio MACOSX_BUNDLE ${GUI_TYPE} ${srcs})
target_link_libraries(CANdevStudio projectconfig Qt5::Quick  Qt5::Widgets Qt5::Qml Qt5::QuickWidgets)
target_compile_definitions(CANdevStudio PRIVATE $<$<CONFIG:Debug>:CDS_DEBUG=true> $<$<NOT:$<CONFIG:Debug>>:CDS_DEBUG=false>)
target_include_directories(CANdevStudio 
    PRIVATE
        "${CMAKE_SOURCE_DIR}/components/"
        "${CMAKE_BINARY_DIR}/generated"
)
install(TARGETS CANdevStudio 
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION ${INSTALL_DEST_BIN} COMPONENT Runtime
)
