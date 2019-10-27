#include <QtWidgets/QApplication>
#include <candevicemodel.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <datamodeltypes/canrawdata.h>
#include <catch.hpp>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[candevice]")
{
    using namespace fakeit;
    CanDeviceModel canDeviceModel;
    REQUIRE(canDeviceModel.caption() == "CanDevice");
    REQUIRE(canDeviceModel.name() == "CanDevice");
    REQUIRE(canDeviceModel.resizable() == false);
    REQUIRE(dynamic_cast<CanDeviceModel*>(canDeviceModel.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(canDeviceModel.embeddedWidget()) != nullptr);
}

TEST_CASE("Port information", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    CanRawData canDeviceDataIn;
    CanRawData canDeviceDataOut;
    REQUIRE(canDeviceModel.nPorts(PortType::Out) == 1);
    REQUIRE(canDeviceModel.nPorts(PortType::In) == 1);
    REQUIRE(canDeviceModel.nPorts(PortType::None) == 0);
    REQUIRE(canDeviceModel.dataType(PortType::Out, 0).id == canDeviceDataOut.type().id);
    REQUIRE(canDeviceModel.dataType(PortType::Out, 0).name == canDeviceDataOut.type().name);
    REQUIRE(canDeviceModel.dataType(PortType::In, 0).id == canDeviceDataIn.type().id);
    REQUIRE(canDeviceModel.dataType(PortType::In, 0).name == canDeviceDataIn.type().name);
    REQUIRE_NOTHROW(canDeviceModel.setInData(nullptr, 0));
}

TEST_CASE("Calling frameReceived emits dataUpdated and outData returns that frame", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    QSignalSpy dataUpdatedSpy(&canDeviceModel, &CanDeviceModel::dataUpdated);
    canDeviceModel.frameReceived(testFrame);

    REQUIRE(dataUpdatedSpy.count() == 1);
    REQUIRE(std::dynamic_pointer_cast<CanRawData>(canDeviceModel.outData(0))->frame().frameId()
        == testFrame.frameId());
}

TEST_CASE("Calling frameSent emits dataUpdated and outData returns that frame", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    QSignalSpy dataUpdatedSpy(&canDeviceModel, &CanDeviceModel::dataUpdated);
    canDeviceModel.frameSent(true, testFrame);
    REQUIRE(dataUpdatedSpy.count() == 1);
    REQUIRE(std::dynamic_pointer_cast<CanRawData>(canDeviceModel.outData(0))->frame().frameId()
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
    REQUIRE(sendFrameSpy.count() == 1);
    REQUIRE(qvariant_cast<QCanBusFrame>(sendFrameSpy.takeFirst().at(0)).frameId() == testFrame.frameId());
}

TEST_CASE("Test save configuration", "[candevice]")
{
    CanDeviceModel canDeviceModel;
    QJsonObject json = canDeviceModel.save();
    REQUIRE(json.find("name") != json.end());
}

TEST_CASE("Getters", "[candevice]")
{
    CanDeviceModel cdModel;

    REQUIRE(cdModel.painterDelegate() != nullptr);
    REQUIRE(cdModel.hasSeparateThread() == true);
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
    cds_debug("Starting unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
