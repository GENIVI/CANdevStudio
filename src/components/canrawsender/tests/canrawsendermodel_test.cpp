#include <QtWidgets/QApplication>
#include <canrawsendermodel.h>
#include <datamodeltypes/canrawdata.h>
#define CATCH_CONFIG_RUNNER
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>
#include <log.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;

TEST_CASE("Test basic functionality", "[canrawsender]")
{
    CanRawSenderModel canRawSenderModel;
    REQUIRE(canRawSenderModel.caption() == "CanRawSender");
    REQUIRE(canRawSenderModel.name() == "CanRawSender");
    REQUIRE(canRawSenderModel.resizable() == false);
    REQUIRE(dynamic_cast<CanRawSenderModel*>(canRawSenderModel.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(canRawSenderModel.embeddedWidget()) != nullptr);
}

TEST_CASE("Port information", "[canrawsender]")
{
    CanRawSenderModel canRawSenderModel;
    CanRawData canRawSenderDataOut;
    REQUIRE(canRawSenderModel.nPorts(PortType::Out) == 1);
    REQUIRE(canRawSenderModel.nPorts(PortType::In) == 0);
    REQUIRE(canRawSenderModel.nPorts(PortType::None) == 0);
    REQUIRE(canRawSenderModel.dataType(PortType::Out, 0).id == canRawSenderDataOut.type().id);
    REQUIRE(canRawSenderModel.dataType(PortType::Out, 0).name == canRawSenderDataOut.type().name);
}

TEST_CASE("Calling sendFrame emits dataUpdated and outData returns that frame", "[canrawsender]")
{
    CanRawSenderModel canRawSenderModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    QSignalSpy dataUpdatedSpy(&canRawSenderModel, &CanRawSenderModel::dataUpdated);
    canRawSenderModel.sendFrame(testFrame);
    REQUIRE(dataUpdatedSpy.count() == 1);
    REQUIRE(
        std::dynamic_pointer_cast<CanRawData>(canRawSenderModel.outData(0))->frame().frameId() == testFrame.frameId());
}

TEST_CASE("Test save configuration", "[canrawsender]")
{
    CanRawSenderModel canRawSenderModel;
    QJsonObject json = canRawSenderModel.save();
    REQUIRE(json.find("name") != json.end());
    REQUIRE(json.find("senderColumns") != json.end());
    REQUIRE(json.find("content") != json.end());
    REQUIRE(json.find("sorting") != json.end());
}

TEST_CASE("Getters/setters", "[canrawview]")
{
    CanRawSenderModel crsModel;

    REQUIRE(crsModel.painterDelegate() != nullptr);
    REQUIRE_NOTHROW(crsModel.setInData({}, 0));
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit tests");
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
