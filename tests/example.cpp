#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>
#include "candevice/candevice.h"

TEST_CASE("Simple test", "[example]") {
    CanDevice canDevice;
    canDevice.init("socketcan", "can0");
    canDevice.start();
}

int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}
