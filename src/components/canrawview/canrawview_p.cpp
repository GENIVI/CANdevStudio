
#include "canrawview.h"
#include "canrawview_p.h"
#include "canrawviewbackend.hpp"
#include "log.hpp"  // cds_debug

#include <QHeaderView>
#include <QStandardItem>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/Qt> // DisplayRole, SortOrder
#include <QtCore/QVariant>
#include <QtSerialBus/QCanBusFrame>

#include <cassert>  // assert
#include <utility>  // move


void CanRawViewPrivate::saveSettings(QJsonObject& json)
{
    auto& ui = backend();

    auto title = ui.getWindowTitle().toStdString();
    assert(0 != title.length());

    json[title.c_str()] = makeColumnsOrder();
    json["Sorting"]     = makeSortingRules();
    json["Models"]      = makeViewModel();
    json["Scrolling"]   = ui.isViewFrozen() ? 1 : 0;
}

void CanRawViewPrivate::frameView(const QCanBusFrame& frame, const QString& direction)
{
    if ( ! _simStarted)
    {
        cds_debug("send/received frame while simulation stopped");
        return;
    }

    auto payHex = frame.payload().toHex();
    // insert space between bytes, skip the end
    for (int ii = payHex.size() - 2; ii >= 2; ii -= 2) {
        payHex.insert(ii, ' ');
    }

    const auto frameID = frame.frameId();
    const auto elapsedMs = static_cast<double>(_timer->elapsed()) / 1000.0;
    auto elapsed = QString::number(elapsedMs, 'f', 2);

    QList<QVariant> qvList;
    qvList.append(_rowID++);
    qvList.append(elapsed.toDouble());  // TODO: or just pass elapsedMs?
    qvList.append(std::move(elapsed));
    qvList.append(frameId);
    qvList.append(QString("0x" + QString::number(frame.frameId(), 16)));
    qvList.append(direction);
    qvList.append(QString::number(frame.payload().size()).toInt());
    qvList.append(QString::fromUtf8(payHex.data(), payHex.size()));

    QList<QStandardItem*> list;

    for (QVariant qvitem : qvList) {
        QStandardItem* item = new QStandardItem;
        assert(nullptr != item);

        item->setData(qvitem, Qt::DisplayRole);
        list.append(item);
    }

    _tvModel.appendRow(std::move(list));

    auto& ui = backend();

    _currentSortOrder = ui.getSortOrder();

    ui.setSorting(_sortIndex, ui.getSortSection(), _currentSortOrder);

    _uniqueModel.updateFilter(frameID, elapsedMs, direction);

    if ( ! ui.isViewFrozen()) { ui.scrollToBottom(); }
}

QJsonObject CanRawViewPrivate::makeSortingRules() const
{
    return { {"prevIndex", _prevIndex}
           , {"sortIndex", _sortIndex}
           , {"currentSortOrder", _currentSortOrder} };
}

QJsonObject CanRawViewPrivate::makeColumnsOrder() const
{
    auto& ui = backend();

    int ii = 0;
    QJsonArray columnList;
    for (const auto& column : _columnsOrder) {
        if (ui.isColumnHidden(ii) == false) {
            columnList.append(column);
        }
        ++ii;
    }

    return {{"Columns", std::move(columnList)}};
}

QJsonArray CanRawViewPrivate::makeViewModel() const
{
    auto& ui = backend();

    QJsonArray items;

    for (auto row = 0; row < _tvModel.rowCount(); ++row) {
        QJsonArray line;

        for (auto col = 0; col < _tvModel.columnCount(); ++col) {
            if (ui.isColumnHidden(col) == false) {
                line.append(_tvModel.data(_tvModel.index(row, col)).toString());
            }
        }

        items.append(std::move(line));
    }

    return items;
}

void CanRawViewPrivate::clear()
{
    _tvModel.removeRows(0, _tvModel.rowCount());
    _uniqueModel.clearFilter();
}

void CanRawViewPrivate::dockUndock()
{
    Q_Q(CanRawView);

    emit q->dockUndock();
}

void CanRawViewPrivate::sort(int index)
{
    auto& ui = backend();

    _currentSortOrder = ui.getSortOrder();
    _sortIndex = index;

    const auto clickedCol =ui.getClickedColumn(index);

    if (("time" == clickedCol) || ("id" == clickedCol))
    {
        _sortIndex = _sortIndex - 1;  // FIXME: below 0 acceptable?
    }

    if (_prevIndex == index) {
        if (_currentSortOrder == Qt::DescendingOrder) {
            ui.setSorting(_sortIndex, index, Qt::DescendingOrder);
        } else {
            ui.setSorting(0, 0, Qt::AscendingOrder);
            _prevIndex = 0;
            _sortIndex = 0;
        }
    } else {
        ui.setSorting(_sortIndex, index, Qt::AscendingOrder);
        _prevIndex = index;
    }
}

void CanRawViewPrivate::setFilter()
{
    _uniqueModel.toggleFilter();
}


