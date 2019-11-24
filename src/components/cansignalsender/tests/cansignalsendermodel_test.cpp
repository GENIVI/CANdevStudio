#include <QtWidgets/QApplication>
#include <cansignalsendermodel.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>
#include <datamodeltypes/cansignalmodel.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[cansignalsenderModel]")
{
    using namespace fakeit;
    CanSignalSenderModel cm;
    REQUIRE(cm.caption() == "CanSignalSender");
    REQUIRE(cm.name() == "CanSignalSender");
    REQUIRE(cm.resizable() == false);
    REQUIRE(cm.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<CanSignalSenderModel*>(cm.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[cansignalsenderModel]")
{
    CanSignalSenderModel cm;
    REQUIRE(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[cansignalsenderModel]")
{
    CanSignalSenderModel cm;

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 1);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 0);
}

TEST_CASE("dataType", "[cansignalsenderModel]")
{
    CanSignalSenderModel cm;

    NodeDataType ndt;

     ndt = cm.dataType(QtNodes::PortType::Out, 0);
     REQUIRE(ndt.id == "signal");
     REQUIRE(ndt.name == "SIG");

     ndt = cm.dataType(QtNodes::PortType::Out, 1);
     REQUIRE(ndt.id == "");
     REQUIRE(ndt.name == "");

     ndt = cm.dataType(QtNodes::PortType::In, 0);
     REQUIRE(ndt.id == "");
     REQUIRE(ndt.name == "");
}

TEST_CASE("outData", "[cansignalsenderModel]")
{
    CanSignalSenderModel cm;

    auto nd = cm.outData(0);
    REQUIRE(!nd);
}

TEST_CASE("setInData", "[cansignalsenderModel]")
{
    CanSignalSenderModel cm;

    cm.setInData({}, 1);
}

TEST_CASE("rcvFrame", "[cansignalsenderModel]")
{
    CanSignalSenderModel cm;
    QSignalSpy spy(&cm, &CanSignalSenderModel::dataUpdated);

    for (int i = 0; i < 130; ++i) {
        cm.rcvSignal("test", 123);
    }

    REQUIRE(spy.count() == 127);

    auto f = std::dynamic_pointer_cast<CanSignalModel>(cm.outData(0));

    REQUIRE(f->name() == "test");
    REQUIRE(f->value().toUInt() == 123);
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
