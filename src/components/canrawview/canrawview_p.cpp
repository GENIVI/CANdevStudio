
#include "canrawview_p.h"
#include "log.hpp"  // cds_debug

#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QtSerialBus/QCanBusFrame>


void CanRawViewPrivate::saveSettings(QJsonObject& json)
{
    Q_Q(CanRawView);

    assert(ui->freezeBox != nullptr);
    assert(q->windowTitle().toStdString().length() != 0);

    // FIXME: change write* function to make*: void(T&) => T()

    QJsonObject jObjects;
    writeColumnsOrder(jObjects);
    json[q->windowTitle().toStdString().c_str()] = std::move(jObjects); ////////////////////////////

    QJsonObject jSortingObject;
    writeSortingRules(jSortingObject);
    jObjects["Sorting"] = std::move(jSortingObject);

    QJsonArray viewModelsArray;
    writeViewModel(viewModelsArray);
    jObjects["Models"] = std::move(viewModelsArray);

    jObjects["Scrolling"] = (ui->freezeBox->isChecked() == true) ? 1 : 0;  ///////////////////////
}

void CanRawViewPrivate::frameView(const QCanBusFrame& frame, const QString& direction)
{
    if ( ! _simStarted)
    {
        cds_debug("send/received frame while simulation stopped");
        return;
    }

    auto payHex = frame.payload().toHex();
    // inster space between bytes, skip the end
    for (int ii = payHex.size() - 2; ii >= 2; ii -= 2) {
        payHex.insert(ii, ' ');
    }

    QList<QVariant> qvList;

    qvList.append(_rowID++);
    qvList.append(QString::number((double)_timer->elapsed() / 1000, 'f', 2).toDouble());
    qvList.append(QString::number((double)_timer->elapsed() / 1000, 'f', 2));
    qvList.append(frame.frameId());
    qvList.append(QString("0x" + QString::number(frame.frameId(), 16)));
    qvList.append(direction);
    qvList.append(QString::number(frame.payload().size()).toInt());
    qvList.append(QString::fromUtf8(payHex.data(), payHex.size()));

    QList<QStandardItem*> list;

    for (QVariant qvitem : qvList) {
        QStandardItem* item = new QStandardItem();
        item->setData(qvitem, Qt::DisplayRole);
        list.append(item);
    }

    _tvModel.appendRow(list);

    auto& ui = backend();
    _currentSortOrder = ui.getSortOrder();

/*  -- goes to CanRawViewBackend
    auto currentSortIndicator = ui->tv->horizontalHeader()->sortIndicatorSection();
*/
    ui.setSorting(sortIndex, ui.getSortIndicator(), _currentSortOrder);
    ui.updateScroll();
}

void CanRawViewPrivate::writeSortingRules(QJsonObject& json) const
{
    json["prevIndex"] = _prevIndex;
    json["sortIndex"] = _sortIndex;
    json["currentSortOrder"] = _currentSortOrder;
}

void CanRawViewPrivate::writeColumnsOrder(QJsonObject& json) const
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
    json["Columns"] = std::move(columnList);
}

void CanRawViewPrivate::writeViewModel(QJsonArray& jsonArray) const
{
    auto& ui = backend();

    for (auto row = 0; row < _tvModel.rowCount(); ++row) {
        QJsonArray lineIter;
        for (auto column = 0; column < _tvModel.columnCount(); ++column) {
            if (ui.isColumnHidden(column) == false) {
                auto pp = _tvModel.data(_tvModel.index(row, column));
                lineIter.append(std::move(pp.toString()));
            }
        }
        jsonArray.append(std::move(lineIter));
    }
}

void CanRawViewPrivate::clear()
{
    _tvModel.removeRows(0, _tvModel.rowCount());
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

    if (("time" == ui.getClickedColumn(index)) || ("id" == ui.getClickedColumn(index)))
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

