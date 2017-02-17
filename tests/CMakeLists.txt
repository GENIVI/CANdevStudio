include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/single_header/catch)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/config/catch)

add_executable(example_test example.cpp)
add_test( NAME ExampleTest COMMAND example_test)

add_executable(socketcan_test socketcan_test.cpp)
target_link_libraries(socketcan_test components)
add_test( NAME SocketCANTest COMMAND socketcan_test)

