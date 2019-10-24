#include <QtWidgets/QApplication>
#include <cansignaldatamodel.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>
#include <QCanBusFrame>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[cansignaldataModel]")
{
    using namespace fakeit;
    CanSignalDataModel cm;
    REQUIRE(cm.caption() == "CanSignalData");
    REQUIRE(cm.name() == "CanSignalData");
    REQUIRE(cm.resizable() == false);
    REQUIRE(cm.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<CanSignalDataModel*>(cm.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[cansignaldataModel]")
{
    CanSignalDataModel cm;
    REQUIRE(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[cansignaldataModel]")
{
    CanSignalDataModel cm;

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 1);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 0);
}

TEST_CASE("dataType", "[cansignaldataModel]")
{
    CanSignalDataModel cm;

    NodeDataType ndt;
        
    ndt = cm.dataType(QtNodes::PortType::Out, 0);
    REQUIRE(ndt.id == "DbData");
    REQUIRE(ndt.name == "DB");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
    
    ndt = cm.dataType(QtNodes::PortType::In, 0);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
}

TEST_CASE("outData", "[cansignaldataModel]")
{
    CanSignalDataModel cm;

    auto nd = cm.outData(0);
    REQUIRE(nd);
}

TEST_CASE("setInData", "[cansignaldataModel]")
{
    CanSignalDataModel cm;

    cm.setInData({}, 1);
}

TEST_CASE("canDbUpdated", "[cansignaldataModel]")
{
    CanSignalDataModel cm;
    CANmessages_t msgs;
    QSignalSpy dbSpy(&cm, &CanSignalDataModel::dataUpdated);

    cm.canDbUpdated(msgs);
    REQUIRE(dbSpy.count() == 1);
    REQUIRE(dbSpy.takeFirst().at(0).toUInt() == 0);
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

