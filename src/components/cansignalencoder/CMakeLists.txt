set(COMPONENT_NAME cansignalencoder)

set(SRC
    cansignalencoder.cpp
    cansignalencoder_p.cpp
    cansignalencodermodel.cpp
)

add_library(${COMPONENT_NAME} ${SRC})
target_link_libraries(${COMPONENT_NAME} Qt5::Widgets Qt5::Core Qt5::SerialBus nodes cds-common rwqueue)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(WITH_TESTS)
    add_executable(${COMPONENT_NAME}_test tests/${COMPONENT_NAME}_test.cpp)
    target_link_libraries(${COMPONENT_NAME}_test ${COMPONENT_NAME} Qt5::Test fakeit cansignaldata)
    add_test(NAME ${COMPONENT_NAME}_test COMMAND ${COMPONENT_NAME}_test)

    add_executable(${COMPONENT_NAME}model_test tests/${COMPONENT_NAME}model_test.cpp)
    target_link_libraries(${COMPONENT_NAME}model_test ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME ${COMPONENT_NAME}model_test COMMAND ${COMPONENT_NAME}model_test)

    set(DBC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/tests/dbc)
    set(DBC_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/dbc)
    add_custom_command(TARGET ${COMPONENT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DBC_SOURCE} ${DBC_DESTINATION})

    target_compile_definitions(${COMPONENT_NAME}_test PRIVATE DBC_PATH="${DBC_DESTINATION}")
    target_compile_definitions(${COMPONENT_NAME}model_test PRIVATE DBC_PATH="${DBC_DESTINATION}")
endif()
