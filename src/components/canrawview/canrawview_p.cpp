
#include "uibackend.hpp"
#include "canrawview_p.h"
#include "canrawview.h"
#include "canrawviewbackend.hpp"
#include "log.hpp"

#include <QDebug>
//#include <QFile>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>

#include <cassert> // assert
#include <memory>  // unique_ptr
#include <utility> // move

const UIBackend<CanRawView>& CanRawViewPrivate::backend() const
{
    assert(nullptr != uiHandle);

    return *uiHandle;
}

UIBackend<CanRawView>& CanRawViewPrivate::backend()
{
    assert(nullptr != uiHandle);

    return *uiHandle;
}

void CanRawViewPrivate::init()
{
    backend().setupUi(this);

    tvModel.setHorizontalHeaderLabels(columnsOrder);
    backend().tv->setModel(&tvModel);
    backend().tv->horizontalHeader()->setSectionsMovable(true);
    backend().tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
    backend().tv->setColumnHidden(0, true);
    backend().tv->setColumnHidden(1, true);
    backend().tv->setColumnHidden(3, true);

    connect(ui->pbClear, &QPushButton::pressed, this, &CanRawViewPrivate::clear);
    connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawViewPrivate::dockUndock);

    connect(backend().tv->horizontalHeader(), &QHeaderView::sectionClicked
          , [=](int logicalIndex) { sort(logicalIndex); });
}

CanRawViewPrivate::CanRawViewPrivate(CanRawView* q)
    : timer{std::make_unique<QElapsedTimer>()}
    , simStarted{false}
    , q_ptr{q}
    , uiRep{std::make_unique<CanRawViewBackend>()} 
    , uiHandle{uiRep.get()}
    , columnsOrder({ "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" })
{
    assert(nullptr != q_ptr);
    assert(nullptr != uiHandle);
    assert(nullptr != timer.get());

    init();
}

CanRawViewPrivate::CanRawViewPrivate(CanRawView* q, UIBackend<CanRawView>& backend)
    : timer{std::make_unique<QElapsedTimer>()}
    , simStarted{false}
    , q_ptr{q}
    , uiHandle{&backend}
    , columnsOrder({ "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" })
{
    assert(nullptr != q_ptr);
    assert(nullptr != uiHandle);
    assert(nullptr != timer.get());

    init();
}

void CanRawViewPrivate::frameView(const QCanBusFrame& frame, const QString& direction)
{
    if ( ! simStarted)
    {
        cds_debug("send/received frame while simulation stopped");
        return;
    }

    auto payHex = frame.payload().toHex();
    for (int ii = payHex.size(); ii >= 2; ii -= 2) {
        payHex.insert(ii, ' ');
    }

    QList<QVariant> qvList;
    QList<QStandardItem*> list;

    qvList.append(rowID++);
    qvList.append(QString::number((double)timer->elapsed() / 1000, 'f', 2).toDouble());
    qvList.append(QString::number((double)timer->elapsed() / 1000, 'f', 2));
    qvList.append(frame.frameId());
    qvList.append(QString("0x" + QString::number(frame.frameId(), 16)));
    qvList.append(direction);
    qvList.append(QString::number(frame.payload().size()).toInt());
    qvList.append(QString::fromUtf8(payHex.data(), payHex.size()));

    for (QVariant qvitem : qvList) {
        QStandardItem* item = new QStandardItem();
        item->setData(qvitem, Qt::DisplayRole);
        list.append(item);
    }

    tvModel.appendRow(list);

    currentSortOrder = backend().tv->horizontalHeader()->sortIndicatorOrder();
    auto currentSortIndicator = backend().tv->horizontalHeader()->sortIndicatorSection();
    backend().tv->sortByColumn(sortIndex, currentSortOrder);
    backend().tv->horizontalHeader()->setSortIndicator(currentSortIndicator, currentSortOrder);

    if (backend().freezeBox->isChecked() == false) {
        backend().tv->scrollToBottom();
    }
}

void CanRawViewPrivate::clear()
{
    tvModel.removeRows(0, tvModel.rowCount());
}

void CanRawViewPrivate::dockUndock()
{
    Q_Q(CanRawView);

    emit q->dockUndock();
}

void CanRawViewPrivate::sort(int clickedIndex)
{
    currentSortOrder = backend().tv->horizontalHeader()->sortIndicatorOrder();
    sortIndex = clickedIndex;

    if ((backend().tv->model()->headerData(clickedIndex, Qt::Horizontal).toString() == "time")
        || (backend().tv->model()->headerData(clickedIndex, Qt::Horizontal).toString() == "id"))
    {
        sortIndex = sortIndex - 1;
    }

    if (prevIndex == clickedIndex) {
        if (currentSortOrder == Qt::DescendingOrder) {
            backend().tv->sortByColumn(sortIndex, Qt::DescendingOrder);
            backend().tv->horizontalHeader()->setSortIndicator(clickedIndex, Qt::DescendingOrder);
        } else {
            backend().tv->sortByColumn(0, Qt::AscendingOrder);
            backend().tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
            prevIndex = 0;
            sortIndex = 0;
        }
    } else {
        backend().tv->sortByColumn(sortIndex, Qt::AscendingOrder);
        backend().tv->horizontalHeader()->setSortIndicator(clickedIndex, Qt::AscendingOrder);
        prevIndex = clickedIndex;
    }
}

void CanRawViewPrivate::writeSortingRules(QJsonObject& json) const
{
    json["prevIndex"] = prevIndex;
    json["sortIndex"] = sortIndex;
    json["currentSortOrder"] = currentSortOrder;
}

void CanRawViewPrivate::writeColumnsOrder(QJsonObject& json) const
{
    assert(backend().tv != nullptr);

    int ii = 0;
    QJsonArray columnList;
    for (const auto& column : columnsOrder) {
        if (backend().tv->isColumnHidden(ii) == false) {
            columnList.append(column);
        }
        ++ii;
    }
    json["Columns"] = std::move(columnList);
}

void CanRawViewPrivate::writeViewModel(QJsonArray& jsonArray) const
{
    assert(backend().tv != nullptr);

    for (auto row = 0; row < tvModel.rowCount(); ++row) {
        QJsonArray lineIter;
        for (auto column = 0; column < tvModel.columnCount(); ++column) {
            if (backend().tv->isColumnHidden(column) == false) {
                auto pp = tvModel.data(tvModel.index(row, column));
                lineIter.append(std::move(pp.toString()));
            }
        }
        jsonArray.append(std::move(lineIter));
    }
}

void CanRawViewPrivate::saveSettings(QJsonObject& json)
{
    QJsonObject jObjects;
    QJsonObject jSortingObject;
    QJsonArray viewModelsArray;
    /*
     * Temporary below code comments use for test during debug and do possibility to write settings to file
     *
    QString fileName("viewSave.cds");
    QFile saveFile(fileName);

    if (saveFile.open(QIODevice::WriteOnly) == false) {
        cds_debug("Problem with open the file {0} to write confiruration", fileName.toStdString());
        return;
    }
    */

    assert(backend().freezeBox != nullptr);
    assert(q_ptr != nullptr);
    assert(q_ptr->windowTitle().toStdString().length() != 0);

    writeColumnsOrder(jObjects);
    writeSortingRules(jSortingObject);
    jObjects["Sorting"] = std::move(jSortingObject);
    jObjects["Scrolling"] = (backend().freezeBox->isChecked() == true) ? 1 : 0;
    writeViewModel(viewModelsArray);
    jObjects["Models"] = std::move(viewModelsArray);
    json[q_ptr->windowTitle().toStdString().c_str()] = std::move(jObjects);
    /*
    QJsonDocument saveDoc(json);
    saveFile.write(saveDoc.toJson());
    */
}
