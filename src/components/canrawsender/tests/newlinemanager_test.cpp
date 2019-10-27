#include <QSignalSpy>
#include <QtWidgets/QApplication>
#include <canrawsender.h>
#include <catch.hpp>
#include <context.h>
#include <fakeit.hpp>
#include <gui/crsguiinterface.h>
#include <log.h>
#include <newlinemanager.h>

TEST_CASE("Create CanRawSender correctly", "[newlinemanager]")
{
    using namespace fakeit;

    Mock<CRSGuiInterface> crsMock;
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<NLMFactoryInterface> nmlMock;

    REQUIRE_NOTHROW(new CanRawSender(CanRawSenderCtx(&crsMock.get(), &nmlMock.get())));
}

TEST_CASE("Constructor with nullptr in argument", "[newlinemanager]")
{
    using namespace fakeit;
    Mock<NLMFactoryInterface> nmlMock;

    REQUIRE_THROWS(new NewLineManager(nullptr, false, nmlMock.get()));
}

TEST_CASE("Constructor with correct arguments", "[newlinemanager]")
{
    using namespace fakeit;
    Mock<CRSGuiInterface> crsMock;

    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<NLMFactoryInterface> nlmFactoryMock;

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    Fake(Method(nlmLineEditMock, editingFinishedCbk));
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
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

    Mock<CRSGuiInterface> crsMock;
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, editingFinishedCbk));
    Fake(Method(nlmLineEditMock, mainWidget));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, getText)).Return("12345678", "12345678", "800", "800", "001", "001", "001");
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    Fake(Method(nlmCheckBoxMock, mainWidget));
    When(Method(nlmCheckBoxMock, getState)).Return(false);
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
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
    pressedCbk();
    pressedCbk();
    REQUIRE(canRawSenderSpy.count() > 0);
}

TEST_CASE("Send button clicked - send several frame test", "[newlinemanager]")
{
    using namespace fakeit;
    PushButtonInterface::pressed_t pressedCbk;

    Mock<NLMFactoryInterface> nlmFactoryMock;

    Mock<CRSGuiInterface> crsMock;
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, editingFinishedCbk));
    Fake(Method(nlmLineEditMock, mainWidget));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, getTextLength)).AlwaysDo([&]() { return 2; });
    // When(Method(nlmLineEditMock, getText)).AlwaysDo([&]() {return "22";});
    When(Method(nlmLineEditMock, getText)).Return("2", "2", "2", "2");
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    Fake(Method(nlmCheckBoxMock, mainWidget));
    When(Method(nlmCheckBoxMock, getState)).Return(true);
    Fake(Method(nlmCheckBoxMock, setDisabled));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
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
    REQUIRE(canRawSenderSpy.count() == 1);
    canRawSenderSpy.wait(100);
    REQUIRE(canRawSenderSpy.count() > 1);
}

TEST_CASE("Get columns wigdet test", "[newlinemanager]")
{
    using namespace fakeit;

    Mock<CRSGuiInterface> crsMock;
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<NLMFactoryInterface> nlmFactoryMock;

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, editingFinishedCbk));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, mainWidget)).AlwaysDo([&]() {
        return reinterpret_cast<QWidget*>(&nlmLineEditMock.get());
    });
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    When(Method(nlmCheckBoxMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmCheckBoxMock.get()));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
    Fake(Method(nlmPushButtonMock, init));
    Fake(Method(nlmPushButtonMock, pressedCbk));
    When(Method(nlmPushButtonMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmPushButtonMock.get()));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    CanRawSender canRawSender(CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()));

    NewLineManager newLineMgr{ &canRawSender, true, nlmFactoryMock.get() };

    for (NewLineManager::ColName ii : NewLineManager::ColNameIterator{ NewLineManager::ColName::IdLine }) {
        REQUIRE(newLineMgr.GetColsWidget(NewLineManager::ColNameIterator{ ii }) != nullptr);
    }
    REQUIRE(newLineMgr.GetColsWidget(NewLineManager::ColNameIterator().end()) == nullptr);
}

TEST_CASE("EditionFinished", "[newlinemanager]")
{
    using namespace fakeit;
    PushButtonInterface::pressed_t pressedCbk;
    LineEditInterface::textChanged_t editFinCbk;
    std::vector<QString> validOut;

    Mock<NLMFactoryInterface> nlmFactoryMock;

    Mock<CRSGuiInterface> crsMock;
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<LineEditInterface> nlmLineEditMock;
    Fake(Method(nlmLineEditMock, textChangedCbk));
    When(Method(nlmLineEditMock, editingFinishedCbk)).Do([&](auto&& fn) { editFinCbk = fn; });
    Fake(Method(nlmLineEditMock, mainWidget));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, setText)).AlwaysDo([&](QString t) {
        // Fakeit is not able to verify const refs by Verify function. Collect values to vector
        validOut.push_back(t);
    });

    When(Method(nlmLineEditMock, getText))
        .Return("1", "12", "123", "1234", "12345", "123456", "1234567", "12345678", "123456789", "ffffffff");
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    Fake(Method(nlmCheckBoxMock, mainWidget));
    When(Method(nlmCheckBoxMock, getState)).Return(false);
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Mock<PushButtonInterface> nlmPushButtonMock;
    Fake(Method(nlmPushButtonMock, init));
    Fake(Method(nlmPushButtonMock, pressedCbk));
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

    editFinCbk();
    editFinCbk();
    editFinCbk();
    editFinCbk();
    editFinCbk();
    editFinCbk();
    editFinCbk();
    editFinCbk();
    editFinCbk();
    editFinCbk();

    // Fakeit is not able to Verify parameters passed by reference
    REQUIRE(validOut.size() == 7);
    REQUIRE(validOut[0] == "001");
    REQUIRE(validOut[1] == "012");
    // "123" skipped - do not have to be adjusted
    REQUIRE(validOut[2] == "00001234");
    REQUIRE(validOut[3] == "00012345");
    REQUIRE(validOut[4] == "00123456");
    REQUIRE(validOut[5] == "01234567");
    // "12345678" skipped - do not have to be adjusted
    // "123456789" Failed to convert (bigger than 32 bits)
    REQUIRE(validOut[6] == "1fffffff");
}
