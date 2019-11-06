#include <QtWidgets/QApplication>
#include <cansignalsender.h>
#define CATCH_CONFIG_RUNNER
#include "log.h"
#include <QCanBusFrame>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <catch.hpp>
#include <fakeit.hpp>
#include <gui/cansignalsenderguiint.h>
#include <bcastmsgs.h>

std::shared_ptr<spdlog::logger> kDefaultLogger;
// needed for QSignalSpy cause according to qtbug 49623 comments
// automatic detection of types is "flawed" in moc
Q_DECLARE_METATYPE(QCanBusFrame);

TEST_CASE("Stubbed methods", "[cansignalsender]")
{
    CanSignalSender c;

    REQUIRE(c.mainWidget() != nullptr);
    REQUIRE(c.mainWidgetDocked() == true);
}

TEST_CASE("setConfig - qobj", "[cansignalsender]")
{
    CanSignalSender c;
    QWidget obj;

    obj.setProperty("name", "Test Name");

    c.setConfig(obj);
}

TEST_CASE("setConfig - json", "[cansignalsender]")
{
    using namespace fakeit;
    Mock<CanSignalSenderGuiInt> guiInt;
    Fake(Method(guiInt, initTv));
    Fake(Method(guiInt, setRemoveCbk));
    Fake(Method(guiInt, setAddCbk));
    Fake(Method(guiInt, setDockUndockCbk));
    Fake(Method(guiInt, setSendCbk));
    Fake(Method(guiInt, addRow));
    Fake(Method(guiInt, getRows));
    Fake(Method(guiInt, getSelectedRows));

    CanSignalSenderCtx ctx(&guiInt.get());
    CanSignalSender c(std::move(ctx));

    QJsonObject obj;
    QJsonArray arr;
    QJsonObject el;
    obj["name"] = "Test Name";

    // No rows item
    c.setConfig(obj);

    // Not an array
    obj["rows"] = "not an array";
    c.setConfig(obj);

    // Empty Array
    obj["rows"] = arr;
    c.setConfig(obj);

    // Array with not_an_object
    arr.append("Should be object");
    obj["rows"] = arr;
    c.setConfig(obj);

    // id only
    arr = QJsonArray();
    el = QJsonObject();
    el["id"] = "0x33";
    arr.append(el);
    obj["rows"] = arr;
    c.setConfig(obj);

    // id, sig, val
    arr = QJsonArray();
    el = QJsonObject();
    el["id"] = "0x33";
    el["sig"] = "ABC";
    el["val"] = "123";
    arr.append(el);
    obj["rows"] = arr;
    c.setConfig(obj);

    Verify(Method(guiInt, addRow)).Exactly(2);
}

TEST_CASE("getConfig", "[cansignalsender]")
{
    CanSignalSender c;

    auto abc = c.getConfig();
}

TEST_CASE("getQConfig", "[cansignalsender]")
{
    CanSignalSender c;

    auto abc = c.getQConfig();
}

TEST_CASE("configChanged", "[cansignalsender]")
{
    CanSignalSender c;

    c.configChanged();
}

TEST_CASE("getSupportedProperties", "[cansignalsender]")
{
    CanSignalSender c;

    auto props = c.getSupportedProperties();

    REQUIRE(props.size() == 2);

    REQUIRE(ComponentInterface::propertyName(props[0]) == "name");
    REQUIRE(ComponentInterface::propertyType(props[0]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[0]) == true);
    REQUIRE(ComponentInterface::propertyField(props[0]) == nullptr);

    REQUIRE(ComponentInterface::propertyName(props[1]) == "CAN database");
    REQUIRE(ComponentInterface::propertyType(props[1]) == QVariant::String);
    REQUIRE(ComponentInterface::propertyEditability(props[1]) == true);
    REQUIRE(ComponentInterface::propertyField(props[1]) != nullptr);
}

TEST_CASE("row removing", "[cansignalsender]")
{
    QStandardItemModel* model;
    CanSignalSenderGuiInt::remove_t removeCbk;

    using namespace fakeit;
    Mock<CanSignalSenderGuiInt> guiInt;
    When(Method(guiInt, initTv)).AlwaysDo([&](auto&& m, auto&&) { model = &m; });
    When(Method(guiInt, setRemoveCbk)).AlwaysDo([&](auto&& cbk) { removeCbk = cbk; });
    Fake(Method(guiInt, setAddCbk));
    Fake(Method(guiInt, setDockUndockCbk));
    Fake(Method(guiInt, setSendCbk));
    Fake(Method(guiInt, addRow));
    Fake(Method(guiInt, getRows));
    When(Method(guiInt, getSelectedRows)).Do([&] {
        QModelIndexList i;

        i.append(model->index(0, 0));
        i.append(model->index(2, 0));

        return i;
    });

    CanSignalSenderCtx ctx(&guiInt.get());
    CanSignalSender c(std::move(ctx));

    QList<QStandardItem*> list1{ new QStandardItem("id_1"), new QStandardItem("sig_1"), new QStandardItem("val_1") };
    QList<QStandardItem*> list2{ new QStandardItem("id_2"), new QStandardItem("sig_2"), new QStandardItem("val_2") };
    QList<QStandardItem*> list3{ new QStandardItem("id_3"), new QStandardItem("sig_3"), new QStandardItem("val_3") };

    model->appendRow(list1);
    model->appendRow(list2);
    model->appendRow(list3);

    REQUIRE(model->rowCount() == 3);

    REQUIRE(model->data(model->index(0, 0)).toString() == "id_1");
    REQUIRE(model->data(model->index(0, 1)).toString() == "sig_1");
    REQUIRE(model->data(model->index(0, 2)).toString() == "val_1");

    REQUIRE(model->data(model->index(1, 0)).toString() == "id_2");
    REQUIRE(model->data(model->index(1, 1)).toString() == "sig_2");
    REQUIRE(model->data(model->index(1, 2)).toString() == "val_2");

    REQUIRE(model->data(model->index(2, 0)).toString() == "id_3");
    REQUIRE(model->data(model->index(2, 1)).toString() == "sig_3");
    REQUIRE(model->data(model->index(2, 2)).toString() == "val_3");

    removeCbk();

    REQUIRE(model->rowCount() == 1);

    REQUIRE(model->data(model->index(0, 0)).toString() == "id_2");
    REQUIRE(model->data(model->index(0, 1)).toString() == "sig_2");
    REQUIRE(model->data(model->index(0, 2)).toString() == "val_2");
}

TEST_CASE("dock/undock", "[cansignaldata]")
{
    CanSignalSenderGuiInt::dockUndock_t dockUndock;

    using namespace fakeit;
    Mock<CanSignalSenderGuiInt> guiInt;
    Fake(Method(guiInt, initTv));
    Fake(Method(guiInt, setRemoveCbk));
    Fake(Method(guiInt, setAddCbk));
    When(Method(guiInt, setDockUndockCbk)).Do([&](auto&& fn) { dockUndock = fn; });
    Fake(Method(guiInt, setSendCbk));
    Fake(Method(guiInt, addRow));
    Fake(Method(guiInt, getRows));
    Fake(Method(guiInt, getSelectedRows));
    Fake(Method(guiInt, mainWidget));

    CanSignalSenderCtx ctx(&guiInt.get());
    CanSignalSender c(std::move(ctx));
    QSignalSpy dockSpy(&c, &CanSignalSender::mainWidgetDockToggled);

    dockUndock();
    dockUndock();
    dockUndock();

    REQUIRE(dockSpy.count() == 3);

    Verify(Method(guiInt, mainWidget)).Exactly(3);
}

TEST_CASE("signal sending", "[cansignaldata]")
{
    CanSignalSenderGuiInt::send_t sendCbk;

    using namespace fakeit;
    Mock<CanSignalSenderGuiInt> guiInt;
    Fake(Method(guiInt, initTv));
    Fake(Method(guiInt, setRemoveCbk));
    Fake(Method(guiInt, setAddCbk));
    Fake(Method(guiInt, setDockUndockCbk));
    When(Method(guiInt, setSendCbk)).Do([&] (auto&& cbk) { sendCbk = cbk; });
    Fake(Method(guiInt, addRow));
    Fake(Method(guiInt, getRows));
    Fake(Method(guiInt, getSelectedRows));
    Fake(Method(guiInt, mainWidget));

    CanSignalSenderCtx ctx(&guiInt.get());
    CanSignalSender c(std::move(ctx));
    QSignalSpy sendSpy(&c, &CanSignalSender::sendSignal);

    sendCbk("0x33", "ABC", 10);

    REQUIRE(sendSpy.count() == 0);

    c.startSimulation();

    sendCbk("0x33", "ABC", 10);
    sendCbk("0x7ff", "DEF", 2222);
    sendCbk("0x3544", "GHI", 5678);

    REQUIRE(sendSpy.count() == 3);

    c.stopSimulation();

    sendCbk("0x123", "YYY", 5678);

    REQUIRE(sendSpy.count() == 3);

    auto args = sendSpy.takeFirst();
    auto id = args.at(0).toString();
    auto val = args.at(1);

    REQUIRE(id == "0x033_ABC");
    REQUIRE(val.toUInt() == 10);

    args = sendSpy.takeFirst();
    id = args.at(0).toString();
    val = args.at(1);

    REQUIRE(id == "0x7ff_DEF");
    REQUIRE(val.toUInt() == 2222);

    args = sendSpy.takeFirst();
    id = args.at(0).toString();
    val = args.at(1);

    REQUIRE(id == "0x00003544_GHI");
    REQUIRE(val.toUInt() == 5678);

}

TEST_CASE("bcast rcv", "[cansignaldata]")
{
    CanSignalSender c;

    QJsonObject msg;
    msg["id"] = "{00000000-0000-0000-0000-000000000001}";
    msg["msg"] = BcastMsg::CanDbUpdated;
    msg["caption"] = "name";
    msg["color"] = "color_321";

    c.simBcastRcv(msg, {});

    auto props = c.getQConfig();

    REQUIRE(props->property("color").toString() == "color_321");

}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting unit tests");
    qRegisterMetaType<QCanBusFrame>(); // required by QSignalSpy
    QApplication a(argc, argv); // QApplication must exist when constructing QWidgets TODO check QTest
    return Catch::Session().run(argc, argv);
}
