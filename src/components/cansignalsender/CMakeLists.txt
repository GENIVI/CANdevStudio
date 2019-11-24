set(COMPONENT_NAME cansignalsender)

set(SRC
    gui/cansignalsender.ui
    gui/cansignalsenderguiimpl.h
    cansignalsender.cpp
    cansignalsender_p.cpp
    cansignalsendermodel.cpp
)

add_library(${COMPONENT_NAME} ${SRC})
target_link_libraries(${COMPONENT_NAME} cds-common CANdb rwqueue)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(WITH_TESTS)
    add_executable(${COMPONENT_NAME}_test tests/${COMPONENT_NAME}_test.cpp)
    target_link_libraries(${COMPONENT_NAME}_test ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME ${COMPONENT_NAME}_test COMMAND ${COMPONENT_NAME}_test)

    add_executable(${COMPONENT_NAME}model_test tests/${COMPONENT_NAME}model_test.cpp)
    target_link_libraries(${COMPONENT_NAME}model_test ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME ${COMPONENT_NAME}model_test COMMAND ${COMPONENT_NAME}model_test)
endif()
