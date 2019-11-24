#include <QtWidgets/QApplication>
#include <cansignaldata.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QAbstractItemModel>
#include <QCanBusFrame>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QWidget>
#include <catch.hpp>
#include <fakeit.hpp>
#include <gui/cansignaldataguiint.h>
#include <searchmodel.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignaldata]")
{
    CanSignalData c;
    REQUIRE(c.mainWidget() != nullptr);
    REQUIRE(c.mainWidgetDocked() == true);

    using namespace fakeit;
    Mock<CanSignalDataGuiInt> guiInt;
    Fake(Method(guiInt, initSearch));
    Fake(Method(guiInt, setMsgView));
    Fake(Method(guiInt, setMsgViewCbk));
    Fake(Method(guiInt, setDockUndockCbk));
    Fake(Method(guiInt, setMsgSettingsUpdatedCbk));

    CanSignalDataCtx ctx(&guiInt.get());
    CanSignalData c2(std::move(ctx));
}

TEST_CASE("setConfig - qobj", "[cansignaldata]")
{
    CanSignalData c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[cansignaldata]")
{
    CanSignalData c;
    QJsonObject obj;
    QJsonArray arr;

    obj["name"] = "Test Name";

    obj["msgSettings"] = "abc";
    c.setConfig(obj);

    arr.append("dsds");
    arr.append("bbb");
    obj["msgSettings"] = arr;
    c.setConfig(obj);
}

TEST_CASE("getConfig", "[cansignaldata]")
{
    CanSignalData c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[cansignaldata]")
{
    CanSignalData c;

    auto abc = c.getQConfig();
}

TEST_CASE("start/stop", "[cansignaldata]")
{
    CanSignalData c;

    c.startSimulation();
    c.stopSimulation();
}

TEST_CASE("getSupportedProperties", "[cansignaldata]")
{
    CanSignalData c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 3);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);

    REQUIRE(ComponentInterface::propertyName(props[1]) == "file");
    REQUIRE(ComponentInterface::propertyType(props[1]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[1]) == true);
    REQUIRE(ComponentInterface::propertyField(props[1]) != nullptr);

    REQUIRE(ComponentInterface::propertyName(props[2]) == "color");
    REQUIRE(ComponentInterface::propertyType(props[2]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[2]) == true);
    REQUIRE(ComponentInterface::propertyField(props[2]) != nullptr);
}

TEST_CASE("loadDbc", "[cansignaldata]")
{
    CanSignalData c;
    QJsonObject obj;
    CANmessages_t msgs;

    QSignalSpy updateSpy(&c, &CanSignalData::simBcastSnd);

    obj["name"] = "Test Name";

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    c.setConfig(obj);
    c.configChanged();

    // Load again (this should not trigger another signal)
    c.setConfig(obj);
    c.configChanged();

    REQUIRE(updateSpy.count() == 1);
    msgs = qvariant_cast<CANmessages_t>(updateSpy.takeFirst().at(1));
    REQUIRE(msgs.size() == 17);

    // File does not exist
    obj["file"] = QString(DBC_PATH) + "/tesla_can";
    c.setConfig(obj);
    c.configChanged();

    REQUIRE(updateSpy.count() == 1);
    msgs = qvariant_cast<CANmessages_t>(updateSpy.takeFirst().at(1));
    REQUIRE(msgs.size() == 0);

    // Wrong file
    obj["file"] = QString(DBC_PATH) + "/project.cds";
    c.setConfig(obj);
    c.configChanged();

    REQUIRE(updateSpy.count() == 1);
    msgs = qvariant_cast<CANmessages_t>(updateSpy.takeFirst().at(1));
    REQUIRE(msgs.size() == 0);
}

QString testJson = R"(
{
    "msgSettings": [
        {
            "cycle": "123",
            "id": "3",
            "initVal": ""
        },
        {
            "cycle": "",
            "id": "e",
            "initVal": "1122334455667788"
        },
        {
            "cycle": "1234567890",
            "id": "45",
            "initVal": "aabbccddeeff0099"
        },
        {
            "cycle": "",
            "id": "6d",
            "initVal": "AABBCCDD"
        },
        {
            "cycle": "1",
            "id": "101",
            "initVal": "EEFF00"
        }
    ],
    "name": "CanSignalData"
}
)";

TEST_CASE("loadDbc with settings", "[cansignaldata]")
{
    CanSignalData c;
    QJsonObject obj = QJsonDocument::fromJson(testJson.toUtf8()).object();
    QSignalSpy updateSpy(&c, &CanSignalData::simBcastSnd);

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    c.setConfig(obj);
    c.configChanged();

    REQUIRE(updateSpy.count() == 1);
    auto msgs = qvariant_cast<CANmessages_t>(updateSpy.takeFirst().at(1));
    REQUIRE(msgs.size() == 17);

    auto msg = msgs.find({ 0x3 });
    REQUIRE(msg != std::end(msgs));
    REQUIRE(msg->first.updateCycle == 123);
    REQUIRE(msg->first.initValue == "");

    msg = msgs.find({ 0xe });
    REQUIRE(msg != std::end(msgs));
    REQUIRE(msg->first.updateCycle == 0);
    REQUIRE(msg->first.initValue == "1122334455667788");

    msg = msgs.find({ 0x45 });
    REQUIRE(msg != std::end(msgs));
    REQUIRE(msg->first.updateCycle == 1234567890);
    REQUIRE(msg->first.initValue == "aabbccddeeff0099");

    msg = msgs.find({ 0x6d });
    REQUIRE(msg != std::end(msgs));
    REQUIRE(msg->first.updateCycle == 0);
    REQUIRE(msg->first.initValue == "AABBCCDD");

    msg = msgs.find({ 0x101 });
    REQUIRE(msg != std::end(msgs));
    REQUIRE(msg->first.updateCycle == 1);
    REQUIRE(msg->first.initValue == "EEFF00");
}

TEST_CASE("dock/undock", "[cansignaldata]")
{
    CanSignalDataGuiInt::dockUndock_t dockUndock;

    using namespace fakeit;
    Mock<CanSignalDataGuiInt> guiInt;
    Fake(Method(guiInt, initSearch));
    Fake(Method(guiInt, setMsgView));
    Fake(Method(guiInt, setMsgViewCbk));
    When(Method(guiInt, setDockUndockCbk)).Do([&](auto&& fn) { dockUndock = fn; });
    Fake(Method(guiInt, setMsgSettingsUpdatedCbk));
    Fake(Method(guiInt, mainWidget));

    CanSignalDataCtx ctx(&guiInt.get());
    CanSignalData c(std::move(ctx));
    QSignalSpy dockSpy(&c, &CanSignalData::mainWidgetDockToggled);

    dockUndock();
    dockUndock();
    dockUndock();

    REQUIRE(dockSpy.count() == 3);

    Verify(Method(guiInt, mainWidget)).Exactly(3);
}

TEST_CASE("View switch test", "[cansignaldata]")
{
    CanSignalDataGuiInt::dockUndock_t dockUndock;
    CanSignalDataGuiInt::msgView_t msgView;
    CanSignalDataGuiInt::msgSettingsUpdated_t settingsUpdated;

    QAbstractItemModel* msgModel = nullptr;
    QAbstractItemModel* sigModel = nullptr;

    using namespace fakeit;
    Mock<CanSignalDataGuiInt> guiInt;
    Fake(Method(guiInt, initSearch));
    When(Method(guiInt, setMsgView)).AlwaysDo([&](auto&& model) { msgModel = &model; });
    When(Method(guiInt, setSigView)).AlwaysDo([&](auto&& model) { sigModel = &model; });
    When(Method(guiInt, setMsgViewCbk)).Do([&](auto&& fn) { msgView = fn; });
    When(Method(guiInt, setDockUndockCbk)).Do([&](auto&& fn) { dockUndock = fn; });
    When(Method(guiInt, setMsgSettingsUpdatedCbk)).Do([&](auto&& fn) { settingsUpdated = fn; });

    CanSignalDataCtx ctx(&guiInt.get());
    CanSignalData c2(std::move(ctx));

    REQUIRE(msgModel != nullptr);
    REQUIRE(sigModel == nullptr);

    msgView();

    REQUIRE(msgModel != nullptr);
    REQUIRE(sigModel != nullptr);
    REQUIRE(sigModel != msgModel);

    msgView();
    msgView();

    Verify(
        Method(guiInt, setMsgView), Method(guiInt, setSigView), Method(guiInt, setMsgView), Method(guiInt, setSigView));
}

TEST_CASE("settings callback", "[cansignaldata]")
{
    CanSignalDataGuiInt::msgSettingsUpdated_t settingsUpdated;
    QStandardItemModel* msgModel = nullptr;

    using namespace fakeit;
    Mock<CanSignalDataGuiInt> guiInt;
    Fake(Method(guiInt, initSearch));
    When(Method(guiInt, setMsgView)).AlwaysDo([&](auto&& model) {
        auto proxy = dynamic_cast<QAbstractProxyModel*>(&model);
        msgModel = dynamic_cast<QStandardItemModel*>(proxy->sourceModel());
    });
    Fake(Method(guiInt, setMsgViewCbk));
    Fake(Method(guiInt, setDockUndockCbk));
    When(Method(guiInt, setMsgSettingsUpdatedCbk)).Do([&](auto&& fn) { settingsUpdated = fn; });

    CanSignalDataCtx ctx(&guiInt.get());
    CanSignalData c(std::move(ctx));
    QSignalSpy settingsSpy(&c, &CanSignalData::simBcastSnd);
    QJsonObject obj;

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    c.setConfig(obj);
    c.configChanged();

    REQUIRE(settingsSpy.count() == 1);
    auto msgs = qvariant_cast<CANmessages_t>(settingsSpy.takeFirst().at(1));
    REQUIRE(msgs.size() == 17);

    auto msg = msgs.find({ 0x45 });
    REQUIRE(msg != std::end(msgs));
    REQUIRE(msg->first.updateCycle == 0);
    REQUIRE(msg->first.initValue == "");

    msgModel->setItem(2, 4, new QStandardItem("123"));
    msgModel->setItem(2, 5, new QStandardItem("456"));

    settingsUpdated();

    REQUIRE(settingsSpy.count() == 1);
    msgs = qvariant_cast<CANmessages_t>(settingsSpy.takeFirst().at(1));
    REQUIRE(msgs.size() == 17);

    msg = msgs.find({ 0x45 });
    REQUIRE(msg != std::end(msgs));
    REQUIRE(msg->first.updateCycle == 123);
    REQUIRE(msg->first.initValue == "456");

    QJsonObject settings = c.getConfig();
    REQUIRE(settings["msgSettings"].toArray()[0].toObject()["id"].toString() == "45");
    REQUIRE(settings["msgSettings"].toArray()[0].toObject()["cycle"].toString() == "123");
    REQUIRE(settings["msgSettings"].toArray()[0].toObject()["initVal"].toString() == "456");
}

TEST_CASE("Filter test", "[cansignaldata]")
{
    CanSignalDataGuiInt::dockUndock_t dockUndock;
    CanSignalDataGuiInt::msgView_t msgView;
    CanSignalDataGuiInt::msgSettingsUpdated_t settingsUpdated;

    SearchModel* msgModel = nullptr;
    SearchModel* sigModel = nullptr;

    using namespace fakeit;
    Mock<CanSignalDataGuiInt> guiInt;
    Fake(Method(guiInt, initSearch));
    When(Method(guiInt, setMsgView)).AlwaysDo([&](auto&& model) { msgModel = dynamic_cast<SearchModel*>(&model); });
    When(Method(guiInt, setSigView)).AlwaysDo([&](auto&& model) { sigModel = dynamic_cast<SearchModel*>(&model); });
    When(Method(guiInt, setMsgViewCbk)).Do([&](auto&& fn) { msgView = fn; });
    When(Method(guiInt, setDockUndockCbk)).Do([&](auto&& fn) { dockUndock = fn; });
    When(Method(guiInt, setMsgSettingsUpdatedCbk)).Do([&](auto&& fn) { settingsUpdated = fn; });

    CanSignalDataCtx ctx(&guiInt.get());
    CanSignalData c(std::move(ctx));
    QJsonObject obj = QJsonDocument::fromJson(testJson.toUtf8()).object();

    obj["file"] = QString(DBC_PATH) + "/tesla_can.dbc";
    c.setConfig(obj);
    c.configChanged();

    REQUIRE(msgModel != nullptr);
    REQUIRE(sigModel == nullptr);

    REQUIRE(msgModel->rowCount() == 17);

    msgView();

    REQUIRE(msgModel != nullptr);
    REQUIRE(sigModel != nullptr);
    REQUIRE(sigModel != msgModel);

    REQUIRE(msgModel->rowCount() == 17);
    REQUIRE(msgModel->isFilterActive() == false);

    // filter msg by Id
    msgModel->updateFilter("0x45");
    REQUIRE(msgModel->rowCount() == 1);
    REQUIRE(msgModel->isFilterActive() == true);

    // filter msg by name
    msgModel->updateFilter("STW_");
    REQUIRE(msgModel->rowCount() == 3);
    REQUIRE(msgModel->isFilterActive() == true);

    // filter msg by id, dlc and cycle
    msgModel->updateFilter("5");
    REQUIRE(msgModel->rowCount() == 3);
    REQUIRE(msgModel->isFilterActive() == true);

    // filter msg by ecu
    msgModel->updateFilter("DI");
    REQUIRE(msgModel->rowCount() == 3);
    REQUIRE(msgModel->isFilterActive() == true);

    // filter msg by cycle
    msgModel->updateFilter("123456789");
    REQUIRE(msgModel->rowCount() == 1);
    REQUIRE(msgModel->isFilterActive() == true);

    // filter msg by initVal
    msgModel->updateFilter("aabbccddeeff0099");
    REQUIRE(msgModel->rowCount() == 1);
    REQUIRE(msgModel->isFilterActive() == true);

    // cancel msg filter
    msgModel->updateFilter("");
    REQUIRE(msgModel->rowCount() == 17);
    REQUIRE(msgModel->isFilterActive() == false);

    REQUIRE(sigModel->rowCount() == 199);
    REQUIRE(sigModel->isFilterActive() == false);

    // filter sig by Id
    sigModel->updateFilter("0x45");
    REQUIRE(sigModel->rowCount() == 31);
    REQUIRE(sigModel->isFilterActive() == true);

    // filter sig by name
    sigModel->updateFilter("DTR_");
    REQUIRE(sigModel->rowCount() == 1);
    REQUIRE(sigModel->isFilterActive() == true);
    sigModel->updateFilter("");

    // filter sig by start bit
    sigModel->updateFilter("22");
    REQUIRE(sigModel->rowCount() == 1);
    REQUIRE(sigModel->isFilterActive() == true);

    // filter sig by length
    sigModel->updateFilter("14");
    REQUIRE(sigModel->rowCount() == 10);
    REQUIRE(sigModel->isFilterActive() == true);

    // filter sig by type
    sigModel->updateFilter("unsigned");
    REQUIRE(sigModel->rowCount() == 136);
    REQUIRE(sigModel->isFilterActive() == true);

    // filter sig by offset and min
    sigModel->updateFilter("-4096");
    REQUIRE(sigModel->rowCount() == 1);
    REQUIRE(sigModel->isFilterActive() == true);

    // filter sig by max
    sigModel->updateFilter("4095");
    REQUIRE(sigModel->rowCount() == 1);
    REQUIRE(sigModel->isFilterActive() == true);
    // filter sig by offset and min
    sigModel->updateFilter("-4096");
    REQUIRE(sigModel->rowCount() == 1);
    REQUIRE(sigModel->isFilterActive() == true);

    sigModel->updateFilter("");
    REQUIRE(sigModel->rowCount() == 199);
    REQUIRE(sigModel->isFilterActive() == false);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Staring unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when contructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
