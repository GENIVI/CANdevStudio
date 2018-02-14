#define CATCH_CONFIG_RUNNER
#include <projectconfig.h>
#include <pcinterface.h>
#include <projectconfigvalidator.h>
#include <fakeit.hpp>
#include <log.h>
#include <QWindow>
#include <QApplication>
#include <QDir>
#include <QSignalSpy>
#include <QCloseEvent>
#include <candevicemodel.h>
#include <nodes/FlowScene>

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
    QtNodes::FlowScene * fs;

    Mock<PCInterface> pcMock;

    Fake(Dtor(pcMock));
    When(Method(pcMock, setNodeCreatedCallback)).Do([&](auto flow, auto&& fn) { fs=flow; nodeCreated = fn; });
    When(Method(pcMock, setNodeDeletedCallback)).Do([&](auto, auto&& fn) { nodeDeleted = fn; });
    When(Method(pcMock, setNodeDoubleClickedCallback)).Do([&](auto, auto&& fn) { nodeClicked = fn; });
    When(Method(pcMock, setNodeContextMenuCallback)).Do([&](auto, auto&& fn) { nodeMenu = fn; });

    ProjectConfig pc(nullptr, ProjectConfigCtx(&pcMock.get()));

    auto &node = fs->createNode(std::make_unique<CanDeviceModel>());
    //nodeMenu(node, QPointF());
    fs->removeNode(node);
}

int main(int argc, char* argv[]){
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

