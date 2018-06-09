

set(srcs
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    toolbar.ui
    aboutdialog.ui
)


add_definitions(-DRAPIDJSON_HAS_STDSTRING=1 -DRAPIDJSON_HAS_CXX11_RVALUE_REFS=1)

add_executable(CANdevStudio ${GUI_TYPE} ${srcs})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../components/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/rapidjson/include")
target_link_libraries(CANdevStudio Qt5::Widgets candevice canrawview canrawsender cds-common nodes projectconfig)
target_compile_definitions(CANdevStudio PRIVATE $<$<CONFIG:Debug>:CDS_DEBUG=true> $<$<NOT:$<CONFIG:Debug>>:CDS_DEBUG=false>)

install(TARGETS CANdevStudio RUNTIME DESTINATION ${INSTALL_DEST_BIN})
