include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/single_header/catch)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/config/catch)
include_directories(${CMAKE_SOURCE_DIR}/src/components)

add_executable(candevice_test candevicetest.cpp candeviceqt_test.cpp)
target_link_libraries(candevice_test candevice Qt5::Core Qt5::SerialBus Qt5::Test cds-common)
target_compile_options(candevice_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanDeviceTest COMMAND candevice_test)

#add_executable(canrawsender_test newlinemanager_test.cpp canrawsender_test.cpp)
#target_link_libraries(canrawsender_test canrawsender Qt5::Core Qt5::SerialBus Qt5::Test cds-common)
#target_compile_options(canrawsender_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
#add_test( NAME CanRawSenderTest COMMAND canrawsender_test)

#add_executable(canrawsendermodel_test canrawsendermodel_test.cpp)
#target_link_libraries(canrawsendermodel_test canrawsender Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
#target_compile_options(canrawsendermodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
#add_test( NAME CanRawSenderModelTest COMMAND canrawsendermodel_test)

add_executable(canrawviewmodel_test canrawviewmodel_test.cpp)
target_link_libraries(canrawviewmodel_test canrawview Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(canrawviewmodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawViewModelTest COMMAND canrawviewmodel_test)

add_executable(candevicemodel_test candevicemodel_test.cpp)
target_link_libraries(candevicemodel_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(candevicemodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanDeviceModelTest COMMAND candevicemodel_test)

add_executable(common_test common_test.cpp)
target_link_libraries(common_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(common_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CommonTest COMMAND common_test)


set(CONFIG_SOURCE ${PROJECT_SOURCE_DIR}/tests/configfiles)
set(CONFIG_DESTINATION ${PROJECT_BINARY_DIR}/tests/configfiles)
add_custom_command(TARGET canrawsender_test canrawviewmodel_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CONFIG_DESTINATION}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CONFIG_SOURCE} ${CONFIG_DESTINATION})
