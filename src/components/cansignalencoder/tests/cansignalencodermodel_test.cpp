#include <QtWidgets/QApplication>
#include <cansignalencodermodel.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <catch.hpp>
#include <datamodeltypes/canrawdata.h>
#include <datamodeltypes/cansignalmodel.h>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Test basic functionality", "[cansignalencoderModel]")
{
    using namespace fakeit;
    CanSignalEncoderModel cm;
    REQUIRE(cm.caption() == "CanSignalEncoder");
    REQUIRE(cm.name() == "CanSignalEncoder");
    REQUIRE(cm.resizable() == false);
    REQUIRE(cm.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<CanSignalEncoderModel*>(cm.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[cansignalencoderModel]")
{
    CanSignalEncoderModel cm;
    REQUIRE(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[cansignalencoderModel]")
{
    CanSignalEncoderModel cm;

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 1);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 1);
}

TEST_CASE("dataType", "[cansignalencoderModel]")
{
    CanSignalEncoderModel cm;

    NodeDataType ndt;

    ndt = cm.dataType(QtNodes::PortType::Out, 0);
    REQUIRE(ndt.id == "rawframe");
    REQUIRE(ndt.name == "RAW");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");

    ndt = cm.dataType(QtNodes::PortType::In, 0);
    REQUIRE(ndt.id == "signal");
    REQUIRE(ndt.name == "SIG");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
}

TEST_CASE("outData", "[cansignalencoderModel]")
{
    CanSignalEncoderModel cm;

    auto nd = cm.outData(0);
    REQUIRE(!nd);
}

TEST_CASE("setInData", "[cansignalencoderModel]")
{
    CanSignalEncoderModel cm;
    QSignalSpy spy(&cm, &CanSignalEncoderModel::sndSignal);

    cm.setInData({}, 1);

    auto data = std::make_shared<CanSignalModel>("aa", 10);
    cm.setInData(data, 0);

    REQUIRE(spy.count() == 1);

    REQUIRE(spy.at(0).at(0).toString() == "aa");
    REQUIRE(spy.at(0).at(1).toUInt() == 10);
}

TEST_CASE("rcvFrame", "[cansignalencoderModel]")
{
    CanSignalEncoderModel cm;
    QSignalSpy spy(&cm, &CanSignalEncoderModel::dataUpdated);

    QCanBusFrame frame;
    frame.setFrameId(0x666);

    for (int i = 0; i < 130; ++i) {
        cm.rcvFrame(frame);
    }

    REQUIRE(spy.count() == 127);

    auto f = std::dynamic_pointer_cast<CanRawData>(cm.outData(0));

    REQUIRE(f->frame().frameId() == 0x666);
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
