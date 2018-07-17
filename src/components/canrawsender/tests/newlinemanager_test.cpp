#include <QSignalSpy>
#include <QtWidgets/QApplication>
#include <canrawsender.h>
#include <context.h>
#include <fakeit.hpp>
#include <gui/crsguiinterface.h>
#include <log.h>
#include <newlinemanager.h>

TEST_CASE("Create CanRawSender correctly", "[newlinemanager]")
{
    using namespace fakeit;

    Mock<CRSGuiInterface> crsMock;
    Fake(Dtor(crsMock));
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<NLMFactoryInterface> nmlMock;
    Fake(Dtor(nmlMock));

    REQUIRE_NOTHROW(new CanRawSender(CanRawSenderCtx(&crsMock.get(), &nmlMock.get())));
}

TEST_CASE("Constructor with nullptr in argument", "[newlinemanager]")
{
    using namespace fakeit;
    Mock<NLMFactoryInterface> nmlMock;
    Fake(Dtor(nmlMock));

    REQUIRE_THROWS(new NewLineManager(nullptr, false, nmlMock.get()));
}

TEST_CASE("Constructor with correct arguments", "[newlinemanager]")
{
    using namespace fakeit;
    Mock<CRSGuiInterface> crsMock;

    Fake(Dtor(crsMock));
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<NLMFactoryInterface> nlmFactoryMock;
    Fake(Dtor(nlmFactoryMock));

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    Fake(Method(nlmPushButtonMock, pressedCbk));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    CanRawSender canRawSender(CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()));

    REQUIRE_NOTHROW(new NewLineManager(&canRawSender, false, nlmFactoryMock.get()));
}

TEST_CASE("Send button clicked - send one frame test", "[newlinemanager]")
{
    using namespace fakeit;
    PushButtonInterface::pressed_t pressedCbk;

    Mock<NLMFactoryInterface> nlmFactoryMock;
    Fake(Dtor(nlmFactoryMock));

    Mock<CRSGuiInterface> crsMock;
    Fake(Dtor(crsMock));
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, mainWidget));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, getTextLength)).AlwaysDo([&]() { return 2; });
    When(Method(nlmLineEditMock, getText)).AlwaysDo([&]() { return "22"; });
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    Fake(Method(nlmCheckBoxMock, mainWidget));
    When(Method(nlmCheckBoxMock, getState)).Return(false);
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    When(Method(nlmPushButtonMock, pressedCbk)).Do([&](auto&& fn) { pressedCbk = fn; });
    Fake(Method(nlmPushButtonMock, mainWidget));
    Fake(Method(nlmPushButtonMock, setDisabled));
    Fake(Method(nlmPushButtonMock, isEnabled));
    Fake(Method(nlmPushButtonMock, setCheckable));
    Fake(Method(nlmPushButtonMock, checkable));
    Fake(Method(nlmPushButtonMock, checked));
    Fake(Method(nlmPushButtonMock, setChecked));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    CanRawSender canRawSender(CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()));

    NewLineManager newLineMgr{ &canRawSender, true, nlmFactoryMock.get() };
    QSignalSpy canRawSenderSpy(&canRawSender, &CanRawSender::sendFrame);
    pressedCbk();
    CHECK(canRawSenderSpy.count() > 0);
}

TEST_CASE("Send button clicked - send several frame test", "[newlinemanager]")
{
    using namespace fakeit;
    PushButtonInterface::pressed_t pressedCbk;

    Mock<NLMFactoryInterface> nlmFactoryMock;
    Fake(Dtor(nlmFactoryMock));

    Mock<CRSGuiInterface> crsMock;
    Fake(Dtor(crsMock));
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, mainWidget));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, getTextLength)).AlwaysDo([&]() { return 2; });
    // When(Method(nlmLineEditMock, getText)).AlwaysDo([&]() {return "22";});
    When(Method(nlmLineEditMock, getText)).Return("2", "2", "1");
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    Fake(Method(nlmCheckBoxMock, mainWidget));
    When(Method(nlmCheckBoxMock, getState)).Return(true);
    Fake(Method(nlmCheckBoxMock, setDisabled));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    When(Method(nlmPushButtonMock, pressedCbk)).Do([&](auto&& fn) { pressedCbk = fn; });
    Fake(Method(nlmPushButtonMock, mainWidget));
    Fake(Method(nlmPushButtonMock, setDisabled));
    Fake(Method(nlmPushButtonMock, isEnabled));
    Fake(Method(nlmPushButtonMock, setCheckable));
    When(Method(nlmPushButtonMock, checkable)).Return(true);
    Fake(Method(nlmPushButtonMock, checked));
    Fake(Method(nlmPushButtonMock, setChecked));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    CanRawSender canRawSender(CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()));
    NewLineManager newLineMgr{ &canRawSender, true, nlmFactoryMock.get() };
    QSignalSpy canRawSenderSpy(&canRawSender, &CanRawSender::sendFrame);
    pressedCbk();
    CHECK(canRawSenderSpy.count() == 1);
    canRawSenderSpy.wait(100);
    CHECK(canRawSenderSpy.count() > 1);
}

TEST_CASE("Get columns wigdet test", "[newlinemanager]")
{
    using namespace fakeit;

    Mock<CRSGuiInterface> crsMock;
    Fake(Dtor(crsMock));
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<NLMFactoryInterface> nlmFactoryMock;
    Fake(Dtor(nlmFactoryMock));

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, mainWidget)).AlwaysDo([&]() {
        return reinterpret_cast<QWidget*>(&nlmLineEditMock.get());
    });
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    When(Method(nlmCheckBoxMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmCheckBoxMock.get()));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    Fake(Method(nlmPushButtonMock, pressedCbk));
    When(Method(nlmPushButtonMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmPushButtonMock.get()));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    CanRawSender canRawSender(CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()));

    NewLineManager newLineMgr{ &canRawSender, true, nlmFactoryMock.get() };

    for (NewLineManager::ColName ii : NewLineManager::ColNameIterator{NewLineManager::ColName::IdLine}) {
        CHECK(newLineMgr.GetColsWidget(NewLineManager::ColNameIterator{ii}) != nullptr);
    }
    CHECK(newLineMgr.GetColsWidget(NewLineManager::ColNameIterator().end()) == nullptr);
}
