#include <QtWidgets/QApplication>
#include <canrawplayermodel.h>
#include <datamodeltypes/canrawdata.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[canrawplayerModel]")
{
    using namespace fakeit;
    CanRawPlayerModel canrawplayerModel;
    REQUIRE(canrawplayerModel.caption() == "CanRawPlayer");
    REQUIRE(canrawplayerModel.name() == "CanRawPlayer");
    REQUIRE(canrawplayerModel.resizable() == false);
    REQUIRE(canrawplayerModel.hasSeparateThread() == true);
    REQUIRE(dynamic_cast<CanRawPlayerModel*>(canrawplayerModel.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(canrawplayerModel.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[canrawplayerModel]")
{
    CanRawPlayerModel canrawplayerModel;
    REQUIRE(canrawplayerModel.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[canrawplayerModel]")
{
    CanRawPlayerModel canrawplayerModel;

    REQUIRE(canrawplayerModel.nPorts(QtNodes::PortType::Out) == 1);
    REQUIRE(canrawplayerModel.nPorts(QtNodes::PortType::In) == 0);
}

TEST_CASE("dataType", "[canrawplayerModel]")
{
    CanRawPlayerModel canrawplayerModel;

    NodeDataType ndt;

    ndt = canrawplayerModel.dataType(QtNodes::PortType::Out, 0);
    REQUIRE(ndt.id == "rawframe");
    REQUIRE(ndt.name == "RAW");

    ndt = canrawplayerModel.dataType(QtNodes::PortType::Out, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");

    ndt = canrawplayerModel.dataType(QtNodes::PortType::In, 0);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
}

TEST_CASE("outData", "[canrawplayerModel]")
{
    CanRawPlayerModel canrawplayerModel;

    auto nd = canrawplayerModel.outData(0);
    REQUIRE(!nd);

    QCanBusFrame frame;
    canrawplayerModel.sendFrame(frame);
    nd = canrawplayerModel.outData(0);
    REQUIRE(nd);
}

TEST_CASE("setInData", "[canrawplayerModel]")
{
    CanRawPlayerModel canrawplayerModel;

    canrawplayerModel.setInData({}, 1);
}

TEST_CASE("sendFrame", "[canrawplayerModel]")
{
    CanRawPlayerModel canrawplayerModel;
    QCanBusFrame frame;

    QSignalSpy dataUpdatedSpy(&canrawplayerModel, &CanRawPlayerModel::dataUpdated);

    for (int i = 0; i < 200; ++i) {
        canrawplayerModel.sendFrame(frame);
    }

    REQUIRE(dataUpdatedSpy.count() == 127);
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
