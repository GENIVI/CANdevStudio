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
    REQUIRE_THROWS(dev.disconnectDevice());
    REQUIRE_THROWS(dev.clearCallbacks());
}

TEST_CASE("Correct init parameters", "[candeviceqt]")
{
    CanDeviceQt dev;
    CHECK(dev.init("socketcan", "can0") == true);
}

TEST_CASE("Initialized device", "[candeviceqt]")
{
    CanDeviceQt dev;
    QCanBusFrame frame;

    CHECK(dev.init("socketcan", "can0") == true);

    REQUIRE_NOTHROW(dev.setFramesWrittenCbk({}));
    REQUIRE_NOTHROW(dev.setFramesReceivedCbk({}));
    REQUIRE_NOTHROW(dev.setErrorOccurredCbk({}));
    REQUIRE_NOTHROW(dev.writeFrame(frame));
    //REQUIRE_NOTHROW(dev.connectDevice());
    REQUIRE_NOTHROW(dev.readFrame());
    REQUIRE_NOTHROW(dev.framesAvailable());
    REQUIRE_NOTHROW(dev.disconnectDevice());
    REQUIRE_NOTHROW(dev.clearCallbacks());
}
