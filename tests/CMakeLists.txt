include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/single_header/catch)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/config/catch)
include_directories(${CMAKE_SOURCE_DIR}/src/components/candevice)

add_executable(example_test example.cpp)
add_test( NAME ExampleTest COMMAND example_test)

add_executable(socketcan_test socketcan_test.cpp)
target_link_libraries(socketcan_test candevice Qt5::Core Qt5::SerialBus)
add_test( NAME SocketCANTest COMMAND socketcan_test)

