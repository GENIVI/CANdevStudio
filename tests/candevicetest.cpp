
#include <candevice/candevice.h>
#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

#include "candevice/candeviceinterface.hpp"

#include "log.hpp"
std::shared_ptr<spdlog::logger> kDefaultLogger;

#include <QSignalSpy>
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

namespace {

// this should be an ordinary operator==, but clang 3.5 (that we use on Travis) doesn't like it then.
bool isEqual(const QCanBusFrame& f1, const QCanBusFrame& f2)
{
    return f1.isValid() == f2.isValid() && f1.frameId() == f2.frameId() && f1.frameType() == f2.frameType()
        && f1.payload() == f2.payload();
}

fakeit::Mock<CanDeviceInterface> getDeviceMock()
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;
    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    return deviceMock;
}

TEST_CASE("Initialization succedded", "[candevice]")
{
    auto deviceMock = getDeviceMock();
    CanDevice canDevice{ deviceMock.get() };
}

TEST_CASE("Start failed - could not connect to device", "[candevice]")
{
    using namespace fakeit;
    auto deviceMock = getDeviceMock();
    When(Method(deviceMock, connectDevice)).Return(false);

    CanDevice canDevice{ deviceMock.get() };
    CHECK(canDevice.start() == false);
}

TEST_CASE("Start succeeded", "[candevice]")
{
    using namespace fakeit;
    auto deviceMock = getDeviceMock();
    When(Method(deviceMock, connectDevice)).Return(true);

    CanDevice canDevice{ deviceMock.get() };
    CHECK(canDevice.start() == true);
}

TEST_CASE("sendFrame results in frameSent being emitted and writeFrame being called", "[candevice]")
{
    using namespace fakeit;
    auto deviceMock = getDeviceMock();
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(false);
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);

    CanDevice canDevice{ deviceMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);

    canDevice.sendFrame(testFrame);
    CHECK(frameSentSpy.count() == 1);
    CHECK(isEqual(qvariant_cast<QCanBusFrame>(frameSentSpy.takeFirst().at(1)), testFrame));
}

TEST_CASE("sendFrame, writeframe returns true, no signal emitted", "[candevice]")
{
    using namespace fakeit;
    auto deviceMock = getDeviceMock();
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);
    QCanBusFrame testFrame;

    CanDevice canDevice{ deviceMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);

    canDevice.sendFrame(testFrame);
    CHECK(frameSentSpy.count() == 0);
}

TEST_CASE("sendFrame defers FrameSent until backend emits frameWritten")
{
    using namespace fakeit;
    auto deviceMock = getDeviceMock();

    CanDeviceInterface::framesWritten_t writtenCbk;
    const auto frame = QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } };

    When(Method(deviceMock, setFramesWrittenCbk)).Do([&](auto&& fn) { writtenCbk = fn; });
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);

    CanDevice canDevice{ deviceMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);

    canDevice.sendFrame(frame);
    writtenCbk(1);
    CHECK(frameSentSpy.count() == 1);
    auto args = frameSentSpy.takeFirst();
    CHECK(args.at(0) == true);
    CHECK(isEqual(qvariant_cast<QCanBusFrame>(args.at(1)), frame));
}

TEST_CASE("emits all available frames when notified by backend", "[candevice]")
{
    using namespace fakeit;
    auto deviceMock = getDeviceMock();
    const std::vector<QCanBusFrame> frames{ QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } },
        QCanBusFrame{ 0, QByteArray{ 0 } }, QCanBusFrame{ 0xdeadbabe, QByteArray{ "\xde\xad\xbe\xef" } } };
    auto currentFrame = frames.begin();
    CanDeviceInterface::framesReceived_t receivedCbk;

    When(Method(deviceMock, setFramesReceivedCbk)).Do([&](auto&& fn) { receivedCbk = fn; });
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, framesAvailable)).AlwaysDo([&]() { return std::distance(currentFrame, frames.end()); });
    When(Method(deviceMock, readFrame)).AlwaysDo([&]() {
        auto f = *currentFrame;
        ++currentFrame;
        return f;
    });

    CanDevice canDevice{ deviceMock.get() };
    QSignalSpy frameReceivedSpy(&canDevice, &CanDevice::frameReceived);
    receivedCbk();
    CHECK(frameReceivedSpy.count() == static_cast<int>(frames.size()));
    for (auto i = 0u; i < frames.size(); ++i) {
        auto signalArgs = frameReceivedSpy.takeFirst();
        CHECK(isEqual(qvariant_cast<QCanBusFrame>(signalArgs.at(0)), frames[i]));
    }
}

TEST_CASE("WriteError causes emitting frameSent with frameSent=false")
{
    using namespace fakeit;
    const auto frame = QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } };
    CanDeviceInterface::errorOccurred_t errorCbk;
    auto deviceMock = getDeviceMock();
    When(Method(deviceMock, setErrorOccurredCbk)).Do([&](auto&& fn) { errorCbk = fn; });
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);
    CanDevice canDevice{ deviceMock.get() };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    canDevice.sendFrame(frame);
    errorCbk(QCanBusDevice::WriteError);
    CHECK(frameSentSpy.count() == 1);
    auto args = frameSentSpy.takeFirst();
    CHECK(args.at(0) == false);
    CHECK(isEqual(qvariant_cast<QCanBusFrame>(args.at(1)), frame));
}
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
