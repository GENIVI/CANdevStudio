#include <QtWidgets/QApplication>
#include <projectconfig/canrawloggermodel.h>
#include <datamodeltypes/canrawloggerdata.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[canrawloggerModel]")
{
    using namespace fakeit;
    CanRawLoggerModel canrawloggerModel;
    CHECK(canrawloggerModel.caption() == "CanRawLogger");
    CHECK(canrawloggerModel.name() == "CanRawLogger");
    CHECK(canrawloggerModel.resizable() == false);
    CHECK(canrawloggerModel.hasSeparateThread() == true);
    CHECK(dynamic_cast<CanRawLoggerModel*>(canrawloggerModel.clone().get()) != nullptr);
    CHECK(dynamic_cast<QLabel*>(canrawloggerModel.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;
    CHECK(canrawloggerModel.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;

    CHECK(canrawloggerModel.nPorts(QtNodes::PortType::Out) == 1);
    CHECK(canrawloggerModel.nPorts(QtNodes::PortType::In) == 0);
}

TEST_CASE("dataType", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;

    NodeDataType ndt;
        
    ndt = canrawloggerModel.dataType(QtNodes::PortType::Out, 0);
    CHECK(ndt.id == "rawsender");
    CHECK(ndt.name == "Raw");

    ndt = canrawloggerModel.dataType(QtNodes::PortType::Out, 1);
    CHECK(ndt.id == "");
    CHECK(ndt.name == "");
    
    ndt = canrawloggerModel.dataType(QtNodes::PortType::In, 0);
    CHECK(ndt.id == "");
    CHECK(ndt.name == "");
}

TEST_CASE("outData", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;

    auto nd = canrawloggerModel.outData(0);
    CHECK(!nd);

    QCanBusFrame frame;
    canrawloggerModel.sendFrame(frame);
    nd = canrawloggerModel.outData(0);
    CHECK(nd);
}

TEST_CASE("setInData", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;

    canrawloggerModel.setInData({}, 1);
}

TEST_CASE("sendFrame", "[canrawloggerModel]")
{
    CanRawLoggerModel canrawloggerModel;
    QCanBusFrame frame;

    QSignalSpy dataUpdatedSpy(&canrawloggerModel, &CanRawLoggerModel::dataUpdated);

    for(int i = 0; i < 200; ++i) {
        canrawloggerModel.sendFrame(frame);
    }

    CHECK(dataUpdatedSpy.count() == 127);
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

