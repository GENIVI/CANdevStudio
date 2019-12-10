#include <QtWidgets/QApplication>
#include <cansignalviewermodel.h>
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

TEST_CASE("Test basic functionality", "[cansignalviewerModel]")
{
    using namespace fakeit;
    CanSignalViewerModel cm;
    REQUIRE(cm.caption() == "CanSignalViewer");
    REQUIRE(cm.name() == "CanSignalViewer");
    REQUIRE(cm.resizable() == false);
    REQUIRE(cm.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<CanSignalViewerModel*>(cm.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[cansignalviewerModel]")
{
    CanSignalViewerModel cm;
    REQUIRE(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[cansignalviewerModel]")
{
    CanSignalViewerModel cm;

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 0);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 1);
}

TEST_CASE("dataType", "[cansignalviewerModel]")
{
    CanSignalViewerModel cm;

    NodeDataType ndt;

    ndt = cm.dataType(QtNodes::PortType::In, 0);
    REQUIRE(ndt.id == "signal");
    REQUIRE(ndt.name == "SIG");

    ndt = cm.dataType(QtNodes::PortType::In, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");

    ndt = cm.dataType(QtNodes::PortType::Out, 0);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
}

TEST_CASE("outData", "[cansignalviewerModel]")
{
    CanSignalViewerModel cm;

    auto nd = cm.outData(0);
    REQUIRE(!nd);
}

TEST_CASE("setInData", "[cansignalviewerModel]")
{
    CanSignalViewerModel cm;
    QSignalSpy spy(&cm, &CanSignalViewerModel::sndSignal);

    cm.setInData({}, 1);

    QVariant val(123);
    auto data = std::make_shared<CanSignalModel>("AAA", val);
    cm.setInData(data, 0);

    REQUIRE(spy.count() == 1);
    REQUIRE(spy.at(0).at(0).toString() == "AAA");
    REQUIRE(spy.at(0).at(1).toUInt() == 123);
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
