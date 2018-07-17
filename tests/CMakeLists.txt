include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/single_header/catch)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/rapidjson/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/config/catch)
include_directories(${CMAKE_SOURCE_DIR}/src/components)

add_executable(canrawview_test canrawview_test.cpp)
target_link_libraries(canrawview_test canrawview Qt5::Test)
target_compile_options(canrawview_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawViewTest COMMAND canrawview_test)

add_executable(canrawviewmodel_test canrawviewmodel_test.cpp)
target_link_libraries(canrawviewmodel_test canrawview Qt5::Test)
target_compile_options(canrawviewmodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawViewModelTest COMMAND canrawviewmodel_test)

add_executable(common_test common_test.cpp)
target_link_libraries(common_test Qt5::Test cds-common projectconfig)
target_compile_options(common_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CommonTest COMMAND common_test)

add_executable(projectconfig_test projectconfig_test.cpp)
target_link_libraries(projectconfig_test Qt5::Test projectconfig)
target_compile_options(projectconfig_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME ProjectConfigTest COMMAND projectconfig_test)

add_executable(projectconfig_test_noqrc projectconfig_test_noqrc.cpp)
target_link_libraries(projectconfig_test_noqrc Qt5::Test cds-common projectconfig)
target_compile_options(projectconfig_test_noqrc PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME ProjectConfigTest_NoQrc COMMAND projectconfig_test_noqrc)

set(CONFIG_SOURCE ${PROJECT_SOURCE_DIR}/tests/configfiles)
set(CONFIG_DESTINATION ${PROJECT_BINARY_DIR}/tests/configfiles)
add_custom_command(TARGET canrawsender_test canrawviewmodel_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CONFIG_DESTINATION}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CONFIG_SOURCE} ${CONFIG_DESTINATION})

add_executable(canrawplayer_test canrawplayer_test.cpp)
target_link_libraries(canrawplayer_test canrawplayer Qt5::Test)
target_compile_options(canrawplayer_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawlayerTest COMMAND canrawplayer_test)

add_executable(canrawplayermodel_test canrawplayermodel_test.cpp)
target_link_libraries(canrawplayermodel_test canrawplayer Qt5::Test)
target_compile_options(canrawplayermodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawPlayerModelTest COMMAND canrawplayermodel_test)

add_executable(canrawlogger_test canrawlogger_test.cpp)
target_link_libraries(canrawlogger_test canrawlogger Qt5::Test)
target_compile_options(canrawlogger_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawLoggerTest COMMAND canrawlogger_test)

add_executable(canrawloggermodel_test canrawloggermodel_test.cpp)
target_link_libraries(canrawloggermodel_test canrawlogger Qt5::Test)
target_compile_options(canrawloggermodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawLoggerModelTest COMMAND canrawloggermodel_test)

add_executable(canrawfilter_test canrawfilter_test.cpp)
target_link_libraries(canrawfilter_test canrawfilter Qt5::Test)
target_compile_options(canrawfilter_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawFilterTest COMMAND canrawfilter_test)

add_executable(canrawfiltermodel_test canrawfiltermodel_test.cpp)
target_link_libraries(canrawfiltermodel_test canrawfilter Qt5::Test)
target_compile_options(canrawfiltermodel_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawFilterModelTest COMMAND canrawfiltermodel_test)
