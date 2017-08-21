
#include <candevice/candevice.h>
#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

#include "candevice/candeviceinterface.hpp"
#include "candevice/canfactory.hpp"

#include "log.hpp"
std::shared_ptr<spdlog::logger> kDefaultLogger;

#include <QSignalSpy>
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

namespace {

bool isEqual(const QCanBusFrame& f1, const QCanBusFrame& f2)
{
    return f1.isValid() == f2.isValid() && f1.frameId() == f2.frameId() && f1.frameType() == f2.frameType()
        && f1.payload() == f2.payload();
}
}

TEST_CASE("Initialization failed", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;
    When(Method(factoryMock, create)).Return(nullptr);
    CanDevice canDevice{ factoryMock.get() };
    CHECK(canDevice.init("", "") == false);
}

TEST_CASE("Initialization succedded", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));

    When(Method(factoryMock, create)).Return(&(deviceMock.get()));
    CanDevice canDevice{ factoryMock.get() };
    CHECK(canDevice.init("", "") == true);
}

TEST_CASE("Start failed", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;

    When(Method(factoryMock, create)).Return(nullptr);

    CanDevice canDevice{ factoryMock.get() };
    CHECK(canDevice.start() == false);
}

TEST_CASE("Start failed - could not connect to device", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, connectDevice)).Return(false);

    When(Method(factoryMock, create)).Return(&(deviceMock.get()));
    CanDevice canDevice{ factoryMock.get() };
    CHECK(canDevice.init("", "") == true);
    CHECK(canDevice.start() == false);
}

TEST_CASE("Start succeeded", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, connectDevice)).Return(true);

    When(Method(factoryMock, create)).Return(&(deviceMock.get()));
    CanDevice canDevice{ factoryMock.get() };
    CHECK(canDevice.init("", "") == true);
    CHECK(canDevice.start() == true);
}

TEST_CASE("sendFrame results in frameSent being emitted and writeFrame being called", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(false);
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);

    When(Method(factoryMock, create)).Return(&(deviceMock.get()));
    CanDevice canDevice{ factoryMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    CHECK(canDevice.init("", "") == true);

    canDevice.sendFrame(testFrame);
    CHECK(frameSentSpy.count() == 1);
    CHECK(qvariant_cast<QCanBusFrame>(frameSentSpy.takeFirst().at(1)).frameId() == testFrame.frameId());
}

TEST_CASE("sendFrame, writeframe returns true, no signal emitted", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);
    QCanBusFrame testFrame;

    When(Method(factoryMock, create)).Return(&(deviceMock.get()));
    CanDevice canDevice{ factoryMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);

    canDevice.sendFrame(testFrame);
    CHECK(frameSentSpy.count() == 0);
}

TEST_CASE("sendFrame, no device availablie, frameSent is not emitted", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;

    QCanBusFrame testFrame;

    When(Method(factoryMock, create)).Return(nullptr);
    CanDevice canDevice{ factoryMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    CHECK(canDevice.init("", "") == false);

    canDevice.sendFrame(testFrame);
    CHECK(frameSentSpy.count() == 0);
}

TEST_CASE("sendFrame defers FrameSent until backend emits frameWritten", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;
    Mock<CanDeviceInterface> deviceMock;

    const auto frame = QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } };
    CanDeviceInterface::framesWritten_t writtenCbk;

    Fake(Dtor(deviceMock));
    When(Method(deviceMock, setFramesWrittenCbk)).Do([&](auto&& fn) { writtenCbk = fn; });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);

    When(Method(factoryMock, create)).Return(&(deviceMock.get()));
    CanDevice canDevice{ factoryMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    CHECK(canDevice.init("", "") == true);

    canDevice.sendFrame(frame);
    writtenCbk(1);
    CHECK(frameSentSpy.count() == 1);
    auto args = frameSentSpy.takeFirst();
    CHECK(args.at(0) == true);
    CHECK(isEqual(qvariant_cast<QCanBusFrame>(args.at(1)), frame));
}

TEST_CASE("TODOTODOTODO  second test 11111", "[candevice]")
{
    using namespace fakeit;
    Mock<CanFactoryInterface> factoryMock;
    Mock<CanDeviceInterface> deviceMock;

    const auto frame = QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } };
    CanDeviceInterface::errorOccurred_t errorCbk;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    When(Method(deviceMock, setErrorOccurredCbk)).Do([&](auto&& fn) { errorCbk = fn; });
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);

    When(Method(factoryMock, create)).Return(&(deviceMock.get()));
    CanDevice canDevice{ factoryMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    CHECK(canDevice.init("", "") == true);

    canDevice.sendFrame(testFrame);

    errorCbk(QCanBusDevice::WriteError);

    CHECK(frameSentSpy.count() == 1);
    CHECK(qvariant_cast<QCanBusFrame>(frameSentSpy.takeFirst().at(1)).frameId() == testFrame.frameId());
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    return Catch::Session().run(argc, argv);
}
