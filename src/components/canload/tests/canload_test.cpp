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

    CHECK(c.mainWidget() == nullptr);
    CHECK(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[canload]")
{
    CanLoad c;
    QObject obj;

    obj.setProperty("name", "Test Name");
    obj.setProperty("period [ms]", "1111");
    obj.setProperty("bitrate [bps]", "2222");

    c.setConfig(obj);
    c.setConfig(c.getConfig());

    auto config = c.getQConfig();

    CHECK(config->property("name") == "Test Name");
    CHECK(config->property("period [ms]") == "1111");
    CHECK(config->property("bitrate [bps]") == "2222");
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

    CHECK(props.find("name") != props.end());
    CHECK(props.find("period [ms]") != props.end());
    CHECK(props.find("bitrate [bps]") != props.end());
    CHECK(props.find("dummy") == props.end());
}

TEST_CASE("start/stop - correct timings", "[canload]")
{
    CanLoad c;
    QObject obj;
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

    CHECK(spy.count() == 1);

    c.stopSimulation();
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
