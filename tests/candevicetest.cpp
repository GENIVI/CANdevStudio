#define private public
#include <candevice/candevice.h>
#undef private

#define CATCH_CONFIG_RUNNER
#include <QSignalSpy>
#include <QtSerialBus/QCanBusDevice>
#include <candeviceinterface.h>
#include <context.h>
#include <fakeit.hpp>
#include <log.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

namespace {

bool isEqual(const QCanBusFrame& f1, const QCanBusFrame& f2)
{
    return f1.isValid() == f2.isValid() && f1.frameId() == f2.frameId() && f1.frameType() == f2.frameType()
        && f1.payload() == f2.payload();
}


void setupBackendInterface(CanDevice& canDevice)
{
    QObject qo;
    qo.setProperty("backend", "dummy");
    qo.setProperty("interface", "dummy");
    canDevice.setConfig(qo);
}

}

TEST_CASE("Initialization failed", "[candevice]")
{
    CanDevice canDevice;
    QCanBusFrame frame;

    // properties not set
    CHECK(canDevice.init() == false);
    
    setupBackendInterface(canDevice);

    CHECK(canDevice.init() == false);

    REQUIRE_NOTHROW(canDevice.framesReceived());
    REQUIRE_NOTHROW(canDevice.startSimulation());
    REQUIRE_NOTHROW(canDevice.sendFrame(frame));
}

TEST_CASE("Initialization succedded", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, clearCallbacks));
    When(Method(deviceMock, init)).AlwaysReturn(true);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == true);

    // clearCallbacks
    CHECK(canDevice.init() == true);

    Verify(Method(deviceMock, clearCallbacks)).Exactly(Once);
}

TEST_CASE("Start failed", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, init)).Return(false);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == false);
    REQUIRE_NOTHROW(canDevice.startSimulation());
}

TEST_CASE("Start failed - could not connect to device", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, clearCallbacks));
    When(Method(deviceMock, init)).Return(true, true, true, false);
    When(Method(deviceMock, connectDevice)).Return(false, true, false, false);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == true);

    REQUIRE_NOTHROW(canDevice.startSimulation());
    // startSimulation will try to reconnect
    Verify(Method(deviceMock, init)).Exactly(2);
    Verify(Method(deviceMock, connectDevice)).Exactly(2);

    REQUIRE_NOTHROW(canDevice.startSimulation());
    Verify(Method(deviceMock, init)).Exactly(3);
    Verify(Method(deviceMock, connectDevice)).Exactly(4);

    CHECK(canDevice.init() == false);
    REQUIRE_NOTHROW(canDevice.startSimulation());
    Verify(Method(deviceMock, init)).Exactly(4);
    Verify(Method(deviceMock, connectDevice)).Exactly(4);
}

TEST_CASE("Start succeeded", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, init)).Return(true);
    When(Method(deviceMock, connectDevice)).Return(true);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == true);
    REQUIRE_NOTHROW(canDevice.startSimulation());
}

TEST_CASE("Stop uninitialized", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    REQUIRE_NOTHROW(canDevice.stopSimulation());
}

TEST_CASE("Stop initialized", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, init)).Return(true);
    When(Method(deviceMock, connectDevice)).Return(true);
    Fake(Method(deviceMock, disconnectDevice));

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == true);

    REQUIRE_NOTHROW(canDevice.stopSimulation());
}

TEST_CASE("Config changed", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, init)).Return(true);
    When(Method(deviceMock, connectDevice)).Return(true);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    REQUIRE_NOTHROW(canDevice.configChanged());

    canDevice.startSimulation();

    REQUIRE_NOTHROW(canDevice.configChanged());
}

TEST_CASE("writeFrame results in error", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(false);
    When(Method(deviceMock, init)).Return(true);
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == true);

    canDevice.sendFrame(testFrame);
}

TEST_CASE("sendFrame, writeframe returns true, no signal emitted", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);
    When(Method(deviceMock, init)).Return(true);
    QCanBusFrame testFrame;

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);

    canDevice.sendFrame(testFrame);
    CHECK(frameSentSpy.count() == 0);
}

TEST_CASE("sendFrame, no device availablie, frameSent is not emitted", "[candevice]")
{
    using namespace fakeit;

    QCanBusFrame testFrame;
    CanDevice canDevice;

    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == false);

    canDevice.sendFrame(testFrame);
    CHECK(frameSentSpy.count() == 0);
}

TEST_CASE("sendFrame defers FrameSent until backend emits frameWritten", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    const auto frame = QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } };
    CanDeviceInterface::framesWritten_t writtenCbk;

    Fake(Dtor(deviceMock));
    When(Method(deviceMock, setFramesWrittenCbk)).Do([&](auto&& fn) { writtenCbk = fn; });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);
    When(Method(deviceMock, init)).Return(true);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == true);

    canDevice.sendFrame(frame);
    writtenCbk(1);
    CHECK(frameSentSpy.count() == 1);
    auto args = frameSentSpy.takeFirst();
    CHECK(args.at(0) == true);
    CHECK(isEqual(qvariant_cast<QCanBusFrame>(args.at(1)), frame));
}

TEST_CASE("Emits all available frames when notified by backend", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    const std::vector<QCanBusFrame> frames{ QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } },
        QCanBusFrame{ 0, QByteArray{ 0 } }, QCanBusFrame{ 0xdeadbabe, QByteArray{ "\xde\xad\xba\xef" } } };
    auto currentFrame = frames.begin();
    CanDeviceInterface::framesReceived_t receivedCbk;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    When(Method(deviceMock, setFramesReceivedCbk)).Do([&](auto&& fn) { receivedCbk = fn; });
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, init)).Return(true);

    When(Method(deviceMock, framesAvailable)).AlwaysDo([&]() { return std::distance(currentFrame, frames.end()); });
    When(Method(deviceMock, readFrame)).AlwaysDo([&]() {
        auto f = *currentFrame;
        ++currentFrame;
        return f;
    });

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QSignalSpy frameReceivedSpy(&canDevice, &CanDevice::frameReceived);
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == true);

    receivedCbk();
    CHECK(frameReceivedSpy.count() == static_cast<int>(frames.size()));
    for (auto i = 0u; i < frames.size(); ++i) {
        auto signalArgs = frameReceivedSpy.takeFirst();
        CHECK(isEqual(qvariant_cast<QCanBusFrame>(signalArgs.at(0)), frames[i]));
    }
}

TEST_CASE("WriteError causes emitting frameSent with framSent=false", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    const auto frame = QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } };
    CanDeviceInterface::errorOccurred_t errorCbk;

    Fake(Dtor(deviceMock));
    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    When(Method(deviceMock, setErrorOccurredCbk)).Do([&](auto&& fn) { errorCbk = fn; });
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);
    When(Method(deviceMock, init)).Return(true);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    setupBackendInterface(canDevice);
    CHECK(canDevice.init() == true);

    canDevice.sendFrame(frame);
    errorCbk(QCanBusDevice::WriteError);
    CHECK(frameSentSpy.count() == 1);
    auto args = frameSentSpy.takeFirst();
    CHECK(args.at(0) == false);
    CHECK(isEqual(qvariant_cast<QCanBusFrame>(args.at(1)), frame));
}

TEST_CASE("read configuration to json format", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;
    Fake(Dtor(deviceMock));
    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QJsonObject config = canDevice.getConfig();
    CHECK(config.count() == canDevice.getSupportedProperties().size());
}

TEST_CASE("setConfig using JSON read with QObject", "[candevice]")
{
    using namespace fakeit;
 
    Mock<CanDeviceInterface> deviceMock;
    Fake(Dtor(deviceMock));
    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    QJsonObject config;

    config["name"] = "CAN1";
    config["backend"] = "socketcan";
    config["interface"] = "can0";
    config["fake"] = "unsupported";

    canDevice.setConfig(config);

    auto qConfig = canDevice.getQConfig();

    CHECK(qConfig->property("name").toString() == "CAN1");
    CHECK(qConfig->property("backend").toString() == "socketcan");
    CHECK(qConfig->property("interface").toString() == "can0");
    CHECK(qConfig->property("fake").isValid() == false);
}

TEST_CASE("Stubbed methods", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;
    Fake(Dtor(deviceMock));
    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    CHECK(canDevice.mainWidget() == nullptr);
    CHECK(canDevice.mainWidgetDocked() == true);
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
