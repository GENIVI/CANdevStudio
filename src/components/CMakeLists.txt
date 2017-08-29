function(add_component componentName componentModelName)
	add_library(${componentName} ${componentName}/${componentName}.cpp ${componentName}/${componentModelName}.cpp)
    target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
    target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(add_component_ui componentName componentModelName)
	add_library(${componentName} ${componentName}/${componentName}.cpp ${componentName}/${componentModelName}.cpp ${componentName}/${componentName}.ui ${componentName}/uniquefiltermodel.cpp)
    target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
    target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

set(CANRAWSENDER_FILES
    canrawsender/canrawsender.ui
    canrawsender/canrawsender.cpp
    canrawsender/canrawsender_p.cpp
    canrawsender/canrawsendermodel.cpp
    canrawsender/newlinemanager.cpp
)

function(add_component_ui_dir componentName srcfiles)
    add_library(${componentName} ${srcfiles})
    target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
    target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()



add_component(candevice candevicemodel)
add_component_ui(canrawview canrawviewmodel)
add_component_ui_dir(canrawsender "${CANRAWSENDER_FILES}")


