set(COMPONENT_NAME projectconfig)

set(SRC
    projectconfig.ui
    projectconfig.cpp    
    dragwidget.cpp
    flowviewwrapper.cpp
    projectconfigvalidator.cpp
    propertyeditordialog.cpp
    propertyeditordialog.ui
    ${CMAKE_SOURCE_DIR}/CANdevResources.qrc
)

add_library(${COMPONENT_NAME} STATIC ${SRC})
target_link_libraries(${COMPONENT_NAME} ${PLUGIN_LIBRARIES} rapidjson)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_BINARY_DIR})

if(WITH_TESTS)
    add_executable(${COMPONENT_NAME}_test tests/${COMPONENT_NAME}_test.cpp)
    target_link_libraries(${COMPONENT_NAME}_test ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME ProjectConfigTestTest COMMAND ${COMPONENT_NAME}_test)
    target_include_directories(${COMPONENT_NAME}_test INTERFACE PRIVATE ${CMAKE_BINARY_DIR})

    add_executable(${COMPONENT_NAME}_test_noqrc tests/${COMPONENT_NAME}_test_noqrc.cpp)
    target_link_libraries(${COMPONENT_NAME}_test_noqrc ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME ProjectConfigTestNoQrc COMMAND ${COMPONENT_NAME}_test_noqrc)

    set(CONFIG_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/tests/configfiles)
    set(CONFIG_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/configfiles)
    add_custom_command(TARGET ${COMPONENT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CONFIG_SOURCE} ${CONFIG_DESTINATION})
endif()
