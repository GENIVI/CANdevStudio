#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>
#include <catch.hpp>
#include "canrawsender/canrawsender.h"
#include "canrawsender/newlinemanager.h"
#include "canrawsender/crsfactoryinterface.hpp"
#include "canrawsender/crsguiinterface.hpp"

#include "log.hpp"
std::shared_ptr<spdlog::logger> kDefaultLogger;

#include <QSignalSpy>
#include <QtCore/QAbstractItemModel>
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc

TEST_CASE("null test", "[newlinemanager]")
{
    using namespace fakeit;
    Mock<CRSFactoryInterface> factoryMock;
    Mock<CRSGuiInterface> crsMock;

    Fake(Dtor(crsMock));
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, getMainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    When(Method(factoryMock, createGui)).Return(&crsMock.get());

    CanRawSender canRawSender{ factoryMock.get() };
    canRawSender.startSimulation();
}


int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring canrawsender unit tests");
    return Catch::Session().run(argc, argv);
}
