
#include "candevice/candeviceqt.hpp"
#include "candevice/canfactoryqt.hpp"
#include <catch.hpp>

TEST_CASE("Invalid parameters throws an exception", "[candeviceqt]")
{
    auto fn = []() { CanDeviceQt{ "", "" }; };
    REQUIRE_THROWS(fn());
}
TEST_CASE("Interface invalid parameters throws an exception", "[candeviceqt]")
{
    CanFactoryQt factory;
    REQUIRE_THROWS(factory.create("", ""));
}
