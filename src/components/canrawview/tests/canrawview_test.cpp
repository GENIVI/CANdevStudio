#define CATCH_CONFIG_RUNNER
#include <QCanBusFrame>
#include <QJsonArray>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTableView>
#include <Qt>
#include <QtWidgets/QApplication>
#include <catch.hpp>
#include <context.h>
#include <fakeit.hpp>

#include <canrawview.h>
#include <crvsortmodel.h>
#include <gui/crvguiinterface.h>
#include <log.h>
#include <memory>

#include <iostream>
std::shared_ptr<spdlog::logger> kDefaultLogger;
using namespace fakeit;

#include <QStandardItemModel>

class CanRawViewPrivate;

void addNewFrame(uint& rowID, double time, uint frameID, uint data, QStandardItemModel& tvModel)
{
    QList<QStandardItem*> list;

    list.append(new QStandardItem(QString::number(rowID++)));
    list.append(new QStandardItem(QString::number(time)));
    list.append(new QStandardItem(std::move(frameID)));
    list.append(new QStandardItem("TX"));
    list.append(new QStandardItem(QString::number(4)));
    list.append(new QStandardItem(QString::number(data)));

    tvModel.appendRow(list);
}

TEST_CASE("Initialize table", "[canrawview]")
{
    Mock<CRVGuiInterface> crvMock;

    Fake(Method(crvMock, setClearCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setFilterCbk));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, setSorting));

    Fake(Method(crvMock, initTableView));
    Fake(Method(crvMock, scrollToBottom));
    When(Method(crvMock, mainWidget)).Return(NULL);
    When(Method(crvMock, isViewFrozen)).Return(false).Return(true).Return(true).Return(false).Return(true).Return(true);
    When(Method(crvMock, getSortOrder)).Return(Qt::AscendingOrder);
    When(Method(crvMock, getClickedColumn)).Return("rowID", "time", "id", "dir", "dlc", "data");
    When(Method(crvMock, isColumnHidden)).Return(true, false, false, false, false, false);

    CRVGuiInterface& i = crvMock.get();
    CanRawView canRawView{ CanRawViewCtx(&i) };

    // Verify(Method(crvMock, setSorting).Using(0, Qt::AscendingOrder));
    // Verify(Method(crvMock, getClickedColumn).Using(0));
    // Verify(Method(crvMock, getClickedColumn).Using(0));

    QCanBusFrame frame;
    frame.setPayload({ "123" });

    REQUIRE_NOTHROW(canRawView.startSimulation());

    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
    REQUIRE_NOTHROW(canRawView.frameSent(true, frame));
    REQUIRE_NOTHROW(canRawView.frameSent(false, frame));

    frame.setExtendedFrameFormat(true);
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
    REQUIRE_NOTHROW(canRawView.frameSent(true, frame));
    REQUIRE_NOTHROW(canRawView.frameSent(false, frame));

    REQUIRE_NOTHROW(canRawView.stopSimulation());
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
}

TEST_CASE("Sort test", "[canrawview]")
{
    QStandardItemModel _tvModel;
    CRVSortModel _sortModel;
    QTableView _tableView;
    uint rowID = 0;

    _sortModel.setSourceModel(&_tvModel);
    _tableView.setModel(&_sortModel);

    // rowID, time, frameID, data//
    addNewFrame(rowID, 0.20, 10, 1, _tvModel);
    addNewFrame(rowID, 1.00, 1, 110, _tvModel);
    addNewFrame(rowID, 10.00, 101, 1000, _tvModel);
    addNewFrame(rowID, 11.00, 11, 11, _tvModel);

    for (int i = 0; i < 4; ++i) {
        _sortModel.sort(i, Qt::AscendingOrder);
        _sortModel.sort(i, Qt::DescendingOrder);
    }

    REQUIRE(_tvModel.rowCount() == 4);
    REQUIRE(_sortModel.isFilterActive() == false);
    // TODO spy sectionClicked signal...
}

TEST_CASE("setConfig using QObject", "[canrawview]")
{
    CanRawView crv;
    QWidget config;

    config.setProperty("name", "CAN1");
    config.setProperty("fake", "unsupported");

    crv.setConfig(config);

    auto qConfig = crv.getQConfig();

    REQUIRE(qConfig->property("name").toString() == "CAN1");
    REQUIRE(qConfig->property("fake").isValid() == false);
}

TEST_CASE("Restore config paths", "[canrawview]")
{
    CanRawView canRawView;
    QJsonObject json;
    QJsonArray columnArray;
    QJsonObject columnItem;

    // No viewColumns
    canRawView.setConfig(json);

    // ViewColumn is not an array
    json["viewColumns"] = "";
    canRawView.setConfig(json);

    // Array size != 5
    columnItem["dummy"] = 123;
    columnArray.append(columnItem);
    json["viewColumns"] = columnArray;
    canRawView.setConfig(json);

    // Array item is not an obj
    columnArray = QJsonArray();
    QJsonValue val;
    columnArray.append(val);
    columnArray.append(val);
    columnArray.append(val);
    columnArray.append(val);
    columnArray.append(val);
    json["viewColumns"] = columnArray;
    canRawView.setConfig(json);

    // name does not exist
    columnArray = QJsonArray();
    columnItem = QJsonObject();
    columnItem["dummy"] = 123;
    columnItem["dummy2"] = 234;
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    json["viewColumns"] = columnArray;
    canRawView.setConfig(json);

    // name is not a string
    columnArray = QJsonArray();
    columnItem = QJsonObject();
    columnItem["name"] = 123;
    columnItem["dummy2"] = 234;
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    json["viewColumns"] = columnArray;
    canRawView.setConfig(json);

    // vIdx does not exist
    columnArray = QJsonArray();
    columnItem = QJsonObject();
    columnItem["name"] = "time";
    columnItem["dummy2"] = 234;
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    json["viewColumns"] = columnArray;
    canRawView.setConfig(json);

    // vIdx is not number
    columnArray = QJsonArray();
    columnItem = QJsonObject();
    columnItem["name"] = "time";
    columnItem["vIdx"] = "dsds";
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    json["viewColumns"] = columnArray;
    canRawView.setConfig(json);

    // Column not found
    columnArray = QJsonArray();
    columnItem = QJsonObject();
    columnItem["name"] = "Blah";
    columnItem["vIdx"] = 1;
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    columnArray.append(columnItem);
    json["viewColumns"] = columnArray;
    canRawView.setConfig(json);

    // No scrolling item
    columnArray = QJsonArray();
    columnItem = QJsonObject();
    columnItem["name"] = "time";
    columnItem["vIdx"] = 2;
    columnArray.append(columnItem);
    columnItem["name"] = "id";
    columnItem["vIdx"] = 3;
    columnArray.append(columnItem);
    columnItem["name"] = "dir";
    columnItem["vIdx"] = 4;
    columnArray.append(columnItem);
    columnItem["name"] = "dlc";
    columnItem["vIdx"] = 5;
    columnArray.append(columnItem);
    columnItem["name"] = "data";
    columnItem["vIdx"] = 6;
    columnArray.append(columnItem);
    json["viewColumns"] = columnArray;
    canRawView.setConfig(json);
}

TEST_CASE("Misc", "[canrawview]")
{
    CanRawView canRawView;

    REQUIRE(canRawView.mainWidgetDocked() == true);
    REQUIRE(canRawView.mainWidget() != nullptr);
}

int main(int argc, char* argv[])
{
    bool haveDebug = std::getenv("CDS_DEBUG") != nullptr;
    kDefaultLogger = spdlog::stdout_color_mt("cds");
    if (haveDebug) {
        kDefaultLogger->set_level(spdlog::level::debug);
    }
    cds_debug("Starting canrawview unit tests");
    QApplication a(argc, argv);
    return Catch::Session().run(argc, argv);
}

TEST_CASE("Dock/Undock", "[canrawview]")
{
    CRVGuiInterface::dockUndock_t dockUndock;

    Mock<CRVGuiInterface> crvMock;
    Fake(Method(crvMock, setClearCbk));
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setFilterCbk));
    When(Method(crvMock, setDockUndockCbk)).Do([&](auto&& fn) { dockUndock = fn; });
    Fake(Method(crvMock, mainWidget));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, initTableView));

    CanRawView canRawView{ CanRawViewCtx(&crvMock.get()) };
    QSignalSpy dockSpy(&canRawView, &CanRawView::mainWidgetDockToggled);

    REQUIRE(canRawView.mainWidgetDocked() == true);

    dockUndock();

    REQUIRE(dockSpy.count() == 1);
    REQUIRE(canRawView.mainWidgetDocked() == false);

    dockUndock();

    REQUIRE(dockSpy.count() == 2);
    REQUIRE(canRawView.mainWidgetDocked() == true);
}

TEST_CASE("Section clicked", "[canrawview]")
{
    CRVGuiInterface::sectionClicked_t sectionClicked;
    QAbstractItemModel* model = nullptr;

    Mock<CRVGuiInterface> crvMock;
    Fake(Method(crvMock, setClearCbk));
    When(Method(crvMock, setSectionClikedCbk)).AlwaysDo([&](auto&& fn) { sectionClicked = fn; });
    Fake(Method(crvMock, setFilterCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, mainWidget));
    When(Method(crvMock, setModel)).AlwaysDo([&](auto&& m) { model = m; });
    When(Method(crvMock, setSorting)).AlwaysDo([&](int sortNdx, Qt::SortOrder order) {
        REQUIRE(model != nullptr);
        model->sort(sortNdx, order);
    });
    Fake(Method(crvMock, initTableView));
    When(Method(crvMock, getSortOrder))
        .Return(Qt::AscendingOrder, Qt::DescendingOrder, Qt::AscendingOrder, Qt::AscendingOrder, Qt::AscendingOrder,
            Qt::AscendingOrder, Qt::AscendingOrder);
    Fake(Method(crvMock, isViewFrozen));
    Fake(Method(crvMock, scrollToBottom));

    CanRawView canRawView{ CanRawViewCtx(&crvMock.get()) };

    QCanBusFrame frame;
    frame.setFrameId(11);
    frame.setPayload({ "123" });
    REQUIRE_NOTHROW(canRawView.startSimulation());
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));

    frame.setFrameId(12);
    frame.setPayload({ "1234" });
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));

    frame.setFrameId(123);
    frame.setPayload({ "12345" });
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));

    sectionClicked(1);
    sectionClicked(1);
    sectionClicked(1);

    Verify(Method(crvMock, setSorting).Using(1, Qt::AscendingOrder),
        Method(crvMock, setSorting).Using(1, Qt::DescendingOrder),
        Method(crvMock, setSorting).Using(0, Qt::AscendingOrder))
        .Exactly(Once);

    sectionClicked(1);
    sectionClicked(2);
    sectionClicked(3);
    sectionClicked(4);

    Verify(Method(crvMock, setSorting).Using(1, Qt::AscendingOrder),
        Method(crvMock, setSorting).Using(2, Qt::AscendingOrder),
        Method(crvMock, setSorting).Using(3, Qt::AscendingOrder),
        Method(crvMock, setSorting).Using(4, Qt::AscendingOrder))
        .Exactly(Once);
}

TEST_CASE("Filter callback", "[canrawview]")
{
    CRVGuiInterface::filter_t filter;
    QAbstractItemModel* model = nullptr;

    Mock<CRVGuiInterface> crvMock;
    Fake(Method(crvMock, setClearCbk));
    When(Method(crvMock, setFilterCbk)).Do([&](auto&& fn) { filter = fn; });
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, mainWidget));
    When(Method(crvMock, setModel)).AlwaysDo([&](auto&& m) { model = m; });
    Fake(Method(crvMock, setSorting));
    Fake(Method(crvMock, initTableView));
    When(Method(crvMock, getSortOrder)).Return(Qt::AscendingOrder, Qt::DescendingOrder, Qt::AscendingOrder);
    Fake(Method(crvMock, isViewFrozen));
    Fake(Method(crvMock, scrollToBottom));

    CanRawView canRawView{ CanRawViewCtx(&crvMock.get()) };

    filter(true);
    REQUIRE_NOTHROW(canRawView.startSimulation());

    REQUIRE(model != nullptr);
    model->sort(0, Qt::AscendingOrder);

    QCanBusFrame frame;
    frame.setFrameId(11);
    frame.setPayload({ "123" });
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
    REQUIRE_NOTHROW(canRawView.frameSent(true, frame));
    REQUIRE_NOTHROW(canRawView.frameSent(true, frame));

    frame.setFrameId(12);
    frame.setPayload({ "1234" });
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
    REQUIRE_NOTHROW(canRawView.frameSent(true, frame));

    frame.setFrameId(123);
    frame.setPayload({ "12345" });
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
    REQUIRE_NOTHROW(canRawView.frameSent(true, frame));

    filter(false);
}

TEST_CASE("Stress test", "[canrawview]")
{
    CRVGuiInterface::filter_t filter;

    Mock<CRVGuiInterface> crvMock;
    Fake(Method(crvMock, setClearCbk));
    When(Method(crvMock, setFilterCbk)).Do([&](auto&& fn) { filter = fn; });
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, mainWidget));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, setSorting));
    Fake(Method(crvMock, initTableView));
    When(Method(crvMock, getSortOrder)).Return(Qt::AscendingOrder, Qt::DescendingOrder, Qt::AscendingOrder);
    Fake(Method(crvMock, isViewFrozen));
    Fake(Method(crvMock, scrollToBottom));

    CanRawView canRawView{ CanRawViewCtx(&crvMock.get()) };

    filter(true);

    REQUIRE_NOTHROW(canRawView.startSimulation());

    QCanBusFrame frame;
    frame.setPayload({ "123" });

    for (int i = 0; i < 0x7ff; ++i) {
        frame.setFrameId(i);
        REQUIRE_NOTHROW(canRawView.frameReceived(frame));
        REQUIRE_NOTHROW(canRawView.frameSent(true, frame));
    }

    filter(false);
}
