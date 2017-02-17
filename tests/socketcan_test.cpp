#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>
#include <iostream>
#include <components/socketcan.h>

TEST_CASE("SocketCAN", "test") {
    SocketCan sc("can0");
}

int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}

