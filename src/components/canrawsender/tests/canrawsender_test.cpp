#include <QJsonArray>
#include <QStandardItemModel>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <canrawsender.h>
#include <context.h>
#define CATCH_CONFIG_RUNNER
#include <QApplication>
#include <QSignalSpy>
#include <catch.hpp>
#include <fakeit.hpp>
#include <gui/crsguiinterface.h>
#include <log.h>
#include <memory>
#include <newlinemanager.h>

using namespace fakeit;

std::shared_ptr<spdlog::logger> kDefaultLogger;

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
    PushButtonInterface::pressed_t sendPressed;

    Mock<CRSGuiInterface> crsMock;

    Mock<CheckBoxInterface> nlmCheckBoxMock;
    Mock<LineEditInterface> nlmLineEditMock;
    Mock<PushButtonInterface> nlmPushButtonMock;

    Mock<NLMFactoryInterface> nlmFactoryMock;
    helpTestClass mHelp;

    Fake(Method(nlmLineEditMock, textChangedCbk));
    When(Method(nlmLineEditMock, mainWidget)).AlwaysDo([&]() {
        return reinterpret_cast<QWidget*>(&nlmLineEditMock.get());
    });
    Fake(Method(nlmLineEditMock, init));
    Fake(Method(nlmLineEditMock, setPlaceholderText));
    Fake(Method(nlmLineEditMock, setDisabled));
    Fake(Method(nlmLineEditMock, getTextLength));
    Fake(Method(nlmLineEditMock, getText));
    Fake(Method(nlmLineEditMock, setText));
    Fake(Method(nlmLineEditMock, editingFinishedCbk));
    When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

    Fake(Method(nlmCheckBoxMock, toggledCbk));
    When(Method(nlmCheckBoxMock, mainWidget)).AlwaysReturn(reinterpret_cast<QWidget*>(&nlmCheckBoxMock.get()));
    Fake(Method(nlmCheckBoxMock, getState));
    Fake(Method(nlmCheckBoxMock, setState));
    Fake(Method(nlmCheckBoxMock, setDisabled));
    When(Method(nlmFactoryMock, createCheckBox)).AlwaysReturn(&nlmCheckBoxMock.get());

    Fake(Method(nlmPushButtonMock, init));
    When(Method(nlmPushButtonMock, pressedCbk)).AlwaysDo([&](auto&& fn) { sendPressed = fn; });
    Fake(Method(nlmPushButtonMock, setDisabled));
    Fake(Method(nlmPushButtonMock, isEnabled));
    Fake(Method(nlmPushButtonMock, setCheckable));
    Fake(Method(nlmPushButtonMock, checkable));
    When(Method(nlmPushButtonMock, checked)).Return(true).Return(false).Return(true);
    Fake(Method(nlmPushButtonMock, setChecked));
    When(Method(nlmPushButtonMock, mainWidget)).AlwaysReturn(reinterpret_cast<QWidget*>(&nlmPushButtonMock.get()));
    When(Method(nlmFactoryMock, createPushButton)).AlwaysReturn(&nlmPushButtonMock.get());

    When(Method(crsMock, setAddCbk)).Do([&](auto&& fn) { addLineCbk = fn; });
    When(Method(crsMock, setRemoveCbk)).Do([&](auto&& fn) { removeLineCbk = fn; });
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    When(Method(crsMock, getSelectedRows)).Do([&]() { return mHelp.getList(); });
    Fake(Method(crsMock, setIndexWidget));

    CanRawSender canRawSender{ CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()) };

    REQUIRE(canRawSender.getLineCount() == 0);

    addLineCbk();
    addLineCbk();
    REQUIRE(canRawSender.getLineCount() == 2);

    canRawSender.stopSimulation();
    canRawSender.startSimulation();

    removeLineCbk();
    REQUIRE(canRawSender.getLineCount() == 1);

    sendPressed();
}

TEST_CASE("Can raw sender save configuration test", "[canrawsender]")
{
    Mock<CRSGuiInterface> crsMock;
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    Fake(Method(crsMock, setDockUndockCbk));
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<NLMFactoryInterface> nlmFactoryMock;

    CanRawSender canRawSender{ CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()) };

    QJsonObject json = canRawSender.getConfig();

    REQUIRE(json.empty() == false);
    REQUIRE(json.count() == 3);
    const auto colIter = json.find("senderColumns");
    REQUIRE(colIter != json.end());
    REQUIRE(colIter.value().type() == QJsonValue::Array);
    const auto colArray = json["senderColumns"].toArray();
    REQUIRE(colArray.empty() == false);
    REQUIRE(colArray.size() == 5);
    REQUIRE(colArray.contains("Id") == true);
    REQUIRE(colArray.contains("Data") == true);
    REQUIRE(colArray.contains("Loop") == true);
    REQUIRE(colArray.contains("Interval") == true);

    REQUIRE(json.contains("content") == true);

    const auto sortIter = json.find("sorting");
    REQUIRE(sortIter != json.end());
    REQUIRE(sortIter.value().type() == QJsonValue::Object);
    const auto sortObj = json["sorting"].toObject();
    REQUIRE(sortObj.contains("currentIndex") == true);
}

class commonTestClass {
public:
    commonTestClass() = delete;
    commonTestClass(const QString& id, const QString& data, const QString& interval, bool loop)
    {
        Fake(Method(crsMock, setAddCbk));
        Fake(Method(crsMock, setRemoveCbk));
        Fake(Method(crsMock, setDockUndockCbk));
        Fake(Method(crsMock, mainWidget));
        Fake(Method(crsMock, initTableView));
        Fake(Method(crsMock, getSelectedRows));
        Fake(Method(crsMock, setIndexWidget));

        Fake(Method(nlmLineEditMock, textChangedCbk));
        Fake(Method(nlmLineEditMock, editingFinishedCbk));
        Fake(Method(nlmLineEditMock, init));
        Fake(Method(nlmLineEditMock, setPlaceholderText));
        Fake(Method(nlmLineEditMock, setDisabled));
        Fake(Method(nlmLineEditMock, setText));
        When(Method(nlmLineEditMock, getText)).Return(id, data, interval);
        When(Method(nlmLineEditMock, mainWidget)).AlwaysDo([&]() {
            return reinterpret_cast<QWidget*>(&nlmLineEditMock.get());
        });
        When(Method(nlmFactoryMock, createLineEdit)).AlwaysDo([&]() { return &nlmLineEditMock.get(); });

        Fake(Method(nlmCheckBoxMock, toggledCbk));
        When(Method(nlmCheckBoxMock, getState)).Return(true, loop);
        Fake(Method(nlmCheckBoxMock, setState));
        When(Method(nlmCheckBoxMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmCheckBoxMock.get()));
        When(Method(nlmFactoryMock, createCheckBox)).Return(&nlmCheckBoxMock.get());

        Fake(Method(nlmPushButtonMock, init));
        Fake(Method(nlmPushButtonMock, pressedCbk));
        When(Method(nlmPushButtonMock, mainWidget)).Return(reinterpret_cast<QWidget*>(&nlmPushButtonMock.get()));
        When(Method(nlmFactoryMock, createPushButton)).Return(&nlmPushButtonMock.get());

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
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    auto idConfStr = jsonObject["content"].toArray()[0].toObject()["id"].toString();
    auto dataConfStr = jsonObject["content"].toArray()[0].toObject()["data"].toString();
    auto intervalConfStr = jsonObject["content"].toArray()[0].toObject()["interval"].toString();
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    REQUIRE(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    REQUIRE(canRawSender.getLineCount() == 1);
}

TEST_CASE("Can raw sender restore configuration test - column name incorrect", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig_badcolumns.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    auto idConfStr = jsonObject["content"].toArray()[0].toObject()["id"].toString();
    auto dataConfStr = jsonObject["content"].toArray()[0].toObject()["data"].toString();
    auto intervalConfStr = jsonObject["content"].toArray()[0].toObject()["interval"].toString();
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    REQUIRE(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    REQUIRE(canRawSender.getLineCount() == 0);
}

TEST_CASE("Can raw sender restore configuration test - Id incorrect", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig_badId.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    QString idConfStr("");
    auto dataConfStr = jsonObject["content"].toArray()[0].toObject()["data"].toString();
    auto intervalConfStr = jsonObject["content"].toArray()[0].toObject()["interval"].toString();
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    REQUIRE(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    REQUIRE(canRawSender.getLineCount() == 0);
}

TEST_CASE("Can raw sender restore configuration test - Data incorrect", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig_badData.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    auto idConfStr = jsonObject["content"].toArray()[0].toObject()["id"].toString();
    QString dataConfStr("");
    auto intervalConfStr = jsonObject["content"].toArray()[0].toObject()["interval"].toString();
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    REQUIRE(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    REQUIRE(canRawSender.getLineCount() == 0);
}

TEST_CASE("Can raw sender restore configuration test - Interval incorrect", "[canrawsender]")
{
    QDir dir("configfiles");
    QFile file(dir.absoluteFilePath("canrawsenderconfig_badInterval.cds"));
    REQUIRE(file.open(QIODevice::ReadOnly) == true);

    QByteArray wholeFile = file.readAll();
    QJsonDocument jsonFile(QJsonDocument::fromJson(wholeFile));
    QJsonObject jsonObject = jsonFile.object();
    auto idConfStr = jsonObject["content"].toArray()[0].toObject()["id"].toString();
    auto dataConfStr = jsonObject["content"].toArray()[0].toObject()["data"].toString();
    QString intervalConfStr("");
    auto loopConfBool = jsonObject["content"].toArray()[0].toObject()["loop"].toBool();

    commonTestClass commontest(idConfStr, dataConfStr, intervalConfStr, loopConfBool);
    CanRawSender canRawSender{ CanRawSenderCtx(&commontest.crsMock.get(), &commontest.nlmFactoryMock.get()) };

    REQUIRE(canRawSender.getLineCount() == 0);
    // Add new line
    canRawSender.setConfig(jsonObject);
    // If configuration is corrected create new line - check it
    REQUIRE(canRawSender.getLineCount() == 0);
}

TEST_CASE("Misc", "[canrawsender]")
{
    CanRawSender canRawSender;

    REQUIRE(canRawSender.mainWidgetDocked() == true);
    REQUIRE(canRawSender.mainWidget() != nullptr);
}

TEST_CASE("setConfig using QObject", "[canrawsender]")
{
    CanRawSender crs;
    QWidget config;

    config.setProperty("name", "CAN1");
    config.setProperty("fake", "unsupported");

    crs.setConfig(config);

    auto qConfig = crs.getQConfig();

    REQUIRE(qConfig->property("name").toString() == "CAN1");
    REQUIRE(qConfig->property("fake").isValid() == false);
}

TEST_CASE("Restore config paths - columnAdopt", "[canrawsender]")
{
    CanRawSender canRawSender;
    QJsonObject json;
    QJsonArray columnArray;
    QJsonObject columnItem;

    // No senderColumns
    canRawSender.setConfig(json);

    // senderColumns is not an array
    json["senderColumns"] = "";
    canRawSender.setConfig(json);

    // Array size != 5
    columnItem["dummy"] = 123;
    columnArray.append(columnItem);
    json["senderColumns"] = columnArray;
    canRawSender.setConfig(json);

    // No Id column
    columnArray = QJsonArray();
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    json["senderColumns"] = columnArray;
    canRawSender.setConfig(json);

    // No Data column
    columnArray = QJsonArray();
    columnArray.append({ "Id" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    json["senderColumns"] = columnArray;
    canRawSender.setConfig(json);

    // No Loop column
    columnArray = QJsonArray();
    columnArray.append({ "Id" });
    columnArray.append({ "Data" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    json["senderColumns"] = columnArray;
    canRawSender.setConfig(json);

    // No Interval column
    columnArray = QJsonArray();
    columnArray.append({ "Id" });
    columnArray.append({ "Data" });
    columnArray.append({ "Loop" });
    columnArray.append({ "Idd" });
    columnArray.append({ "Idd" });
    json["senderColumns"] = columnArray;
    canRawSender.setConfig(json);

    // Validation complete
    columnArray = QJsonArray();
    columnArray.append({ "Id" });
    columnArray.append({ "Data" });
    columnArray.append({ "Loop" });
    columnArray.append({ "Interval" });
    columnArray.append({ "Idd" });
    json["senderColumns"] = columnArray;
    canRawSender.setConfig(json);
}

TEST_CASE("Restore config paths - contentAdopt", "[canrawsender]")
{
    CanRawSender canRawSender;
    QJsonObject json;
    QJsonArray columnArray;
    QJsonObject columnItem;
    QJsonArray contentArray;

    // contentAdopt Validation complete
    columnArray = QJsonArray();
    columnArray.append({ "Id" });
    columnArray.append({ "Data" });
    columnArray.append({ "Loop" });
    columnArray.append({ "Interval" });
    columnArray.append({ "Idd" });
    json["senderColumns"] = columnArray;

    // content is not array
    json["content"] = "";
    canRawSender.setConfig(json);

    // no Data Id Interval Loop and Send fields
    contentArray = QJsonArray();
    contentArray.append({ "dummy" });
    json["content"] = contentArray;
    canRawSender.setConfig(json);

    // Data has wrong type
    contentArray = QJsonArray();
    contentArray.append(QJsonObject({ { "data", true } }));
    json["content"] = contentArray;
    canRawSender.setConfig(json);

    // Id has wrong type
    contentArray = QJsonArray();
    contentArray.append(QJsonObject({ { "id", true } }));
    json["content"] = contentArray;
    canRawSender.setConfig(json);

    // interval has wrong type
    contentArray = QJsonArray();
    contentArray.append(QJsonObject({ { "interval", true } }));
    json["content"] = contentArray;
    canRawSender.setConfig(json);

    // loop has wrong type
    contentArray = QJsonArray();
    contentArray.append(QJsonObject({ { "loop", "aa" } }));
    json["content"] = contentArray;
    canRawSender.setConfig(json);

    // loop has wrong type
    contentArray = QJsonArray();
    contentArray.append(QJsonObject({ { "send", "aa" } }));
    json["content"] = contentArray;
    canRawSender.setConfig(json);
}

TEST_CASE("Restore config paths - sortingAdopt", "[canrawsender]")
{
    CanRawSender canRawSender;
    QJsonObject json;
    QJsonArray columnArray;
    QJsonObject columnItem;
    QJsonArray contentArray;

    // contentAdopt Validation complete
    columnArray = QJsonArray();
    columnArray.append({ "Id" });
    columnArray.append({ "Data" });
    columnArray.append({ "Loop" });
    columnArray.append({ "Interval" });
    columnArray.append({ "Idd" });
    json["senderColumns"] = columnArray;
    contentArray = QJsonArray();
    contentArray.append({ "dummy" });
    json["content"] = contentArray;
    canRawSender.setConfig(json);

    // sorting is not an Object
    json["sorting"] = "";
    canRawSender.setConfig(json);

    // sorting is Object size != 1
    json["sorting"] = QJsonObject({ { "a", "b" }, { "c", "d" } });
    canRawSender.setConfig(json);

    // no currentIndex
    json["sorting"] = QJsonObject({ { "a", "b" } });
    canRawSender.setConfig(json);

    // currentIndex is not a number
    json["sorting"] = QJsonObject({ { "currentIndex", "b" } });
    canRawSender.setConfig(json);

    // All good!
    json["sorting"] = QJsonObject({ { "currentIndex", 12 } });
    canRawSender.setConfig(json);
}

TEST_CASE("Dock/Undock", "[canrawsender]")
{
    CRSGuiInterface::dockUndock_t dockUndock;

    Mock<CRSGuiInterface> crsMock;
    Fake(Method(crsMock, setAddCbk));
    Fake(Method(crsMock, setRemoveCbk));
    When(Method(crsMock, setDockUndockCbk)).Do([&](auto&& fn) { dockUndock = fn; });
    Fake(Method(crsMock, mainWidget));
    Fake(Method(crsMock, initTableView));
    Fake(Method(crsMock, getSelectedRows));
    Fake(Method(crsMock, setIndexWidget));

    Mock<NLMFactoryInterface> nlmFactoryMock;

    CanRawSender canRawSender{ CanRawSenderCtx(&crsMock.get(), &nlmFactoryMock.get()) };
    QSignalSpy dockSpy(&canRawSender, &CanRawSender::mainWidgetDockToggled);

    REQUIRE(canRawSender.mainWidgetDocked() == true);

    dockUndock();

    REQUIRE(dockSpy.count() == 1);
    REQUIRE(canRawSender.mainWidgetDocked() == false);

    dockUndock();

    REQUIRE(dockSpy.count() == 2);
    REQUIRE(canRawSender.mainWidgetDocked() == true);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit tests");
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
