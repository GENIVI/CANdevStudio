include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/single_header/catch)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/config/catch)
include_directories(${CMAKE_SOURCE_DIR}/src/components)

add_executable(example_test example.cpp )
target_link_libraries(example_test candevice Qt5::Core Qt5::SerialBus)
add_test( NAME ExampleTest COMMAND example_test)

add_executable(candevice_test candevicetest.cpp candeviceqt_test.cpp)
target_link_libraries(candevice_test candevice Qt5::Core Qt5::SerialBus Qt5::Test cds-common)
target_compile_options(candevice_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanDeviceTest COMMAND candevice_test)

add_executable(canrawview_test canrawviewtest.cpp)
target_link_libraries(canrawview_test canrawview Qt5::Core Qt5::SerialBus Qt5::Test cds-common)
target_compile_options(canrawview_test PRIVATE $<$<CXX_COMPILER_ID:GNU>:-fno-devirtualize>)
add_test( NAME CanRawViewTest COMMAND canrawview_test)

