#define CATCH_CONFIG_RUNNER
#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QMenu>
#include <QSignalSpy>
#include <QWindow>
#include <candevicemodel.h>
#include <canrawviewmodel.h>
#include <fakeit.hpp>
#include <log.h>
#include <nodes/FlowScene>
#include <pcinterface.h>
#include <projectconfig.h>
#include <projectconfigvalidator.h>

#include "ui_projectconfig.h"
#include <QPushButton>
#include <iconlabel.h>
#include <plugins.hpp>

std::shared_ptr<spdlog::logger> kDefaultLogger;

TEST_CASE("Loading and saving", "[projectconfig]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("projectconfig.cds"));
    ProjectConfig pc(new QWidget);
    QByteArray outConfig;

    CHECK(file.open(QIODevice::ReadOnly) == true);
    auto inConfig = file.readAll();

    REQUIRE_NOTHROW(pc.load(inConfig));
    REQUIRE_NOTHROW(outConfig = pc.save());
    REQUIRE_NOTHROW(pc.clearGraphView());
    REQUIRE_NOTHROW(pc.load(outConfig));
}

TEST_CASE("Color mode", "[projectconfig]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("projectconfig.cds"));
    ProjectConfig pc(new QWidget);

    CHECK(file.open(QIODevice::ReadOnly) == true);
    auto inConfig = file.readAll();
    REQUIRE_NOTHROW(pc.load(inConfig));

    REQUIRE_NOTHROW(pc.setColorMode(true));
    REQUIRE_NOTHROW(pc.setColorMode(false));
}

TEST_CASE("Close event", "[projectconfig]")
{
    QCloseEvent e;
    ProjectConfig pc(new QWidget);
    QSignalSpy closeSpy(&pc, &ProjectConfig::closeProject);

    pc.closeEvent(&e);

    CHECK(closeSpy.count() == 1);
}

TEST_CASE("Validation schema parse error", "[projectconfig]")
{
    CHECK(ProjectConfigValidator::loadConfigSchema("Makefile") == false);
}

TEST_CASE("Validation JSON format error", "[projectconfig]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("projectconfig_wrong.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);
    auto inConfig = file.read(40);

    CHECK(ProjectConfigValidator::validateConfiguration(inConfig) == false);
}

TEST_CASE("Validation schema validation failed", "[projectconfig]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("projectconfig_wrong.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);
    auto inConfig = file.readAll();

    CHECK(ProjectConfigValidator::validateConfiguration(inConfig) == false);
}

TEST_CASE("Validation succeeded", "[projectconfig]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("projectconfig.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);
    auto inConfig = file.readAll();

    CHECK(ProjectConfigValidator::validateConfiguration(inConfig));
}

TEST_CASE("Validator validate", "[projectconfig]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("projectconfig.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);
    auto inConfig = file.readAll();

    ProjectConfigValidator pcv;
    CHECK(pcv.loadConfigSchema());
    CHECK(pcv.validateConfiguration(inConfig));
}

TEST_CASE("callbacks test", "[projectconfig]")
{
    using namespace fakeit;
    PCInterface::node_t nodeCreated;
    PCInterface::node_t nodeDeleted;
    PCInterface::node_t nodeClicked;
    PCInterface::menu_t nodeMenu;
    QtNodes::FlowScene* fs;

    Mock<PCInterface> pcMock;

    Fake(Dtor(pcMock));
    When(Method(pcMock, setNodeCreatedCallback)).Do([&](auto flow, auto&& fn) {
        fs = flow;
        nodeCreated = fn;
    });
    When(Method(pcMock, setNodeDeletedCallback)).Do([&](auto, auto&& fn) { nodeDeleted = fn; });
    When(Method(pcMock, setNodeDoubleClickedCallback)).Do([&](auto, auto&& fn) { nodeClicked = fn; });
    When(Method(pcMock, setNodeContextMenuCallback)).Do([&](auto, auto&& fn) { nodeMenu = fn; });
    Fake(Method(pcMock, showContextMenu));
    Fake(Method(pcMock, openProperties));

    ProjectConfig pc(nullptr, ProjectConfigCtx(&pcMock.get()));
    pc.simulationStarted();

    auto& node = fs->createNode(std::make_unique<CanDeviceModel>());
    node.restore({});
    nodeCreated(node);
    nodeClicked(node);
    nodeMenu(node, QPointF());

    QSignalSpy showingSpy(&pc, &ProjectConfig::handleWidgetShowing);
    auto& node2 = fs->createNode(std::make_unique<CanRawViewModel>());
    nodeClicked(node2);
    nodeMenu(node2, QPointF());
    CHECK(showingSpy.count() == 1);

    pc.simulationStopped();

    nodeClicked(node);
    nodeMenu(node, QPointF());
    nodeClicked(node2);
    nodeMenu(node2, QPointF());

    fs->removeNode(node);
    fs->removeNode(node2);
}

TEST_CASE("Plugin loading - sections not initialized", "[common]")
{

    QtNodes::FlowScene graphScene;
    Plugins plugins(graphScene.registry());
    REQUIRE_NOTHROW(plugins.addWidgets({ 0x11223344 }));
}

int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(CANdevResources);
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring unit tests");
    QApplication a(argc, argv);
    return Catch::Session().run(argc, argv);
}
