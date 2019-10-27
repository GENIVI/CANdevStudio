#include <QtWidgets/QApplication>
#include <canloadmodel.h>
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

TEST_CASE("Test basic functionality", "[canloadModel]")
{
    using namespace fakeit;
    CanLoadModel cm;
    REQUIRE(cm.caption() == "CanLoad");
    REQUIRE(cm.name() == "CanLoad");
    REQUIRE(cm.resizable() == false);
    REQUIRE(cm.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<CanLoadModel*>(cm.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[canloadModel]")
{
    CanLoadModel cm;
    REQUIRE(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[canloadModel]")
{
    CanLoadModel cm;

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 0);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 1);
}

TEST_CASE("dataType", "[canloadModel]")
{
    CanLoadModel cm;

    NodeDataType ndt;
        
    ndt = cm.dataType(QtNodes::PortType::In, 0);
    REQUIRE(ndt.id == "rawframe");
    REQUIRE(ndt.name == "RAW");

    ndt = cm.dataType(QtNodes::PortType::In, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");

    ndt = cm.dataType(QtNodes::PortType::Out, 0);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
}

TEST_CASE("outData", "[canloadModel]")
{
    CanLoadModel cm;

    auto nd = cm.outData(0);
    REQUIRE(!nd);
}

TEST_CASE("setInData", "[canloadModel]")
{
    CanLoadModel cm;
    QCanBusFrame frame;
    auto data = std::make_shared<CanRawData>(frame);
    QSignalSpy spy(&cm, &CanLoadModel::frameIn);

    REQUIRE(spy.count() == 0);

    cm.setInData(data, 1);
    cm.setInData({}, 1);

    REQUIRE(spy.count() == 1);
}

TEST_CASE("currentLoad", "[canloadModel]")
{
    CanLoadModel cm;
    std::array<uint8_t, 10> array = {0, 0, 0, 200, 199, 100, 100, 8, 9, 0};
    QSignalSpy spy(&cm, &CanLoadModel::requestRedraw);

    REQUIRE(spy.count() == 0);

    for(auto a : array) {
        cm.currentLoad(a);
    }

    REQUIRE(spy.count() == 6);
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

