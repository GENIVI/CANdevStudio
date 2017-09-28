#define CATCH_CONFIG_RUNNER
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTableView>

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

    //  Q_D(CanRawView);

    Mock<CRVGuiInterface> crvMock;
    Fake(Dtor(crvMock));

    Fake(Method(crvMock, setClearCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setFilterCbk));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, setSorting));

    Fake(Method(crvMock, initTableView));
    When(Method(crvMock, isViewFrozen)).Return(false);
    When(Method(crvMock, getSortOrder)).Return(Qt::AscendingOrder);
    When(Method(crvMock, getClickedColumn)).Return("rowID", "time", "id", "dir", "dlc", "data");
    When(Method(crvMock, isColumnHidden)).Return(true);

    CRVGuiInterface& i = crvMock.get();

    i.setSorting(0, Qt::AscendingOrder);
    i.getClickedColumn(0);
    i.isColumnHidden(0);

    Verify(Method(crvMock, setSorting).Using(0, Qt::AscendingOrder));
    Verify(Method(crvMock, getClickedColumn).Using(0));
    Verify(Method(crvMock, getClickedColumn).Using(0));

    CanRawView canRawView{ CanRawViewCtx(&i) };
    REQUIRE_NOTHROW(canRawView.startSimulation());

    CanRawViewPrivate* d_ptr = canRawView.d_func();
    CHECK(d_ptr != nullptr);
}

TEST_CASE("Unique filter test", "[canrawview]")
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

    QStandardItemModel _tvModel;
    UniqueFilterModel _uniqueModel;
    QTableView _tableView;
    uint rowID = 0;

    _uniqueModel.setSourceModel(&_tvModel);
    _tableView.setModel(&_uniqueModel);

    // rowID, time, frameID, data//
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
    Mock<CRVGuiInterface> crvMock;
    Fake(Dtor(crvMock));

    Fake(Method(crvMock, setClearCbk));
    Fake(Method(crvMock, setDockUndockCbk));
    Fake(Method(crvMock, setSectionClikedCbk));
    Fake(Method(crvMock, setFilterCbk));
    Fake(Method(crvMock, setModel));
    Fake(Method(crvMock, setSorting));
    Fake(Method(crvMock, initTableView));

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

    CHECK(_tvModel.rowCount() == 4);
    CHECK(_uniqueModel.isFilterActive() == false);
    // TODO spy sectionClicked signal...
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
