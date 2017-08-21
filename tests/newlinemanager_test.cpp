/*
#include "canrawsender/newlinemanager.h"
#include "canrawsender/canrawsender.h"
#define CATCH_CONFIG_RUNNER
#include <QtWidgets/QApplication>
#include <catch.hpp>
#include <fakeit.hpp>
#include <memory>

TEST_CASE("Create NewLineManager class with wrong argument", "[newlinemanager]")
{
    REQUIRE_THROWS(new NewLineManager(nullptr, false));
}

TEST_CASE("Create NewLineManager class with correct argument", "[newlinemanager]")
{
    CanRawSender canRawSender;
    REQUIRE_NOTHROW(new NewLineManager(&canRawSender, false));
}

TEST_CASE("Check initial wigdets settings", "[newlinemanager]")
{
    CanRawSender canRawSender;
    NewLineManager newLine(&canRawSender, false);

    auto intervalWidget = newLine.GetColsWidget(NewLineManager::ColName::IntervalLine);
    CHECK(intervalWidget->isEnabled() == false);

    auto sendButtonWidget = newLine.GetColsWidget(NewLineManager::ColName::SendButton);
    CHECK(sendButtonWidget->isEnabled() == false);
}

TEST_CASE("Check get column widget returns", "[newlinemanager]")
{
    CanRawSender canRawSender;
    NewLineManager newLine(&canRawSender, false);

    auto idWidget = newLine.GetColsWidget(NewLineManager::ColName::IdLine);
    CHECK(idWidget != nullptr);

    auto batWigdet = newLine.GetColsWidget(NewLineManager::ColNameIterator().end());
    CHECK(batWigdet == nullptr);
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    return Catch::Session().run(argc, argv);
}
*/
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
/*
TEST_CASE("add button test", "[newlinemanager]")
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

    //REQUIRE_THROWS(new NewLineManager(&canRawSender, true));

    QCanBusFrame testFrame;
    testFrame.setFrameId(123);
    CanRawView canRawView{ factoryMock.get() };
    canRawView.frameReceived(testFrame);

}
*/
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
