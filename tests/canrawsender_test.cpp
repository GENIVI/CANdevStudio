#include <QJsonArray>
#include <QStandardItemModel>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <canrawsender.h>
#include <context.h>
#include <fakeit.hpp>
#include <gui/crsguiinterface.h>
#include <memory>
#include <newlinemanager.h>

using namespace fakeit;

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
        QModelIndexList getList()
        {
            return indexList;
        }

    private:
        QStandardItemModel tvModel;
        QStandardItem item;
        QList<QStandardItem*> list;
        QModelIndexList indexList;
    };

    CRSGuiInterface::add_t addLineCbk;
    CRSGuiInterface::remove_t removeLineCbk;

    Mock<CRSGuiInterface> crsMock;

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Mock<LineEditInterface> nlmLineEditMock;
    Mock<PushButtonInterface> nlmPushButtonMock;

    Mock<NLMFactoryInterface> nlmFactoryMock;
    Fake(Dtor(nlmFactoryMock));
    helpTestClass mHelp;

    Fake(Dtor(nlmLineEditMock));
    Fake(Method(nlmLineEditMock, textChangedCbk));
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    When(Method(nlmLineEditMock, mainWidget)).AlwaysDo([&]() {
        return reinterpret_cast<QWidget*>(&nlmLineEditMock.get());
    });
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Fake(Dtor(nlmCheckBoxMock));
    Fake(Method(nlmCheckBoxMock, toggledCbk));
    Fake(Method(nlmCheckBoxMock, setDisabled));
    When(Method(nlmCheckBoxMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmCheckBoxMock.get()));
    When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

    Fake(Dtor(nlmPushButtonMock));
    Fake(Method(nlmPushButtonMock, init));
    Fake(Method(nlmPushButtonMock, pressedCbk));
    Fake(Method(nlmPushButtonMock, setCheckable));
    Fake(Method(nlmPushButtonMock, checkable));
    Fake(Method(nlmPushButtonMock, checked));
    Fake(Method(nlmPushButtonMock, setChecked));
    When(Method(nlmPushButtonMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmPushButtonMock.get()));
    When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

    Fake(Dtor(crsMock));
    When(Method(crsMock, setAddCbk)).Do([&](auto&& fn) { addLineCbk = fn; });
    When(Method(crsMock, setRemoveCbk)).Do([&](auto&& fn) { removeLineCbk = fn; });
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    When(Method(crsMock, getSelectedRows)).Do([&]() { return mHelp.getList(); });
    Fake(Method(crsMock, setIndexWidget));

    CanRawSender canRawSender{ CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()) };

    CHECK(canRawSender.getLineCount() == 0);
    addLineCbk();
    CHECK(canRawSender.getLineCount() == 1);
    removeLineCbk();
    CHECK(canRawSender.getLineCount() == 0);
}

TEST_CASE("Can raw sender save configuration test", "[canrawsender]")
{
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

    CanRawSender canRawSender{ CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()) };

    QJsonObject json = canRawSender.getConfig();

    CHECK(json.empty() == false);
    CHECK(json.count() == 3);
    const auto colIter = json.find("senderColumns");
    CHECK(colIter != json.end());
    CHECK(colIter.value().type() == QJsonValue::Array);
    const auto colArray = json["senderColumns"].toArray();
    CHECK(colArray.empty() == false);
    CHECK(colArray.size() == 5);
    CHECK(colArray.contains("Id") == true);
    CHECK(colArray.contains("Data") == true);
    CHECK(colArray.contains("Loop") == true);
    CHECK(colArray.contains("Interval") == true);

    CHECK(json.contains("content") == true);

    const auto sortIter = json.find("sorting");
    CHECK(sortIter != json.end());
    CHECK(sortIter.value().type() == QJsonValue::Object);
    const auto sortObj = json["sorting"].toObject();
    CHECK(sortObj.contains("currentIndex") == true);
}

class commonTestClass {
public:
    commonTestClass() = delete;
    commonTestClass(const QString& id, const QString& data, const QString& interval, bool loop)
    {
        Fake(Dtor(crsMock));
        Fake(Method(crsMock, setAddCbk));
        Fake(Method(crsMock, setRemoveCbk));
        Fake(Method(crsMock, setDockUndockCbk));
        Fake(Method(crsMock, mainWidget));
        Fake(Method(crsMock, initTableView));
        Fake(Method(crsMock, getSelectedRows));
        Fake(Method(crsMock, setIndexWidget));
        Fake(Dtor(nlmFactoryMock));

        Fake(Dtor(nlmLineEditMock));
        Fake(Method(nlmLineEditMock, textChangedCbk));
        Fake(Method(nlmLineEditMock, init));
        Fake(Method(nlmLineEditMock, setPlaceholderText));
        Fake(Method(nlmLineEditMock, setDisabled));
        Fake(Method(nlmLineEditMock, setText));
        When(Method(nlmLineEditMock, getText)).Return(id, data, interval);
        When(Method(nlmLineEditMock, mainWidget)).AlwaysDo([&]() {
            return reinterpret_cast<QWidget*>(&nlmLineEditMock.get());
        });
        When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

        Fake(Dtor(nlmCheckBoxMock));
        Fake(Method(nlmCheckBoxMock, toggledCbk));
        When(Method(nlmCheckBoxMock, getState)).Return(true, loop);
        Fake(Method(nlmCheckBoxMock, setState));
        When(Method(nlmCheckBoxMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmCheckBoxMock.get()));
        When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

        Fake(Dtor(nlmPushButtonMock));
        Fake(Method(nlmPushButtonMock, init));
        Fake(Method(nlmPushButtonMock, pressedCbk));
        When(Method(nlmPushButtonMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmPushButtonMock.get()));
        When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

        Fake(Dtor(crsMock));
        Fake(Method(crsMock, setAddCbk));
        Fake(Method(crsMock, setRemoveCbk));
        Fake(Method(crsMock, setDockUndockCbk));
        Fake(Method(crsMock, mainWidget));
        Fake(Method(crsMock, initTableView));
        Fake(Method(crsMock, getSelectedRows));
        Fake(Method(crsMock, setIndexWidget));
    }

public:
    Mock<CRSGuiInterface> crsMock;
    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Mock<LineEditInterface> nlmLineEditMock;
    Mock<PushButtonInterface> nlmPushButtonMock;
    Mock<NLMFactoryInterface> nlmFactoryMock;
};

TEST_CASE("Can raw sender restore configuration test - pass", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    auto idConfStr = jsonObject["content"].toArray()[0].toObject()["id"].toString();
    auto dataConfStr = jsonObject["content"].toArray()[0].toObject()["data"].toString();
    auto intervalConfStr = jsonObject["content"].toArray()[0].toObject()["interval"].toString();
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    CHECK(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    CHECK(canRawSender.getLineCount() == 1);
}

TEST_CASE("Can raw sender restore configuration test - column name incorrect", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig_badcolumns.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    auto idConfStr = jsonObject["content"].toArray()[0].toObject()["id"].toString();
    auto dataConfStr = jsonObject["content"].toArray()[0].toObject()["data"].toString();
    auto intervalConfStr = jsonObject["content"].toArray()[0].toObject()["interval"].toString();
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    CHECK(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    CHECK(canRawSender.getLineCount() == 0);
}

TEST_CASE("Can raw sender restore configuration test - Id incorrect", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig_badId.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    QString idConfStr("");
    auto dataConfStr = jsonObject["content"].toArray()[0].toObject()["data"].toString();
    auto intervalConfStr = jsonObject["content"].toArray()[0].toObject()["interval"].toString();
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    CHECK(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    CHECK(canRawSender.getLineCount() == 0);
}

TEST_CASE("Can raw sender restore configuration test - Data incorrect", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig_badData.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    auto idConfStr = jsonObject["content"].toArray()[0].toObject()["id"].toString();
    QString dataConfStr("");
    auto intervalConfStr = jsonObject["content"].toArray()[0].toObject()["interval"].toString();
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    CHECK(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    CHECK(canRawSender.getLineCount() == 0);
}

TEST_CASE("Can raw sender restore configuration test - Interval incorrect", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig_badInterval.cds"));
    CHECK(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    auto idConfStr = jsonObject["content"].toArray()[0].toObject()["id"].toString();
    auto dataConfStr = jsonObject["content"].toArray()[0].toObject()["data"].toString();
    QString intervalConfStr("");
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    CHECK(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    CHECK(canRawSender.getLineCount() == 0);
}
