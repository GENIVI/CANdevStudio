#include <QtWidgets/QApplication>
#include <cansignaldecodermodel.h>
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

TEST_CASE("Test basic functionality", "[cansignaldecoderModel]")
{
    using namespace fakeit;
    CanSignalDecoderModel cm;
    REQUIRE(cm.caption() == "CanSignalDecoder");
    REQUIRE(cm.name() == "CanSignalDecoder");
    REQUIRE(cm.resizable() == false);
    REQUIRE(cm.hasSeparateThread() == false);
    REQUIRE(dynamic_cast<CanSignalDecoderModel*>(cm.clone().get()) != nullptr);
    REQUIRE(dynamic_cast<QLabel*>(cm.embeddedWidget()) != nullptr);
}

TEST_CASE("painterDelegate", "[cansignaldecoderModel]")
{
    CanSignalDecoderModel cm;
    REQUIRE(cm.painterDelegate() != nullptr);
}

TEST_CASE("nPorts", "[cansignaldecoderModel]")
{
    CanSignalDecoderModel cm;

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 1);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 1);
}

TEST_CASE("dataType", "[cansignaldecoderModel]")
{
    CanSignalDecoderModel cm;

    NodeDataType ndt;

    ndt = cm.dataType(QtNodes::PortType::Out, 0);
    REQUIRE(ndt.id == "signal");
    REQUIRE(ndt.name == "SIG");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");

    ndt = cm.dataType(QtNodes::PortType::In, 0);
    REQUIRE(ndt.id == "rawframe");
    REQUIRE(ndt.name == "RAW");

    ndt = cm.dataType(QtNodes::PortType::Out, 1);
    REQUIRE(ndt.id == "");
    REQUIRE(ndt.name == "");
}

TEST_CASE("outData", "[cansignaldecoderModel]")
{
    CanSignalDecoderModel cm;

    auto nd = cm.outData(0);
    REQUIRE(!nd);
}

TEST_CASE("setInData", "[cansignaldecoderModel]")
{
    CanSignalDecoderModel cm;
    QSignalSpy spy(&cm, &CanSignalDecoderModel::sndFrame);

    cm.setInData({}, 1);

    QCanBusFrame frame;
    frame.setFrameId(0x123);

    auto data = std::make_shared<CanRawData>(frame);
    cm.setInData(data, 0);

    REQUIRE(spy.count() == 1);

    REQUIRE(qvariant_cast<QCanBusFrame>(spy.at(0).at(0)).frameId() == 0x123);
}

TEST_CASE("rcvFrame", "[cansignaldecoderModel]")
{
    CanSignalDecoderModel cm;
    QSignalSpy spy(&cm, &CanSignalDecoderModel::dataUpdated);

    for (int i = 0; i < 130; ++i) {
        cm.rcvSignal("123_abc", 123, Direction::TX);
    }

    REQUIRE(spy.count() == 127);

    auto f = std::dynamic_pointer_cast<CanSignalModel>(cm.outData(0));

    REQUIRE(f->name() == "123_abc");
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
