#include <QtWidgets/QApplication>
#include <candevicemodel.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <datamodeltypes/canrawdata.h>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[candevice]")
{
    using namespace fakeit;
    CanDeviceModel canDeviceModel;
    CHECK(canDeviceModel.caption() == "CanDevice");
    CHECK(canDeviceModel.name() == "CanDevice");
    CHECK(canDeviceModel.resizable() == false);
    CHECK(dynamic_cast<CanDeviceModel*>(canDeviceModel.clone().get()) != nullptr);
    CHECK(dynamic_cast<QLabel*>(canDeviceModel.embeddedWidget()) != nullptr);
}

TEST_CASE("Port information", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    CanRawData canDeviceDataIn;
    CanRawData canDeviceDataOut;
    CHECK(canDeviceModel.nPorts(PortType::Out) == 1);
    CHECK(canDeviceModel.nPorts(PortType::In) == 1);
    CHECK(canDeviceModel.nPorts(PortType::None) == 0);
    CHECK(canDeviceModel.dataType(PortType::Out, 0).id == canDeviceDataOut.type().id);
    CHECK(canDeviceModel.dataType(PortType::Out, 0).name == canDeviceDataOut.type().name);
    CHECK(canDeviceModel.dataType(PortType::In, 0).id == canDeviceDataIn.type().id);
    CHECK(canDeviceModel.dataType(PortType::In, 0).name == canDeviceDataIn.type().name);
    REQUIRE_NOTHROW(canDeviceModel.setInData(nullptr, 0));
}

TEST_CASE("Calling frameReceived emits dataUpdated and outData returns that frame", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    QSignalSpy dataUpdatedSpy(&canDeviceModel, &CanDeviceModel::dataUpdated);
    canDeviceModel.frameReceived(testFrame);

    CHECK(dataUpdatedSpy.count() == 1);
    CHECK(std::dynamic_pointer_cast<CanRawData>(canDeviceModel.outData(0))->frame().frameId()
        == testFrame.frameId());
}

TEST_CASE("Calling frameSent emits dataUpdated and outData returns that frame", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    QSignalSpy dataUpdatedSpy(&canDeviceModel, &CanDeviceModel::dataUpdated);
    canDeviceModel.frameSent(true, testFrame);
    CHECK(dataUpdatedSpy.count() == 1);
    CHECK(std::dynamic_pointer_cast<CanRawData>(canDeviceModel.outData(0))->frame().frameId()
        == testFrame.frameId());
}

TEST_CASE("Calling setInData will result in sendFrame being emitted", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    auto canDeviceDataIn = std::make_shared<CanRawData>(testFrame);
    QSignalSpy sendFrameSpy(&canDeviceModel, &CanDeviceModel::sendFrame);

    canDeviceModel.setInData(canDeviceDataIn, 0);
    CHECK(sendFrameSpy.count() == 1);
    CHECK(qvariant_cast<QCanBusFrame>(sendFrameSpy.takeFirst().at(0)).frameId() == testFrame.frameId());
}

TEST_CASE("Test save configuration", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    QJsonObject json = canDeviceModel.save();
    CHECK(json.find("name") != json.end());
}

TEST_CASE("Getters", "[candevice]")
{
    CanDeviceModel cdModel;

    CHECK(cdModel.painterDelegate() != nullptr);
    CHECK(cdModel.hasSeparateThread() == true);
}

TEST_CASE("Tx queue full test", "[candevice]")
{
    CanDeviceModel cdModel;
    QCanBusFrame frame;

    for(int i=0; i < 200; ++i) {
        cdModel.frameSent(true, frame);
    }
}

TEST_CASE("Rx queue full test", "[candevice]")
{
    CanDeviceModel cdModel;
    QCanBusFrame frame;

    for(int i=0; i < 200; ++i) {
        cdModel.frameReceived(frame);
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
    QApplication a(argc, argv); // QApplication must exist when contructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
