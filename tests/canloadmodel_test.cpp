#include <QtWidgets/QApplication>
#include <projectconfig/canloadmodel.h>
#include <datamodeltypes/canloaddata.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[canloadModel]")
{
    using namespace fakeit;
    CanLoadModel cm;
    CHECK(cm.caption() == "CanLoad");
    CHECK(cm.name() == "CanLoad");
    CHECK(cm.resizable() == false);
    CHECK(cm.hasSeparateThread() == true);
    CHECK(dynamic_cast<CanLoadModel*>(cm.clone().get()) != nullptr);
    CHECK(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[canloadModel]")
{
    CanLoadModel cm;
    CHECK(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[canloadModel]")
{
    CanLoadModel cm;

    CHECK(cm.nPorts(QtNodes::PortType::Out) == 1);
    CHECK(cm.nPorts(QtNodes::PortType::In) == 0);
}

TEST_CASE("dataType", "[canloadModel]")
{
    CanLoadModel cm;

    NodeDataType ndt;
        
    ndt = cm.dataType(QtNodes::PortType::Out, 0);
    CHECK(ndt.id == "rawsender");
    CHECK(ndt.name == "Raw");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    CHECK(ndt.id == "");
    CHECK(ndt.name == "");
    
    ndt = cm.dataType(QtNodes::PortType::In, 0);
    CHECK(ndt.id == "");
    CHECK(ndt.name == "");
}

TEST_CASE("outData", "[canloadModel]")
{
    CanLoadModel cm;

    auto nd = cm.outData(0);
    CHECK(!nd);
}

TEST_CASE("setInData", "[canloadModel]")
{
    CanLoadModel cm;

    cm.setInData({}, 1);
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

