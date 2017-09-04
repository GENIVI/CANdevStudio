#define CATCH_CONFIG_RUNNER
#include "canrawsender/canrawsender.h"
#include "canrawsender/crsfactoryinterface.hpp"
#include "canrawsender/crsguiinterface.hpp"
#include "canrawsender/newlinemanager.h"
#include <catch.hpp>
#include <fakeit.hpp>

#include "log.hpp"
std::shared_ptr<spdlog::logger> kDefaultLogger;

#include <QSignalSpy>
int id = qRegisterMetaType<QCanBusFrame>("QCanBusFrame");

TEST_CASE("Create CanRawSender correctly", "[newlinemanager]")
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
    REQUIRE_NOTHROW(new CanRawSender(factoryMock.get()));
}

TEST_CASE("Constructor with nullptr in argument", "[newlinemanager]")
{
    REQUIRE_THROWS(new NewLineManager(nullptr, false));
}

TEST_CASE("Constructor with correct arguments", "[newlinemanager]")
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

    Mock<NLMFactoryInterface> nlmFactoryMock;
    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Mock<LineEditInterface> nlmLineEditMock;
    Mock<PushButtonInterface> nlmPushButtonMock;

    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, releasedCbk));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    Fake(Method(nlmPushButtonMock, pressedCbk));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    REQUIRE_NOTHROW(new NewLineManager(&canRawSender, false, nlmFactoryMock.get()));
}

TEST_CASE("Send button clicked - send one frame test", "[newlinemanager]")
{
    using namespace fakeit;
    Mock<CRSFactoryInterface> factoryMock;
    Mock<CRSGuiInterface> crsMock;
    PushButtonInterface::pressed_t pressedCbk;
    Mock<NLMFactoryInterface> nlmFactoryMock;
    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Mock<LineEditInterface> nlmLineEditMock;
    Mock<PushButtonInterface> nlmPushButtonMock;

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

    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, getMainWidget));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, getTextLength)).AlwaysDo([&]() { return 2; });
    When(Method(nlmLineEditMock, getText)).AlwaysDo([&]() { return "22"; });
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, releasedCbk));
    Fake(Method(nlmCheckBoxMock, getMainWidget));
    When(Method(nlmCheckBoxMock, getState)).Return(false);
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    When(Method(nlmPushButtonMock, pressedCbk)).Do([&](auto&& fn) { pressedCbk = fn; });
    ;
    Fake(Method(nlmPushButtonMock, getMainWidget));
    Fake(Method(nlmPushButtonMock, setDisabled));
    Fake(Method(nlmPushButtonMock, isEnabled));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    NewLineManager newLineMgr{ &canRawSender, true, nlmFactoryMock.get() };
    QSignalSpy canRawSenderSpy(&canRawSender, &CanRawSender::sendFrame);
    pressedCbk();
    CHECK(canRawSenderSpy.count() > 0);
}

/* TODO
 * Problem with QTimer expired test
 * Debuger description: QObject::startTimer: Timers can only be used with threads started with QThread
TEST_CASE("Send button clicked - send several frame test", "[newlinemanager]")
{
    using namespace fakeit;
    Mock<CRSFactoryInterface> factoryMock;
    Mock<CRSGuiInterface> crsMock;
    PushButtonInterface::pressed_t pressedCbk;
    Mock<NLMFactoryInterface> nlmFactoryMock;
    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Mock<LineEditInterface> nlmLineEditMock;
    Mock<PushButtonInterface> nlmPushButtonMock;

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

    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, getMainWidget));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, getTextLength)).AlwaysDo([&]() {return 2;});
    //When(Method(nlmLineEditMock, getText)).AlwaysDo([&]() {return "22";});
    When(Method(nlmLineEditMock, getText)).Return("2", "2", "1");
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() {return &nlmLineEditMock.get();});

    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, releasedCbk));
    Fake(Method(nlmCheckBoxMock, getMainWidget));
    When(Method(nlmCheckBoxMock, getState)).Return(true);
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    When(Method(nlmPushButtonMock, pressedCbk)).Do([&](auto&& fn) { pressedCbk = fn; });;
    Fake(Method(nlmPushButtonMock, getMainWidget));
    Fake(Method(nlmPushButtonMock, setDisabled));
    Fake(Method(nlmPushButtonMock, isEnabled));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    NewLineManager newLineMgr{ &canRawSender, true, nlmFactoryMock.get()};
    QSignalSpy canRawSenderSpy(&canRawSender, &CanRawSender::sendFrame);
    pressedCbk();
    CHECK(canRawSenderSpy.count() == 1);
    //sleep(1);
    //canRawSenderSpy.wait(1000);
    //while (canRawSenderSpy.count() < 2) { QThread::msleep(100);}
    CHECK(canRawSenderSpy.count() > 1);
}
*/
TEST_CASE("Get columns wigdet test", "[newlinemanager]")
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

    Mock<NLMFactoryInterface> nlmFactoryMock;
    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Mock<LineEditInterface> nlmLineEditMock;
    Mock<PushButtonInterface> nlmPushButtonMock;

    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, getMainWidget)).AlwaysDo([&]() {
        return reinterpret_cast<QWidget*>(&nlmLineEditMock.get());
    });
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, releasedCbk));
    When(Method(nlmCheckBoxMock, getMainWidget)).Return(reinterpret_cast<QWidget*>(&nlmCheckBoxMock.get()));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    Fake(Method(nlmPushButtonMock, pressedCbk));
    When(Method(nlmPushButtonMock, getMainWidget)).Return(reinterpret_cast<QWidget*>(&nlmPushButtonMock.get()));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    NewLineManager newLineMgr{ &canRawSender, true, nlmFactoryMock.get() };

    for (NewLineManager::ColName ii : NewLineManager::ColNameIterator()) {
        CHECK(newLineMgr.GetColsWidget(ii) != nullptr);
    }
    CHECK(newLineMgr.GetColsWidget(NewLineManager::ColNameIterator().end()) == nullptr);
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
