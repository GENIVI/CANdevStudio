#include "candeviceqt.h"
#include <catch.hpp>
#include <QEventLoop>
#include <QVariant>

TEST_CASE("Invalid parameters init fails", "[candeviceqt]")
{
    CanDeviceQt dev;
    REQUIRE(dev.init("", "") == false);
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
    REQUIRE_THROWS(dev.setConfigurationParameter(0, 0));
}

TEST_CASE("Correct init parameters", "[candeviceqt]")
{
    CanDeviceQt dev;
    REQUIRE(dev.init("socketcan", "can0") == true);
}

TEST_CASE("Initialized device", "[candeviceqt]")
{
    CanDeviceQt dev;
    QCanBusFrame frame;

    REQUIRE(dev.init("socketcan", "can0") == true);

    REQUIRE_NOTHROW(dev.connectDevice());
    REQUIRE_NOTHROW(dev.setFramesWrittenCbk({}));
    REQUIRE_NOTHROW(dev.setFramesReceivedCbk({}));
    REQUIRE_NOTHROW(dev.setErrorOccurredCbk({}));
    REQUIRE_NOTHROW(dev.writeFrame(frame));
    REQUIRE_NOTHROW(dev.readFrame());
    REQUIRE_NOTHROW(dev.framesAvailable());
    REQUIRE_NOTHROW(dev.disconnectDevice());
    REQUIRE_NOTHROW(dev.clearCallbacks());
    REQUIRE_NOTHROW(dev.setConfigurationParameter(0, 0));
}

TEST_CASE("Thread", "[candeviceqt]")
{
    CanDeviceQt dev;
    QEventLoop el;

    dev.setParent(&el);
    REQUIRE(dev.init("socketcan", "can0") == true);
}
