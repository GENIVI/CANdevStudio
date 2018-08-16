#include <QtWidgets/QApplication>
#include <cansignaldata.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <fakeit.hpp>
#include <QCanBusFrame>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignaldata]")
{
    CanSignalData c;

    CHECK(c.mainWidget() != nullptr);
    CHECK(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[cansignaldata]")
{
    CanSignalData c;
    QObject obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[cansignaldata]")
{
    CanSignalData c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);
}

TEST_CASE("getConfig", "[cansignaldata]")
{
    CanSignalData c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[cansignaldata]")
{
    CanSignalData c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[cansignaldata]")
{
    CanSignalData c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[cansignaldata]")
{
    CanSignalData c;

    auto props = c.getSupportedProperties();

    CHECK(props.find("name") != props.end());
    CHECK(props.find("dummy") == props.end());
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
