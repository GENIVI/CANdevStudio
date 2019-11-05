#include <QtWidgets/QApplication>
#include <cansignalsender.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>
#include <gui/cansignalsenderguiint.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignalsender]")
{
    CanSignalSender c;

    REQUIRE(c.mainWidget() != nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[cansignalsender]")
{
    CanSignalSender c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[cansignalsender]")
{
    using namespace fakeit;
    Mock<CanSignalSenderGuiInt> guiInt;
    Fake(Method(guiInt, initTv));
    Fake(Method(guiInt, setRemoveCbk));
    Fake(Method(guiInt, setAddCbk));
    Fake(Method(guiInt, setDockUndockCbk));
    Fake(Method(guiInt, setSendCbk));
    Fake(Method(guiInt, addRow));
    Fake(Method(guiInt, getRows));
    Fake(Method(guiInt, getSelectedRows));

    CanSignalSenderCtx ctx(&guiInt.get());
    CanSignalSender c(std::move(ctx));

    QJsonObject obj;
    QJsonArray arr;
    QJsonObject el;
    obj["name"] = "Test Name";

    // Not an array
    obj["rows"] = "not an array";
    c.setConfig(obj);

    // Empty Array
    obj["rows"] = arr;
    c.setConfig(obj);

    // Array with not_an_object
    arr.append("Should be object");
    obj["rows"] = arr;
    c.setConfig(obj);

    // id only
    arr = QJsonArray();
    el = QJsonObject();
    el["id"] = "0x33";
    arr.append(el);
    obj["rows"] = arr;
    c.setConfig(obj);

    Verify(Method(guiInt, addRow)).Exactly(1);
}

TEST_CASE("getConfig", "[cansignalsender]")
{
    CanSignalSender c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[cansignalsender]")
{
    CanSignalSender c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[cansignalsender]")
{
    CanSignalSender c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[cansignalsender]")
{
    CanSignalSender c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 2);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);
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
