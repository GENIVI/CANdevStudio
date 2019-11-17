#include <QtWidgets/QApplication>
#include <cansignalencoder.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>
#include <cansignaldata.h>
#include <bcastmsgs.h>
#include <QJsonDocument>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignalencoder]")
{
    CanSignalEncoder c;

    REQUIRE(c.mainWidget() == nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[cansignalencoder]")
{
    CanSignalEncoder c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[cansignalencoder]")
{
    CanSignalEncoder c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);
}

TEST_CASE("getConfig", "[cansignalencoder]")
{
    CanSignalEncoder c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[cansignalencoder]")
{
    CanSignalEncoder c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[cansignalencoder]")
{
    CanSignalEncoder c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[cansignalencoder]")
{
    CanSignalEncoder c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 2);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);

    REQUIRE(ComponentInterface::propertyName(props[1]) == "CAN database");
    REQUIRE(ComponentInterface::propertyType(props[1]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[1]) == true);
    REQUIRE(ComponentInterface::propertyField(props[1]) != nullptr);
}

QString testJson = R"(
{
    "msgSettings": [
        {
            "cycle": "100",
            "id": "3",
            "initVal": ""
        },
        {
            "cycle": "200",
            "id": "e",
            "initVal": "1122334455667788"
        },
        {
            "cycle": "500",
            "id": "45",
            "initVal": "aabbccddeeff0099"
        },
        {
            "cycle": "2000",
            "id": "6d",
            "initVal": "AABBCCDD"
        }
    ],
    "name": "CanSignalData"
}
)";

TEST_CASE("send preconfigured", "[cansignalencoder]")
{
    CanSignalData data;
    CanSignalEncoder c;
    QJsonObject obj = QJsonDocument::fromJson(testJson.toUtf8()).object();
    QThread th;

    QObject::connect(&data, &CanSignalData::simBcastSnd, &c, &CanSignalEncoder::simBcastRcv);
    QObject::connect(&c, &CanSignalEncoder::simBcastSnd, &data, &CanSignalData::simBcastRcv);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    data.setConfig(obj);
    data.configChanged();

    QSignalSpy sigSndSpy(&c, &CanSignalEncoder::sndFrame);

    data.moveToThread(&th);
    c.moveToThread(&th);

    QObject::connect(&th, &QThread::started, &c, &CanSignalEncoder::startSimulation);
    QObject::connect(&th, &QThread::finished, &c, &CanSignalEncoder::stopSimulation);
    QObject::connect(&th, &QThread::started, &data, &CanSignalData::startSimulation);
    QObject::connect(&th, &QThread::finished, &data, &CanSignalData::stopSimulation);

    th.start();

    QThread::sleep(1);

    th.quit();
    th.wait();

    REQUIRE(sigSndSpy.count() > 0);
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
