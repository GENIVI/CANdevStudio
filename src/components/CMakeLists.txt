function(add_component componentName componentModelName)
	add_library(${componentName} ${componentName}/${componentName}.cpp ${componentName}/${componentModelName}.cpp)
    target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
    target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(add_component_ui componentName componentModelName)
	add_library(${componentName} ${componentName}/${componentName}.cpp ${componentName}/${componentModelName}.cpp ${componentName}/${componentName}.ui)
    target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
    target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(add_component_dir componentName)
    file(GLOB SRC_FILES ${componentName}/*.cpp)
    add_library(${componentName} ${SRC_FILES})
    target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus)
    target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

add_component(candevice)
add_component_ui(canrawview)
add_component_dir(canrawsender)


