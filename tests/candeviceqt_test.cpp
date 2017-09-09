#include <candevice/candeviceqt.h>
#include <catch.hpp>

TEST_CASE("Invalid parameters init fails", "[candeviceqt]")
{
    CanDeviceQt dev;
    CHECK(dev.init("", "") == false);
}

TEST_CASE("Uninitialized device", "[candeviceqt]")
{
    CanDeviceQt dev;
    QCanBusFrame frame;

    REQUIRE_THROWS(dev.setFramesWrittenCbk({}));
    REQUIRE_THROWS(dev.setFramesReceivedCbk({}));
    REQUIRE_THROWS(dev.setErrorOccurredCbk({}));
    REQUIRE_THROWS(dev.writeFrame(frame));
    REQUIRE_THROWS(dev.connectDevice());
    REQUIRE_THROWS(dev.readFrame());
    REQUIRE_THROWS(dev.framesAvailable());
}
