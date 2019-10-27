#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <QtWidgets/QApplication>
#include <canload.h>
#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <fakeit.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[canload]")
{
    CanLoad c;
    CanLoadCtx ctx;
    CanLoad c2(std::move(ctx));

    REQUIRE(c.mainWidget() == nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[canload]")
{
    CanLoad c;
    QWidget obj;

    obj.setProperty("name", "Test Name");
    obj.setProperty("period [ms]", "1111");
    obj.setProperty("bitrate [bps]", "2222");

    c.setConfig(obj);
    c.setConfig(c.getConfig());

    auto config = c.getQConfig();

    REQUIRE(config->property("name") == "Test Name");
    REQUIRE(config->property("period [ms]") == "1111");
    REQUIRE(config->property("bitrate [bps]") == "2222");
}

TEST_CASE("configChanged", "[canload]")
{
    CanLoad c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[canload]")
{
    CanLoad c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 3);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);

    REQUIRE(ComponentInterface::propertyName(props[1]) == "bitrate [bps]");
    REQUIRE(ComponentInterface::propertyType(props[1]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[1]) == true);
    REQUIRE(ComponentInterface::propertyField(props[1]) != nullptr);

    REQUIRE(ComponentInterface::propertyName(props[2]) == "period [ms]");
    REQUIRE(ComponentInterface::propertyType(props[2]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[2]) == true);
    REQUIRE(ComponentInterface::propertyField(props[2]) != nullptr);
}

TEST_CASE("start/stop - correct timings", "[canload]")
{
    CanLoad c;
    QWidget obj;
    QSignalSpy spy(&c, &CanLoad::currentLoad);
    QCanBusFrame frame;
    frame.setFrameId(0x11);
    frame.setPayload({ "123" });

    obj.setProperty("name", "Test Name");
    obj.setProperty("period [ms]", "10");
    obj.setProperty("bitrate [bps]", "500000");

    c.setConfig(obj);

    c.startSimulation();

    for (int i = 0; i < 100; ++i) {
        c.frameIn(frame);
    }

    while (spy.count() == 0) {
        QApplication::processEvents();
    }

    REQUIRE(spy.count() == 1);

    c.stopSimulation();
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
