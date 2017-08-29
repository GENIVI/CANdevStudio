#function(add_component componentName componentModelName)
	#add_library(${componentName} ${componentName}/${componentName}.cpp ${componentName}/${componentModelName}.cpp)
    #target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
    #target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#endfunction()

#function(add_component_ui componentName componentModelName)
	#add_library(${componentName} ${componentName}/${componentName}.cpp ${componentName}/${componentModelName}.cpp ${componentName}/${componentName}.ui ${componentName}/uniquefiltermodel.cpp)
    #target_link_libraries(${componentName} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common)
    #target_include_directories(${componentName} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
#endfunction()

#add_component(candevice candevicemodel)
#add_component_ui(canrawview canrawviewmodel)

add_subdirectory(candevice)
add_subdirectory(canrawsender)
add_subdirectory(canrawview)
