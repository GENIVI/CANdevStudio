function(add_component componentName)
    add_library(${componentName} ${componentName}/${componentName}.cpp)
    target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus)
    target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

add_component(candevice)
add_component(canrawview)
add_component(canrawsender)
add_component(cansignalview)
add_component(cansignalsender)

