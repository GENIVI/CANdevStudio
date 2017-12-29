#include <QtWidgets/QApplication>
#include <projectconfig/canrawsendermodel.h>
#include <datamodeltypes/canrawsenderdata.h>
#define CATCH_CONFIG_RUNNER
#include <QSignalSpy>
#include <fakeit.hpp>
#include <log.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;

TEST_CASE("Test basic functionality", "[canrawsender]")
{
    CanRawSenderModel canRawSenderModel;
    CHECK(canRawSenderModel.caption() == "CanRawSender");
    CHECK(canRawSenderModel.name() == "CanRawSender");
    CHECK(canRawSenderModel.resizable() == false);
    CHECK(dynamic_cast<CanRawSenderModel*>(canRawSenderModel.clone().get()) != nullptr);
    CHECK(dynamic_cast<QLabel*>(canRawSenderModel.embeddedWidget()) != nullptr);
}

TEST_CASE("Port information", "[canrawsender]")
{
    CanRawSenderModel canRawSenderModel;
    CanRawSenderDataOut canRawSenderDataOut;
    CHECK(canRawSenderModel.nPorts(PortType::Out) == 1);
    CHECK(canRawSenderModel.nPorts(PortType::In) == 0);
    CHECK(canRawSenderModel.nPorts(PortType::None) == 0);
    CHECK(canRawSenderModel.dataType(PortType::Out, 0).id == canRawSenderDataOut.type().id);
    CHECK(canRawSenderModel.dataType(PortType::Out, 0).name == canRawSenderDataOut.type().name);
}

TEST_CASE("Calling sendFrame emits dataUpdated and outData returns that frame", "[canrawsender]")
{
    CanRawSenderModel canRawSenderModel;
    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    QSignalSpy dataUpdatedSpy(&canRawSenderModel, &CanRawSenderModel::dataUpdated);
    canRawSenderModel.sendFrame(testFrame);
    CHECK(dataUpdatedSpy.count() == 1);
    CHECK(std::dynamic_pointer_cast<CanRawSenderDataOut>(canRawSenderModel.outData(0))->frame().frameId()
        == testFrame.frameId());
}

TEST_CASE("Test save configuration", "[canrawsender]")
{
    CanRawSenderModel canRawSenderModel;
    QJsonObject json = canRawSenderModel.save();
    CHECK(json.find("name") != json.end());
    CHECK(json.find("senderColumns") != json.end());
    CHECK(json.find("content") != json.end());
    CHECK(json.find("sorting") != json.end());
}

TEST_CASE("Getters/setters", "[canrawview]")
{
    CanRawSenderModel crsModel;

    CHECK(crsModel.painterDelegate() != nullptr);
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
    QApplication a(argc, argv); // QApplication must exist when contructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
