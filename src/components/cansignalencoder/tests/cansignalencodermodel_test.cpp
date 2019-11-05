#include <QtWidgets/QApplication>
#include <cansignalencodermodel.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <catch.hpp>
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

    REQUIRE(cm.nPorts(QtNodes::PortType::Out) == 0);
    REQUIRE(cm.nPorts(QtNodes::PortType::In) == 0);
}

TEST_CASE("dataType", "[cansignalencoderModel]")
{
    CanSignalEncoderModel cm;

    NodeDataType ndt;

    // ndt = cm.dataType(QtNodes::PortType::Out, 0);
    // REQUIRE(ndt.id == "rawframe");
    // REQUIRE(ndt.name == "RAW");

    // ndt = cm.dataType(QtNodes::PortType::Out, 1);
    // REQUIRE(ndt.id == "");
    // REQUIRE(ndt.name == "");

    // ndt = cm.dataType(QtNodes::PortType::In, 0);
    // REQUIRE(ndt.id == "");
    // REQUIRE(ndt.name == "");
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

    cm.setInData({}, 1);
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
