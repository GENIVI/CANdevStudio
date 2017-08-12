
#include <catch.hpp>
#include "candevice/candeviceqt.hpp"

TEST_CASE("Invalid parameters throws an exception", "[candeviceqt]")
{
    auto fn = []() { CanDeviceQt{ "", "" }; };
    REQUIRE_THROWS(fn());
}
