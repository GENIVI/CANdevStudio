#define CATCH_CONFIG_RUNNER
#include <QCanBusFrame>
#include <QJsonArray>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTableView>
#include <Qt>
#include <QtWidgets/QApplication>
#include <context.h>
#include <fakeit.hpp>

#include <log.h>
#include <memory>

#define private public
#include <canrawview.h>
#include <gui/crvguiinterface.h>
#include <uniquefiltermodel.h>
#undef private

#include <iostream>
std::shared_ptr<spdlog::logger> kDefaultLogger;
using namespace fakeit;

#include <QStandardItemModel>

class CanRawViewPrivate;

void addNewFrame(
    uint& rowID, double time, uint frameID, uint data, QStandardItemModel& tvModel, UniqueFilterModel& uniqueModel)
{
    QList<QStandardItem*> list;

    list.append(new QStandardItem(QString::number(rowID++)));
    list.append(new QStandardItem(QString::number(time)));
    list.append(new QStandardItem(std::move(frameID)));
    list.append(new QStandardItem("TX"));
    list.append(new QStandardItem(QString::number(4)));
    list.append(new QStandardItem(QString::number(data)));

    tvModel.appendRow(list);

    //_ui.setSorting(_sortIndex, _ui.getSortOrder());
    uniqueModel.updateFilter(QString::number(frameID), QString::number(time), "TX");
}

TEST_CASE("Initialize table", "[canrawview]")
{
    Mock<CRVGuiInterface> crvMock;
    Fake(Dtor(crvMock));

    Fake(Method(crvMock, setClearCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setFilterCbk));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, setSorting));

    Fake(Method(crvMock, initTableView));
    Fake(Method(crvMock, scrollToBottom));
    When(Method(crvMock, mainWidget)).Return(NULL);
    When(Method(crvMock, isViewFrozen)).Return(false).Return(true).Return(true);
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

    REQUIRE_NOTHROW(canRawView.stopSimulation());
    REQUIRE_NOTHROW(canRawView.frameReceived(frame));
}

TEST_CASE("Unique filter test", "[canrawview]")
{
    QStandardItemModel _tvModel;
    UniqueFilterModel _uniqueModel;
    QTableView _tableView;
    uint rowID = 0;

    _uniqueModel.setSourceModel(&_tvModel);
    _tableView.setModel(&_uniqueModel);

    QCanBusFrame testFrame;
    testFrame.setFrameId(123);

    // rowID, time, frameID, data
    addNewFrame(rowID, 0.20, 1, 0, _tvModel, _uniqueModel);
    addNewFrame(rowID, 0.40, 2, 0, _tvModel, _uniqueModel);
    addNewFrame(rowID, 0.60, 3, 0, _tvModel, _uniqueModel);
    addNewFrame(rowID, 0.80, 2, 20, _tvModel, _uniqueModel); // duplicate frameID 2
    addNewFrame(rowID, 1.00, 1, 10, _tvModel, _uniqueModel); // duplicate frameID 1

    CHECK(_tvModel.rowCount() == 5);
    CHECK(_uniqueModel.isFilterActive() == false);
    _uniqueModel.toggleFilter();
    CHECK(_uniqueModel.isFilterActive() == true);
    CHECK(_uniqueModel._uniques.size() == 3);

    _uniqueModel.clearFilter();
    CHECK(_uniqueModel._uniques.size() == 0);
    CHECK(_tvModel.rowCount() == 5);

    QModelIndex rowIDidx_0 = _tvModel.index(0, 0);
    QModelIndex rowIDidx_1 = _tvModel.index(1, 0);
    uint rowID_0 = _tvModel.data(rowIDidx_0).toUInt();
    uint rowID_1 = _tvModel.data(rowIDidx_1).toUInt();
    CHECK(rowID_0 == rowID_1 - 1);
}

TEST_CASE("Sort test", "[canrawview]")
{
    QStandardItemModel _tvModel;
    UniqueFilterModel _uniqueModel;
    QTableView _tableView;
    uint rowID = 0;

    _uniqueModel.setSourceModel(&_tvModel);
    _tableView.setModel(&_uniqueModel);

    // rowID, time, frameID, data//
    addNewFrame(rowID, 0.20, 10, 1, _tvModel, _uniqueModel);
    addNewFrame(rowID, 1.00, 1, 110, _tvModel, _uniqueModel);
    addNewFrame(rowID, 10.00, 101, 1000, _tvModel, _uniqueModel);
    addNewFrame(rowID, 11.00, 11, 11, _tvModel, _uniqueModel);

    _uniqueModel.toggleFilter();

    for (int i = 0; i < 4; ++i) {
        _uniqueModel.sort(i, Qt::AscendingOrder);
        _uniqueModel.sort(i, Qt::DescendingOrder);
    }

    CHECK(_tvModel.rowCount() == 4);
    CHECK(_uniqueModel.isFilterActive() == true);
    // TODO spy sectionClicked signal...
}

TEST_CASE("setConfig using QObject", "[canrawview]")
{
    CanRawView crv;
    QObject config;

    config.setProperty("name", "CAN1");
    config.setProperty("fake", "unsupported");

    crv.setConfig(config);

    auto qConfig = crv.getQConfig();

    CHECK(qConfig->property("name").toString() == "CAN1");
    CHECK(qConfig->property("fake").isValid() == false);
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

    CHECK(canRawView.mainWidgetDocked() == true);
    CHECK(canRawView.mainWidget() != nullptr);
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
    Fake(Dtor(crvMock));
    Fake(Method(crvMock, setClearCbk));
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setFilterCbk));
    When(Method(crvMock, setDockUndockCbk)).Do([&](auto&& fn) { dockUndock = fn; });
    Fake(Method(crvMock, mainWidget));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, initTableView));

    CanRawView canRawView{ CanRawViewCtx(&crvMock.get()) };
    QSignalSpy dockSpy(&canRawView, &CanRawView::mainWidgetDockToggled);

    CHECK(canRawView.mainWidgetDocked() == true);

    dockUndock();

    CHECK(dockSpy.count() == 1);
    CHECK(canRawView.mainWidgetDocked() == false);

    dockUndock();

    CHECK(dockSpy.count() == 2);
    CHECK(canRawView.mainWidgetDocked() == true);
}

TEST_CASE("Section clicked", "[canrawview]")
{
    CRVGuiInterface::sectionClicked_t sectionClicked;

    Mock<CRVGuiInterface> crvMock;
    Fake(Dtor(crvMock));
    Fake(Method(crvMock, setClearCbk));
    When(Method(crvMock, setSectionClikedCbk)).Do([&](auto&& fn) { sectionClicked = fn; });
    Fake(Method(crvMock, setFilterCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, mainWidget));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, setSorting));
    Fake(Method(crvMock, initTableView));
    When(Method(crvMock, getSortOrder)).Return(Qt::AscendingOrder, Qt::DescendingOrder, Qt::AscendingOrder);

    CanRawView canRawView{ CanRawViewCtx(&crvMock.get()) };

    sectionClicked(1);
    sectionClicked(1);
    sectionClicked(1);

    Verify(Method(crvMock, setSorting).Using(1, Qt::AscendingOrder),
        Method(crvMock, setSorting).Using(1, Qt::DescendingOrder),
        Method(crvMock, setSorting).Using(0, Qt::AscendingOrder))
        .Exactly(Once);
}

TEST_CASE("Filter callback", "[canrawview]")
{
    CRVGuiInterface::filter_t filter;

    Mock<CRVGuiInterface> crvMock;
    Fake(Dtor(crvMock));
    Fake(Method(crvMock, setClearCbk));
    When(Method(crvMock, setFilterCbk)).Do([&](auto&& fn) { filter = fn; });
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, mainWidget));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, setSorting));
    Fake(Method(crvMock, initTableView));
    When(Method(crvMock, getSortOrder)).Return(Qt::AscendingOrder, Qt::DescendingOrder, Qt::AscendingOrder);

    CanRawView canRawView{ CanRawViewCtx(&crvMock.get()) };

    filter();
}
