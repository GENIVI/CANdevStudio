#include <QtWidgets/QApplication>
#include <cansignaldata.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>
#include <QCanBusFrame>
#include <QWidget>
#include <gui/cansignaldataguiint.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignaldata]")
{
    CanSignalData c;
    CHECK(c.mainWidget() != nullptr);
    CHECK(c.mainWidgetDocked() == true);

    using namespace fakeit;
    Mock<CanSignalDataGuiInt> guiInt;
    Fake(Method(guiInt, initSearch));
    Fake(Method(guiInt, setMsgView));
    Fake(Method(guiInt, setMsgViewCbk));
    Fake(Method(guiInt, setDockUndockCbk));
    Fake(Method(guiInt, setMsgSettingsUpdatedCbk));

    CanSignalDataCtx ctx(&guiInt.get());
    CanSignalData c2(std::move(ctx));
}

TEST_CASE("setConfig - qobj", "[cansignaldata]")
{
    CanSignalData c;
    QWidget obj;

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

    REQUIRE(props.size() == 2);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);

    REQUIRE(ComponentInterface::propertyName(props[1]) == "file");
    REQUIRE(ComponentInterface::propertyType(props[1]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[1]) == true);
    REQUIRE(ComponentInterface::propertyField(props[1]) != nullptr);
}

TEST_CASE("loadDbc", "[cansignaldata]")
{
    CanSignalData c;
    QJsonObject obj;

    obj["name"] = "Test Name";
    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";

    c.setConfig(obj);
    c.configChanged();
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
