#include "candevice/candeviceqt.hpp"
#include <catch.hpp>

TEST_CASE("Invalid parameters throws an exception", "[candeviceqt]")
{
    CanDeviceQt dev;
    CHECK(dev.init("", "") == false);
}
