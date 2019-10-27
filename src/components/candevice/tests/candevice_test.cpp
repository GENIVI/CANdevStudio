#include "candevice.h"
#define CATCH_CONFIG_RUNNER
#include <QApplication>
#include <QSignalSpy>
#include <QtSerialBus/QCanBusDevice>
#include <candeviceinterface.h>
#include <catch.hpp>
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
    QWidget qo;
    qo.setProperty("backend", "dummy");
    qo.setProperty("interface", "dummy");
    canDevice.setConfig(qo);
}

} // namespace

TEST_CASE("Initialization failed", "[candevice]")
{
    CanDevice canDevice;
    QCanBusFrame frame;

    // properties not set
    REQUIRE(canDevice.init() == false);

    setupBackendInterface(canDevice);

    REQUIRE(canDevice.init() == false);

    REQUIRE_NOTHROW(canDevice.startSimulation());
    REQUIRE_NOTHROW(canDevice.sendFrame(frame));
}

TEST_CASE("Initialization succeeded", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;
    std::function<void()> rcvCbk;

    Fake(Method(deviceMock, setFramesWrittenCbk));
    When(Method(deviceMock, setFramesReceivedCbk)).AlwaysDo([&](auto&& fn) { rcvCbk = fn; });
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, clearCallbacks));
    Fake(Method(deviceMock, setParent));
    When(Method(deviceMock, init)).Return(true).Return(true).Return(false);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == true);

    // clearCallbacks
    REQUIRE(canDevice.init() == true);

    Verify(Method(deviceMock, clearCallbacks)).Exactly(Once);

    REQUIRE(canDevice.init() == false);
    REQUIRE_NOTHROW(rcvCbk());
}

TEST_CASE("Start failed", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, setParent));
    When(Method(deviceMock, init)).Return(false);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == false);
    REQUIRE_NOTHROW(canDevice.startSimulation());
}

TEST_CASE("Start failed - could not connect to device", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, clearCallbacks));
    Fake(Method(deviceMock, setParent));
    When(Method(deviceMock, init)).Return(true, true, true, false);
    When(Method(deviceMock, connectDevice)).Return(false, true, false, false);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == true);

    REQUIRE_NOTHROW(canDevice.startSimulation());
    // startSimulation will try to reconnect
    Verify(Method(deviceMock, init)).Exactly(2);
    Verify(Method(deviceMock, connectDevice)).Exactly(2);

    REQUIRE_NOTHROW(canDevice.startSimulation());
    Verify(Method(deviceMock, init)).Exactly(3);
    Verify(Method(deviceMock, connectDevice)).Exactly(4);

    REQUIRE(canDevice.init() == false);
    REQUIRE_NOTHROW(canDevice.startSimulation());
    Verify(Method(deviceMock, init)).Exactly(4);
    Verify(Method(deviceMock, connectDevice)).Exactly(4);
}

TEST_CASE("Start succeeded", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, init)).Return(true);
    When(Method(deviceMock, connectDevice)).Return(true);
    Fake(Method(deviceMock, setParent));

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == true);
    REQUIRE_NOTHROW(canDevice.startSimulation());
}

TEST_CASE("Stop uninitialized", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Method(deviceMock, setParent));

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    REQUIRE_NOTHROW(canDevice.stopSimulation());
}

TEST_CASE("Stop initialized", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, init)).Return(true);
    When(Method(deviceMock, connectDevice)).Return(true);
    Fake(Method(deviceMock, disconnectDevice));
    Fake(Method(deviceMock, setParent));

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == true);

    REQUIRE_NOTHROW(canDevice.stopSimulation());
}

TEST_CASE("Config changed", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    When(Method(deviceMock, setFramesWrittenCbk)).Do([](const auto& cb) { cb(100); });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    When(Method(deviceMock, init)).Return(true);
    When(Method(deviceMock, connectDevice)).Return(true);
    Fake(Method(deviceMock, setParent));

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    REQUIRE_NOTHROW(canDevice.configChanged());

    canDevice.startSimulation();

    REQUIRE_NOTHROW(canDevice.configChanged());
}

TEST_CASE("writeFrame results in error", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    Fake(Method(deviceMock, setParent));
    When(Method(deviceMock, writeFrame)).Return(false);
    When(Method(deviceMock, init)).Return(true);
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == true);

    canDevice.sendFrame(testFrame);
}

TEST_CASE("sendFrame, writeframe returns true, no signal emitted", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    Fake(Method(deviceMock, setParent));
    When(Method(deviceMock, writeFrame)).Return(true);
    When(Method(deviceMock, init)).Return(true);
    QCanBusFrame testFrame;

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);

    canDevice.sendFrame(testFrame);
    REQUIRE(frameSentSpy.count() == 0);
}

TEST_CASE("sendFrame, no device available, frameSent is not emitted", "[candevice]")
{
    using namespace fakeit;

    QCanBusFrame testFrame;
    CanDevice canDevice;

    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == false);

    canDevice.sendFrame(testFrame);
    REQUIRE(frameSentSpy.count() == 0);
}

TEST_CASE("sendFrame defers FrameSent until backend emits frameWritten", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    const auto frame = QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } };
    CanDeviceInterface::framesWritten_t writtenCbk;

    When(Method(deviceMock, setFramesWrittenCbk)).Do([&](auto&& fn) { writtenCbk = fn; });
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    Fake(Method(deviceMock, setParent));
    When(Method(deviceMock, writeFrame)).Return(true);
    When(Method(deviceMock, init)).Return(true);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == true);

    canDevice.sendFrame(frame);
    writtenCbk(1);
    REQUIRE(frameSentSpy.count() == 1);
    auto args = frameSentSpy.takeFirst();
    REQUIRE(args.at(0) == true);
    REQUIRE(isEqual(qvariant_cast<QCanBusFrame>(args.at(1)), frame));
}

TEST_CASE("Emits all available frames when notified by backend", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    const std::vector<QCanBusFrame> frames{ QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } },
        QCanBusFrame{ 0, QByteArray{ 0 } }, QCanBusFrame{ 0xdeadbabe, QByteArray{ "\xde\xad\xba\xef" } } };
    auto currentFrame = frames.begin();
    CanDeviceInterface::framesReceived_t receivedCbk;

    Fake(Method(deviceMock, setFramesWrittenCbk));
    When(Method(deviceMock, setFramesReceivedCbk)).Do([&](auto&& fn) { receivedCbk = fn; });
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, connectDevice));
    Fake(Method(deviceMock, setParent));
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
    REQUIRE(canDevice.init() == true);

    receivedCbk();
    REQUIRE(frameReceivedSpy.count() == static_cast<int>(frames.size()));
    for (auto i = 0u; i < frames.size(); ++i) {
        auto signalArgs = frameReceivedSpy.takeFirst();
        REQUIRE(isEqual(qvariant_cast<QCanBusFrame>(signalArgs.at(0)), frames[i]));
    }
}

TEST_CASE("WriteError causes emitting frameSent with frameSent=false", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    const auto frame = QCanBusFrame{ 0x12345678, QByteArray{ "\x50\x30\10" } };
    CanDeviceInterface::errorOccurred_t errorCbk;

    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    When(Method(deviceMock, setErrorOccurredCbk)).Do([&](auto&& fn) { errorCbk = fn; });
    Fake(Method(deviceMock, connectDevice));
    When(Method(deviceMock, writeFrame)).Return(true);
    When(Method(deviceMock, init)).Return(true);
    Fake(Method(deviceMock, setParent));

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QSignalSpy frameSentSpy(&canDevice, &CanDevice::frameSent);
    setupBackendInterface(canDevice);
    REQUIRE(canDevice.init() == true);

    canDevice.sendFrame(frame);
    errorCbk(QCanBusDevice::WriteError);
    REQUIRE(frameSentSpy.count() == 1);
    auto args = frameSentSpy.takeFirst();
    REQUIRE(args.at(0) == false);
    REQUIRE(isEqual(qvariant_cast<QCanBusFrame>(args.at(1)), frame));
}

TEST_CASE("read configuration to json format", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;
    Fake(Method(deviceMock, setParent));
    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };
    QJsonObject config = canDevice.getConfig();
    REQUIRE(config.count() == canDevice.getSupportedProperties().size());
}

TEST_CASE("setConfig using JSON read with QObject", "[candevice]")
{
    using namespace fakeit;

    Mock<CanDeviceInterface> deviceMock;
    Fake(Method(deviceMock, setParent));
    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    QJsonObject config;

    config["name"] = "CAN1";
    config["backend"] = "socketcan";
    config["interface"] = "can0";
    config["fake"] = "unsupported";

    canDevice.setConfig(config);

    auto qConfig = canDevice.getQConfig();

    REQUIRE(qConfig->property("name").toString() == "CAN1");
    REQUIRE(qConfig->property("backend").toString() == "socketcan");
    REQUIRE(qConfig->property("interface").toString() == "can0");
    REQUIRE(qConfig->property("fake").isValid() == false);
}

TEST_CASE("Stubbed methods", "[candevice]")
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;
    Fake(Method(deviceMock, setParent));
    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    REQUIRE(canDevice.mainWidget() == nullptr);
    REQUIRE(canDevice.mainWidgetDocked() == true);
}

auto prepareConfigTestMock(std::vector<std::pair<int, QVariant>>& ver)
{
    using namespace fakeit;
    Mock<CanDeviceInterface> deviceMock;

    Fake(Method(deviceMock, setFramesWrittenCbk));
    Fake(Method(deviceMock, setFramesReceivedCbk));
    Fake(Method(deviceMock, setErrorOccurredCbk));
    Fake(Method(deviceMock, clearCallbacks));
    Fake(Method(deviceMock, setParent));
    When(Method(deviceMock, init)).AlwaysReturn(true);
    // Fake(Method(deviceMock, setConfigurationParameter));
    When(Method(deviceMock, setConfigurationParameter)).AlwaysDo([&](int key, const QVariant& v) {
        ver.push_back(std::make_pair(key, v));
    });
    return deviceMock;
}

void testConfig(fakeit::Mock<CanDeviceInterface>& deviceMock, CanDevice& canDevice, const QString& configStr)
{
    QWidget qo;

    deviceMock.ClearInvocationHistory();

    qo.setProperty("backend", "dummy");
    qo.setProperty("interface", "dummy");
    qo.setProperty("configuration", configStr);
    canDevice.setConfig(qo);
    REQUIRE(canDevice.init() == true);
}

// Use macro so Catch could show exact line of failure
#define testConfig_Expect0(mock, dev, str)                                                                             \
    testConfig(mock, dev, str);                                                                                        \
    fakeit::Verify(Method(mock, setConfigurationParameter)).Exactly(0);

// Use macro so Catch could show exact line of failure
#define testConfig_Expect1(mock, dev, key, val, str)                                                                   \
    testConfig(mock, dev, str);                                                                                        \
    fakeit::Verify(Method(mock, setConfigurationParameter)).Exactly(1);                                                \
    REQUIRE(v.back().first == key);                                                                                      \
    REQUIRE(v.back().second == val);

TEST_CASE("Config parameter - invalid format and unsupported", "[candevice]")
{
    using namespace fakeit;
    std::vector<std::pair<int, QVariant>> v;
    auto&& deviceMock = prepareConfigTestMock(v);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    testConfig_Expect0(deviceMock, canDevice, "dummy");
    testConfig_Expect0(deviceMock, canDevice, "=");
    testConfig_Expect0(deviceMock, canDevice, "    ");
    testConfig_Expect0(deviceMock, canDevice, "dummy=dummy");
    testConfig_Expect0(deviceMock, canDevice, "dummy=dummy;");
    testConfig_Expect0(deviceMock, canDevice, ";");
    testConfig_Expect0(deviceMock, canDevice, "RawFilterKey=123");
    testConfig_Expect0(deviceMock, canDevice, "ErrorFilterKey=123");
}

TEST_CASE("Config parameter - LoopbackKey", "[candevice]")
{
    using namespace fakeit;
    std::vector<std::pair<int, QVariant>> v;
    auto&& deviceMock = prepareConfigTestMock(v);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    testConfig_Expect0(deviceMock, canDevice, "LoopbackKey");
    testConfig_Expect0(deviceMock, canDevice, "LoopbackKey=");
    testConfig_Expect0(deviceMock, canDevice, "  LoopbackKey  =    ");

    const int key = QCanBusDevice::LoopbackKey;
    testConfig_Expect1(deviceMock, canDevice, key, true, "LoopbackKey=True");
    testConfig_Expect1(deviceMock, canDevice, key, true, "LoopbackKey=TRUE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, true, "; dummy =    dummy;   LoopbackKey=TRUE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, true, "  LoopbackKey =  true; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "LoopbackKey=False");
    testConfig_Expect1(deviceMock, canDevice, key, false, "LoopbackKey=FALSE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "; dummy =    dummy;   LoopbackKey=TRU;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "  LoopbackKey =  false; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "LoopbackKey=dummy");
}

TEST_CASE("Config parameter - ReceiveOwnKey", "[candevice]")
{
    using namespace fakeit;
    std::vector<std::pair<int, QVariant>> v;
    auto&& deviceMock = prepareConfigTestMock(v);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    testConfig_Expect0(deviceMock, canDevice, "ReceiveOwnKey");
    testConfig_Expect0(deviceMock, canDevice, "ReceiveOwnKey=");
    testConfig_Expect0(deviceMock, canDevice, "  ReceiveOwnKey  =    ");

    const int key = QCanBusDevice::ReceiveOwnKey;
    testConfig_Expect1(deviceMock, canDevice, key, true, "ReceiveOwnKey=True");
    testConfig_Expect1(deviceMock, canDevice, key, true, "ReceiveOwnKey=TRUE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, true, "; dummy =    dummy;   ReceiveOwnKey=TRUE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, true, "  ReceiveOwnKey =  true; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "ReceiveOwnKey=False");
    testConfig_Expect1(deviceMock, canDevice, key, false, "ReceiveOwnKey=FALSE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "; dummy =    dummy;   ReceiveOwnKey=TRU;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "  ReceiveOwnKey =  false; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "ReceiveOwnKey=dummy");
}

TEST_CASE("Config parameter - CanFdKey", "[candevice]")
{
    using namespace fakeit;
    std::vector<std::pair<int, QVariant>> v;
    auto&& deviceMock = prepareConfigTestMock(v);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    testConfig_Expect0(deviceMock, canDevice, "CanFdKey");
    testConfig_Expect0(deviceMock, canDevice, "CanFdKey=");
    testConfig_Expect0(deviceMock, canDevice, "  CanFdKey  =    ");

    const int key = QCanBusDevice::CanFdKey;
    testConfig_Expect1(deviceMock, canDevice, key, true, "CanFdKey=True");
    testConfig_Expect1(deviceMock, canDevice, key, true, "CanFdKey=TRUE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, true, "; dummy =    dummy;   CanFdKey=TRUE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, true, "  CanFdKey =  true; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "CanFdKey=False");
    testConfig_Expect1(deviceMock, canDevice, key, false, "CanFdKey=FALSE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "; dummy =    dummy;   CanFdKey=TRU;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "  CanFdKey =  false; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, false, "CanFdKey=dummy");
}

TEST_CASE("Config parameter - BitRateKey", "[candevice]")
{
    using namespace fakeit;
    std::vector<std::pair<int, QVariant>> v;
    auto&& deviceMock = prepareConfigTestMock(v);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    testConfig_Expect0(deviceMock, canDevice, "BitRateKey");
    testConfig_Expect0(deviceMock, canDevice, "BitRateKey=");
    testConfig_Expect0(deviceMock, canDevice, "  BitRateKey  =   ");
    testConfig_Expect0(deviceMock, canDevice, "  BitRateKey  =   -1 ");
    testConfig_Expect0(deviceMock, canDevice, "  BitRateKey  =   asdasd ");

    const int key = QCanBusDevice::BitRateKey;
    testConfig_Expect1(deviceMock, canDevice, key, 1000000, "BitRateKey=1000000");
    testConfig_Expect1(deviceMock, canDevice, key, 123456789, "; dummy =    dummy;   BitRateKey=123456789;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, 1, "  BitRateKey =  1; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, 0, "BitRateKey=0");
}

TEST_CASE("Config parameter - UserKey", "[candevice]")
{
    using namespace fakeit;
    std::vector<std::pair<int, QVariant>> v;
    auto&& deviceMock = prepareConfigTestMock(v);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    testConfig_Expect0(deviceMock, canDevice, "UserKey");
    testConfig_Expect0(deviceMock, canDevice, "UserKey=");
    testConfig_Expect0(deviceMock, canDevice, "  UserKey  =    ");

    const int key = QCanBusDevice::UserKey;
    testConfig_Expect1(deviceMock, canDevice, key, "True", "UserKey=True");
    testConfig_Expect1(deviceMock, canDevice, key, "TexT", "UserKey=TexT;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, "123456", "; dummy =    dummy;   UserKey=123456;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, "-111", "  UserKey =  -111; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, "FALSE", "UserKey=FALSE;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, "TRU", "; dummy =    dummy;   UserKey=TRU;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, "false", "  UserKey =  false; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, "dummy", "UserKey=dummy");
}

#if QT_VERSION >= 0x050900
TEST_CASE("Config parameter - DataBitRateKey", "[candevice]")
{
    using namespace fakeit;
    std::vector<std::pair<int, QVariant>> v;
    auto&& deviceMock = prepareConfigTestMock(v);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    testConfig_Expect0(deviceMock, canDevice, "DataBitRateKey");
    testConfig_Expect0(deviceMock, canDevice, "DataBitRateKey=");
    testConfig_Expect0(deviceMock, canDevice, "  DataBitRateKey  =   ");
    testConfig_Expect0(deviceMock, canDevice, "  DataBitRateKey  =   -1 ");
    testConfig_Expect0(deviceMock, canDevice, "  DataBitRateKey  =   asdasd ");

    const int key = QCanBusDevice::DataBitRateKey;
    testConfig_Expect1(deviceMock, canDevice, key, 1000000, "DataBitRateKey=1000000");
    testConfig_Expect1(
        deviceMock, canDevice, key, 123456789, "; dummy =    dummy;   DataBitRateKey=123456789;dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, 1, "  DataBitRateKey =  1; dummy=dummy");
    testConfig_Expect1(deviceMock, canDevice, key, 0, "DataBitRateKey=0");
}
#endif

TEST_CASE("Config parameter - multiple keys", "[candevice]")
{
    using namespace fakeit;
    std::vector<std::pair<int, QVariant>> v;
    auto&& deviceMock = prepareConfigTestMock(v);

    CanDevice canDevice{ CanDeviceCtx(&deviceMock.get()) };

    testConfig_Expect0(deviceMock, canDevice,
        "RawKeyFilter;ErrorFilterKey;LoopbackKey;ReceiveOwnKey;BitRateKey;CanFdKey;DataBitRateKey;UserKey");
    testConfig_Expect0(deviceMock, canDevice,
        "RawKeyFilter=;   ErrorFilterKey   =;    LoopbackKey;    "
        "ReceiveOwnKey;BitRateKey;CanFdKey;DataBitRateKey;UserKey=;");

    testConfig(deviceMock, canDevice,
        "RawKeyFilter=;ErrorFilterKey;    LoopbackKey = true;    ReceiveOwnKey = false ;BitRateKey= "
        "100000;CanFdKey=true;UserKey=1000;");
    fakeit::Verify(Method(deviceMock, setConfigurationParameter)).Exactly(5);
    REQUIRE(v[0].first == QCanBusDevice::LoopbackKey);
    REQUIRE(v[0].second == true);
    REQUIRE(v[1].first == QCanBusDevice::ReceiveOwnKey);
    REQUIRE(v[1].second == false);
    REQUIRE(v[2].first == QCanBusDevice::BitRateKey);
    REQUIRE(v[2].second == 100000);
    REQUIRE(v[3].first == QCanBusDevice::CanFdKey);
    REQUIRE(v[3].second == true);
    REQUIRE(v[4].first == QCanBusDevice::UserKey);
    REQUIRE(v[4].second == "1000");

    testConfig(deviceMock, canDevice,
        "RawKeyFilter=;ErrorFilterKey;    LoopbackKey = FALSE;    ReceiveOwnKey = TRue ;BitRateKey= "
        "666;CanFdKey=xxx;UserKey=1000;");
    fakeit::Verify(Method(deviceMock, setConfigurationParameter)).Exactly(5);
    REQUIRE(v[5].first == QCanBusDevice::LoopbackKey);
    REQUIRE(v[5].second == false);
    REQUIRE(v[6].first == QCanBusDevice::ReceiveOwnKey);
    REQUIRE(v[6].second == true);
    REQUIRE(v[7].first == QCanBusDevice::BitRateKey);
    REQUIRE(v[7].second == 666);
    REQUIRE(v[8].first == QCanBusDevice::CanFdKey);
    REQUIRE(v[8].second == false);
    REQUIRE(v[9].first == QCanBusDevice::UserKey);
    REQUIRE(v[9].second == "1000");
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
