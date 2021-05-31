#define CATCH_CONFIG_RUNNER
#include <QCanBusFrame>
#include <QJsonArray>
#include <QStandardItemModel>
#include <QTableView>
#include <Qt>
#include <QtWidgets/QApplication>
#include <catch.hpp>
#include <context.h>
#include <fakeit.hpp>
#include <QSignalSpy>

#include "gui/qmlexecutorguiint.h"
#include "qmlexecutor.h"
#include "qmlexecutormodel.h"
#include "guimock.h"

using namespace fakeit;

std::shared_ptr<spdlog::logger> kDefaultLogger;

static const char* const propertyNameName = "name";
static const char* const propertyQMLFileName = "QML file";


TEST_CASE("Stubbed methods", "[qmlexecutor]")
{
    QMLExecutor c;

    REQUIRE(c.mainWidget() != nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj/getconfig", "[qmlexecutor]")
{
    QMLExecutor c;
    QWidget obj;
    QString fileName{ "sample file" };
    QString objectName{ "sample file" };
    QString sampleName{ "bad file" };

    obj.setProperty(propertyNameName, objectName);
    obj.setProperty(propertyQMLFileName, fileName);
    obj.setProperty("bad property name", sampleName);

    c.setConfig(obj);

    auto config = c.getConfig();

    REQUIRE(config.keys().size() == 2);
    REQUIRE(config[propertyNameName].toString() == objectName);
    REQUIRE(config[propertyQMLFileName].toString() == fileName);
}

TEST_CASE("setConfig - json/getconfig", "[qmlexecutor]")
{
    QMLExecutor c;
    QJsonObject json;
    QString fileName{ "sample file" };
    QString objectName{ "sample file" };
    QString sampleName{ "bad file" };

    json[propertyNameName] = objectName;
    json[propertyQMLFileName] = fileName;
    json["bad property name"] = "sampleName";

    c.setConfig(json);

    auto config = c.getConfig();

    REQUIRE(config.keys().size() == 2);
    REQUIRE(config[propertyNameName].toString() == objectName);
    REQUIRE(config[propertyQMLFileName].toString() == fileName);
}

TEST_CASE("configChanged", "[qmlexecutor]")
    {
    auto goodMock = new GuiMock();

    QUrl check;

    Fake(Method(goodMock->mock, setModel));
    When(Method(goodMock->mock, loadQML)).AlwaysDo([&check](const QUrl& url) { check = url; });
    Fake(Method(goodMock->mock, mainWidget));

    QMLExecutorCtx context(goodMock);
    QMLExecutor c(std::move(context));

    QString fileName{ "c:/sample_file.txt" };
    QJsonObject json{ { propertyQMLFileName, fileName } };

    json[propertyQMLFileName] = fileName;

    c.setConfig(json);
    c.configChanged();

    // Using( QUrl::fromLocalFile(fileName)) faile here
    Verify(Method(goodMock->mock, loadQML)).Once();
    REQUIRE(check == QUrl::fromLocalFile(fileName));
    }

TEST_CASE("qml changed from ui", "[qmlexecutor]")
{
    auto goodMock = new GuiMock();

    Fake(Method(goodMock->mock, setModel));
    Fake(Method(goodMock->mock, loadQML));
    Fake(Method(goodMock->mock, mainWidget));


    QMLExecutorCtx context(goodMock);
    QMLExecutor c(std::move(context));

    QString fileName{ "c:/sample_file.txt" };

    emit goodMock->QMLLoaded(QUrl::fromLocalFile(fileName));

    auto config = c.getConfig();

    REQUIRE(config[propertyQMLFileName].toString() == QUrl::fromLocalFile(fileName).toLocalFile());
}

TEST_CASE("simBcastRcv", "[qmlexecutor]")
{
   QMLExecutor c;

   QJsonObject msg;
   msg["msg"] = BcastMsg::GuiStyleSwitched;

   REQUIRE_NOTHROW(c.simBcastRcv(msg, QVariant()));
}

TEST_CASE("getSupportedProperties", "[qmlexecutor]")
{
    QMLExecutor c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 2);

    REQUIRE(ComponentInterface::propertyName(props[0]) == propertyNameName);
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);

    REQUIRE(ComponentInterface::propertyName(props[1]) == propertyQMLFileName);
    REQUIRE(ComponentInterface::propertyType(props[1]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[1]) == true);
    REQUIRE(ComponentInterface::propertyField(props[1]) != nullptr);
    REQUIRE_NOTHROW(ComponentInterface::propertyField(props[1])());
}

TEST_CASE("start/stop", "[qmlexecutor]")
{
    QMLExecutor c;

    CANBusModel model;

    c.setCANBusModel(&model);

    int startCount{ 0 };
    int stopCount{ 0 };

    QObject::connect(&model, &CANBusModel::simulationStarted, [&]() { startCount++; });
    QObject::connect(&model, &CANBusModel::simulationStopped, [&]() { stopCount++; });

    c.startSimulation();
    c.stopSimulation();

    REQUIRE(startCount == 1);
    REQUIRE(stopCount == 1);
}

TEST_CASE("getQConfig", "[qmlexecutor]")
{
   QMLExecutor c;
   QJsonObject json;
   QString fileName{ "sample file" };
   QString objectName{ "sample file" };

   json[propertyNameName] = objectName;
   json[propertyQMLFileName] = fileName;

   c.setConfig(json);

   auto config = c.getQConfig();

   REQUIRE(config->property(propertyNameName).toString() == objectName);
   REQUIRE(config->property(propertyQMLFileName).toString() == fileName);
}

TEST_CASE("dock/undock", "[qmlexecutor]")
{
    auto goodMock = new GuiMock();

    Fake(Method(goodMock->mock, setModel));
    Fake(Method(goodMock->mock, loadQML));
    Fake(Method(goodMock->mock, mainWidget));

    QMLExecutorCtx context(goodMock);
    QMLExecutor c(std::move(context));
    QSignalSpy dockSpy(&c, &QMLExecutor::mainWidgetDockToggled);

    emit goodMock->dockUndock();
    emit goodMock->dockUndock();
    emit goodMock->dockUndock();

    REQUIRE(dockSpy.count() == 3);

    Verify(Method(goodMock->mock, mainWidget)).Exactly(3);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit tests");

    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
