#include <candevice/candevice.h>
// Keep fakeit as a last include. Otherwise it may cause compilation errors on Windows. 
#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

TEST_CASE("Simple test", "[example]") {
    CanDevice canDevice;
    canDevice.init("peakcan", "can0");
    canDevice.start();
}

int main(int argc, char *argv[])
{
    return Catch::Session().run(argc, argv);
}
