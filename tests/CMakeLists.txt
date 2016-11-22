include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/single_header/catch)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/fakeit/config/catch)
add_executable(example_test example.cpp)
add_test( NAME ExampleTest COMMAND example_test)
