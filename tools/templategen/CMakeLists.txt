include_directories(${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/cxxopts/include)

set(SRC
    main.cpp
    )

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)

add_executable(templategen ${SRC})
target_link_libraries(templategen ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

