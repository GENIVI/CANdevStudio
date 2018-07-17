set(COMPONENT_NAME canrawsender)

set(SRC
    gui/canrawsender.ui
    gui/crsgui.h
    canrawsender.cpp
    canrawsender_p.cpp
    newlinemanager.cpp
    canrawsendermodel.cpp
)

add_library(${COMPONENT_NAME} STATIC ${SRC})
target_link_libraries(${COMPONENT_NAME} cds-common)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(WITH_TESTS)
    add_executable(${COMPONENT_NAME}_test ${COMPONENT_NAME}_test.cpp)
    target_link_libraries(${COMPONENT_NAME}_test ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME CanRawSenderTest COMMAND ${COMPONENT_NAME}_test)

    add_executable(${COMPONENT_NAME}model_test ${COMPONENT_NAME}model_test.cpp)
    target_link_libraries(${COMPONENT_NAME}model_test ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME CanRawSenderModelTest COMMAND ${COMPONENT_NAME}model_test)
endif()
