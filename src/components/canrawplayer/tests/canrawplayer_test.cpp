#include <QtWidgets/QApplication>
#include <canrawplayer.h>
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

TEST_CASE("Stubbed methods", "[canrawplayer]")
{
    CanRawPlayerCtx ctx;
    CanRawPlayer c(std::move(ctx));

    REQUIRE(c.mainWidget() == nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[canrawplayer]")
{
    CanRawPlayer c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[canrawplayer]")
{
    CanRawPlayer c;
    QJsonObject obj;

    obj["name"] = "Test Name";

    c.setConfig(obj);
}

TEST_CASE("getConfig", "[canrawplayer]")
{
    CanRawPlayer c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[canrawplayer]")
{
    CanRawPlayer c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[canrawplayer]")
{
    CanRawPlayer c;
    QWidget obj;
    std::string filename = "wrongFile";

    c.configChanged();

    std::ofstream file(filename);
    file << "blah";
    file.close();

    chmod(filename.c_str(), 0);
    obj.setProperty("file", QString(filename.c_str()));
    c.setConfig(obj);

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[canrawplayer]")
{
    CanRawPlayer c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 3);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);

    REQUIRE(ComponentInterface::propertyName(props[1]) == "file");
    REQUIRE(ComponentInterface::propertyType(props[1]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[1]) == true);
    REQUIRE(ComponentInterface::propertyField(props[1]) != nullptr);

    REQUIRE(ComponentInterface::propertyName(props[2]) == "timer tick [ms]");
    REQUIRE(ComponentInterface::propertyType(props[2]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[2]) == true);
    REQUIRE(ComponentInterface::propertyField(props[2]) != nullptr);
}

static QString createTestFile()
{
    std::string filename = "testfile.log";
    std::ofstream file(filename);

    file << "(000.001103)  can0  4A1   [2]  C7 B2\n";
    file << "(000.002232)  can0  794   [5]  06 41 1A 2F 28\n";
    file << "(000.003365)  can0  55A   [7]  A0 0D EE 33 3C 33 A7\n";
    file << "(000.004497)  can0  4B1   [8]  60 E4 37 4C D8 64 37 13\n";
    file << "(000.005588)  can0  0A3   [5]  70 C6 55 0F DE\n";
    file << "(000.059614)  can0  199043EC   [7]  2B 86 F8 52 4E D9 7D\n";
    file << "(000.060767)  can0  0709939E   [8]  87 A4 5B 0C 01 0F C7 6D\n";
    file << "(000.061918)  can0  11989CEC   [8]  22 F8 20 1E 43 3B C1 40\n";
    file << "(000.063150)  can0  07602F88   [7]  2C 29 3A 33 A9 7E 3C\n";
    file << "(000.064412)  can0  1896047E   [0] \n";

    file.close();

    return filename.c_str();
}

static QString createTestFile2()
{
    std::string filename = "testfile.log";
    std::ofstream file(filename);

    file << "(000.001103)  can0  4A1   [2]  C7 B2\n";
    file << "(000.002232)  can0  794   [5]  06 41 1A 2F 28\n";
    file << "(000.003365)  can0  55A   [7]  A0 0D EE 33 3C 33 A7\n";
    file << "(000.004497)  can0  4B1   [8]  60 E4 37 4C D8 64 37 13\n";
    file << "(000.005588)  can0  0A3   [5]  70 C6 55 0F DE\n";
    file << "(100.259614)  can0  199043EC   [7]  2B 86 F8 52 4E D9 7D\n";
    file << "(100.260767)  can0  0709939E   [8]  87 A4 5B 0C 01 0F C7 6D\n";
    file << "(100.261918)  can0  11989CEC   [8]  22 F8 20 1E 43 3B C1 40\n";
    file << "(100.263150)  can0  07602F88   [7]  2C 29 3A 33 A9 7E 3C\n";
    file << "(100.264412)  can0  1896047E   [0] \n";

    file.close();

    return filename.c_str();
}

TEST_CASE("Send test", "[canrawplayer]")
{
    using namespace std::chrono_literals;

    CanRawPlayer c;
    QWidget props;
    QSignalSpy sendSpy(&c, &CanRawPlayer::sendFrame);
    QThread th;

    c.moveToThread(&th);

    QObject::connect(&th, &QThread::started, &c, &CanRawPlayer::startSimulation);
    QObject::connect(&th, &QThread::finished, &c, &CanRawPlayer::stopSimulation);

    props.setProperty("file", createTestFile());
    c.setConfig(props);
    c.configChanged();

    th.start();

    std::this_thread::sleep_for(200ms);

    th.quit();
    th.wait();

    REQUIRE(sendSpy.count() == 10);
}

TEST_CASE("Send test 2", "[canrawplayer]")
{
    using namespace std::chrono_literals;

    CanRawPlayer c;
    QWidget props;
    QSignalSpy sendSpy(&c, &CanRawPlayer::sendFrame);
    QThread th;

    c.moveToThread(&th);

    QObject::connect(&th, &QThread::started, &c, &CanRawPlayer::startSimulation);
    QObject::connect(&th, &QThread::finished, &c, &CanRawPlayer::stopSimulation);

    props.setProperty("file", createTestFile2());
    c.setConfig(props);
    c.configChanged();

    th.start();

    std::this_thread::sleep_for(200ms);

    th.quit();
    th.wait();

    REQUIRE(sendSpy.count() == 5);
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
