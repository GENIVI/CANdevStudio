include_directories(${CMAKE_SOURCE_DIR}/3rdParty/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/cxxopts/include)

set(SRC
    main.cpp
    )

set_property(SOURCE main.cpp PROPERTY SKIP_AUTOMOC ON)
set_property(SOURCE main.cpp PROPERTY SKIP_AUTOUIC ON)

add_executable(templategen ${SRC})
target_link_libraries(templategen ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})

