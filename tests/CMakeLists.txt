include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/single_header/catch)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/config/catch)
include_directories(${CMAKE_SOURCE_DIR}/src/components)

add_executable(candevice_test candevicetest.cpp candeviceqt_test.cpp)
target_link_libraries(candevice_test candevice Qt5::Core Qt5::SerialBus Qt5::Test cds-common)
target_compile_options(candevice_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanDeviceTest COMMAND candevice_test)

add_executable(canrawsender_test newlinemanager_test.cpp canrawsender_test.cpp)
target_link_libraries(canrawsender_test canrawsender Qt5::Core Qt5::SerialBus Qt5::Test cds-common)
target_compile_options(canrawsender_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawSenderTest COMMAND canrawsender_test)

add_executable(canrawsendermodel_test canrawsendermodel_test.cpp)
target_link_libraries(canrawsendermodel_test canrawsender Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(canrawsendermodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawSenderModelTest COMMAND canrawsendermodel_test)

add_executable(canrawview_test canrawview_test.cpp)
target_link_libraries(canrawview_test canrawview Qt5::Core Qt5::SerialBus Qt5::Test cds-common)
target_compile_options(canrawview_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawViewTest COMMAND canrawview_test)

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

add_executable(projectconfig_test projectconfig_test.cpp)
target_link_libraries(projectconfig_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(projectconfig_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME ProjectConfigTest COMMAND projectconfig_test)

add_executable(projectconfig_test_noqrc projectconfig_test_noqrc.cpp)
target_link_libraries(projectconfig_test_noqrc candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(projectconfig_test_noqrc PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME ProjectConfigTest_NoQrc COMMAND projectconfig_test_noqrc)

set(CONFIG_SOURCE ${PROJECT_SOURCE_DIR}/tests/configfiles)
set(CONFIG_DESTINATION ${PROJECT_BINARY_DIR}/tests/configfiles)
add_custom_command(TARGET canrawsender_test canrawviewmodel_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CONFIG_DESTINATION}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CONFIG_SOURCE} ${CONFIG_DESTINATION})

add_executable(canrawplayer_test canrawplayer_test.cpp)
target_link_libraries(canrawplayer_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig canrawplayer)
target_compile_options(canrawplayer_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawlayerTest COMMAND canrawplayer_test)

add_executable(canrawplayermodel_test canrawplayermodel_test.cpp)
target_link_libraries(canrawplayermodel_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(canrawplayermodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawPlayerModelTest COMMAND canrawplayermodel_test)

add_executable(canrawlogger_test canrawlogger_test.cpp)
target_link_libraries(canrawlogger_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig canrawplayer)
target_compile_options(canrawlogger_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawLoggerTest COMMAND canrawlogger_test)

add_executable(canrawloggermodel_test canrawloggermodel_test.cpp)
target_link_libraries(canrawloggermodel_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(canrawloggermodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawLoggerModelTest COMMAND canrawloggermodel_test)

add_executable(canload_test canload_test.cpp)
target_link_libraries(canload_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig canrawplayer)
target_compile_options(canload_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanLoadTest COMMAND canload_test)

add_executable(canloadmodel_test canloadmodel_test.cpp)
target_link_libraries(canloadmodel_test candevice Qt5::Core Qt5::SerialBus Qt5::Test nodes cds-common projectconfig)
target_compile_options(canloadmodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanLoadModelTest COMMAND canloadmodel_test)
