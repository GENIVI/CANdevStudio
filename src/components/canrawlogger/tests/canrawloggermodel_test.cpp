#include <QtWidgets/QApplication>
#include <canrawloggermodel.h>
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

TEST_CASE("Test basic functionality", "[canrawloggerModel]")
{
    using namespace fakeit;
    CanRawLoggerModel canrawloggerModel;
    REQUIRE(canrawloggerModel.caption() == "CanRawLogger");
    REQUIRE(canrawloggerModel.name() == "CanRawLogger");
    REQUIRE(canrawloggerModel.resizable() == false);
    REQUIRE(canrawloggerModel.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<CanRawLoggerModel*>(canrawloggerModel.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(canrawloggerModel.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;
    REQUIRE(canrawloggerModel.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;

    REQUIRE(canrawloggerModel.nPorts(QtNodes::PortType::Out) == 0);
    REQUIRE(canrawloggerModel.nPorts(QtNodes::PortType::In) == 1);
}

TEST_CASE("dataType", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;

    NodeDataType ndt;

    ndt = canrawloggerModel.dataType(QtNodes::PortType::Out, 0);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");

    ndt = canrawloggerModel.dataType(QtNodes::PortType::In, 0);
    REQUIRE(ndt.id == "rawframe");
    REQUIRE(ndt.name == "RAW");

    ndt = canrawloggerModel.dataType(QtNodes::PortType::In, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
}

TEST_CASE("outData", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;

    auto nd = canrawloggerModel.outData(0);
    REQUIRE(!nd);
}

TEST_CASE("setInData", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;
    QCanBusFrame frame;
    QSignalSpy rxSpy(&canrawloggerModel, &CanRawLoggerModel::frameReceived);
    QSignalSpy txSpy(&canrawloggerModel, &CanRawLoggerModel::frameSent);

    auto&& rxData = std::make_shared<CanRawData>(frame, Direction::RX, true);
    auto&& txData = std::make_shared<CanRawData>(frame, Direction::TX, true);
    auto&& errData = std::make_shared<CanRawData>(frame, static_cast<Direction>(11), true);

    canrawloggerModel.setInData(rxData, 1);
    canrawloggerModel.setInData(txData, 1);
    canrawloggerModel.setInData(errData, 1);
    canrawloggerModel.setInData({}, 1);

    REQUIRE(txSpy.count() == 1);
    REQUIRE(rxSpy.count() == 1);
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
