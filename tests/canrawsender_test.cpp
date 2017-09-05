#include "canrawsender/canrawsender.h"
#include "canrawsender/crsfactoryinterface.hpp"
#include "canrawsender/crsguiinterface.hpp"
#include "canrawsender/newlinemanager.h"
#include <QStandardItemModel>
#include <catch.hpp>
#include <fakeit.hpp>
#include <memory>

TEST_CASE("Add and remove frame test", "[canrawsender]")
{

    class helpTestClass {
    public:
        helpTestClass()
        {
            list.append(&item);
            tvModel.appendRow(list);
            auto idx = tvModel.indexFromItem(list.at(0));
            indexList.append(idx);
        }
        QModelIndexList getList() { return indexList; }

    private:
        QStandardItemModel tvModel;
        QStandardItem item;
        QList<QStandardItem*> list;
        QModelIndexList indexList;
    };

    using namespace fakeit;
    CRSGuiInterface::add_t addLineCbk;
    CRSGuiInterface::remove_t removeLineCbk;

    Mock<CRSFactoryInterface> factoryMock;
    Mock<CRSGuiInterface> crsMock;

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Mock<LineEditInterface> nlmLineEditMock;
    Mock<PushButtonInterface> nlmPushButtonMock;

    Mock<NLMFactoryInterface> nlmFactoryMock;
    helpTestClass mHelp;

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

    Fake(Dtor(crsMock));
    When(Method(crsMock, setAddCbk)).Do([&](auto&& fn) { addLineCbk = fn; });
    When(Method(crsMock, setRemoveCbk)).Do([&](auto&& fn) { removeLineCbk = fn; });
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, getMainWidget));
    Fake(Method(crsMock, initTableView));
    When(Method(crsMock, getSelectedRows)).Do([&]() { return mHelp.getList(); });
    Fake(Method(crsMock, setIndexWidget));
    When(Method(crsMock, newLine)).Do([&](auto& _ptr, auto& _flag) {
        return std::make_unique<NewLineManager>(_ptr, _flag, nlmFactoryMock.get());
    });

    When(Method(factoryMock, createGui)).Return(&crsMock.get());
    CanRawSender canRawSender{ factoryMock.get() };

    CHECK(canRawSender.getLineCount() == 0);
    addLineCbk();
    CHECK(canRawSender.getLineCount() == 1);
    removeLineCbk();
    CHECK(canRawSender.getLineCount() == 0);
}
