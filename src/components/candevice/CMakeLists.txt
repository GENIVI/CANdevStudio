set(COMPONENT_NAME candevice)

set(SRC
    candevice.cpp
    candevicemodel.cpp
)

add_library(${COMPONENT_NAME} STATIC ${SRC})
target_link_libraries(${COMPONENT_NAME} cds-common rwqueue)
target_include_directories(${COMPONENT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(WITH_TESTS)
    set(SRC ${COMPONENT_NAME}_test.cpp)

    if(LINUX)
        list(APPEND candeviceqt_test.cpp)
    endif()

    add_executable(${COMPONENT_NAME}_test ${SRC})
    target_link_libraries(${COMPONENT_NAME}_test ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME CanDeviceTest COMMAND ${COMPONENT_NAME}_test)

    add_executable(${COMPONENT_NAME}model_test ${COMPONENT_NAME}model_test.cpp)
    target_link_libraries(${COMPONENT_NAME}model_test ${COMPONENT_NAME} Qt5::Test fakeit)
    add_test(NAME CanDeviceModelTest COMMAND ${COMPONENT_NAME}model_test)
endif()
