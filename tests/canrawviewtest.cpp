#include <candevice/candevice.h>
#define CATCH_CONFIG_RUNNER
#include <fakeit.hpp>

#include "canrawview/canrawview.h"
#include "canrawview/crvfactoryinterface.hpp"
#include "canrawview/crvguiinterface.hpp"

#include "log.hpp"
std::shared_ptr<spdlog::logger> kDefaultLogger;

#include <QSignalSpy>
#include <QtCore/QAbstractItemModel>
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc

TEST_CASE("Initialization failed", "[canrawview]")
{
    using namespace fakeit;
    Mock<CRVFactoryInterface> factoryMock;
    Mock<CRVGuiInterface> crvMock;

    Fake(Dtor(crvMock));
    Fake(Method(crvMock, setClearCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, getMainWidget));
    Fake(Method(crvMock, initTableView));
    Fake(Method(crvMock, updateScroll));
    Fake(Method(crvMock, getSortOrder));
    Fake(Method(crvMock, getClickedColumn));
    Fake(Method(crvMock, setSorting));

    When(Method(factoryMock, createGui)).Return(&crvMock.get());

    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    CanRawView canRawView{ factoryMock.get() };
    canRawView.frameReceived(testFrame);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring unit tests");
    return Catch::Session().run(argc, argv);
}
