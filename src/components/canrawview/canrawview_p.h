#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "gui/crvgui.h"
#include "uniquefiltermodel.h"
#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>
#include <log.h>
#include <memory>

class CanRawViewPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

public:
    CanRawViewPrivate(CanRawView* q, CanRawViewCtx&& ctx = CanRawViewCtx(new CRVGui))
        : _ctx(std::move(ctx))
        , _simStarted(false)
        , _ui(_ctx.get<CRVGuiInterface>())
        , _columnsOrder({ "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" })
        , q_ptr(q)
    {
        _tvModel.setHorizontalHeaderLabels(_columnsOrder);

        _ui.initTableView(_tvModel);
        _uniqueModel.setSourceModel(&_tvModel);
        _ui.setModel(&_uniqueModel);

        _ui.setClearCbk(std::bind(&CanRawViewPrivate::clear, this));
        _ui.setSectionClikedCbk(std::bind(&CanRawViewPrivate::sort, this, std::placeholders::_1));
        _ui.setFilterCbk(std::bind(&CanRawViewPrivate::setFilter, this));
        _ui.setDockUndockCbk([this] {
            docked = !docked;
            emit q_ptr->mainWidgetDockToggled(_ui.mainWidget());
        });
    }

    ~CanRawViewPrivate()
    {
    }

    void saveSettings(QJsonObject& json)
    {
        QJsonObject jSortingObject;
        QJsonArray viewModelsArray;

        writeColumnsOrder(json);
        writeSortingRules(jSortingObject);
        json["sorting"] = std::move(jSortingObject);
        json["scrolling"] = _ui.isViewFrozen();
        writeViewModel(viewModelsArray);
        json["models"] = std::move(viewModelsArray);
    }

    void frameView(const QCanBusFrame& frame, const QString& direction)
    {
        if (!_simStarted) {
            cds_debug("send/received frame while simulation stopped");
            return;
        }

        auto payHex = frame.payload().toHex();
        // insert space between bytes, skip the end
        for (int ii = payHex.size() - 2; ii >= 2; ii -= 2) {
            payHex.insert(ii, ' ');
        }

        QList<QVariant> qvList;
        QList<QStandardItem*> list;

        int frameID = frame.frameId();
        double time = _timer.elapsed() / 1000.0;

        qvList.append(_rowID++);
        qvList.append(std::move(time));
        qvList.append(QString::number(time, 'f', 2));
        qvList.append(std::move(frameID));
        qvList.append(QString("0x" + QString::number(frameID, 16)));
        qvList.append(direction);
        qvList.append(QString::number(frame.payload().size()).toInt());
        qvList.append(QString::fromUtf8(payHex.data(), payHex.size()));

        for (QVariant qvitem : qvList) {
            QStandardItem* item = new QStandardItem();
            item->setData(qvitem, Qt::DisplayRole);
            list.append(item);
        }

        _tvModel.appendRow(list);

        // Sort after reception of each frame and appending it to _tvModel
        _currentSortOrder = _ui.getSortOrder();
        int currentSortIndicator = _ui.getSortSection();
        _ui.setSorting(_sortIndex, currentSortIndicator, _currentSortOrder);

        _uniqueModel.updateFilter(frameID, time, direction);

        if (!_ui.isViewFrozen()) {
            _ui.scrollToBottom();
        }
    }

private:
    void writeSortingRules(QJsonObject& json) const
    {
        json["prevIndex"] = _prevIndex;
        json["sortIndex"] = _sortIndex;
        json["currentSortOrder"] = _currentSortOrder;
    }

    void writeColumnsOrder(QJsonObject& json) const
    {
        int ii = 0;
        QJsonArray columnList;
        for (const auto& column : _columnsOrder) {
            if (_ui.isColumnHidden(ii) == false) {
                columnList.append(column);
            }
            ++ii;
        }
        json["columns"] = std::move(columnList);
    }

    void writeViewModel(QJsonArray& jsonArray) const
    {
        for (auto row = 0; row < _tvModel.rowCount(); ++row) {
            QJsonArray lineIter;
            for (auto column = 0; column < _tvModel.columnCount(); ++column) {
                if (_ui.isColumnHidden(column) == false) {
                    auto pp = _tvModel.data(_tvModel.index(row, column));
                    lineIter.append(std::move(pp.toString()));
                }
            }
            jsonArray.append(std::move(lineIter));
        }
    }

private slots:
    /**
     * @brief clear
     *
     * This function is used to clear data and filter models
     */
    void clear()
    {
        _tvModel.removeRows(0, _tvModel.rowCount());
        _uniqueModel.clearFilter();
    }

    void sort(const int clickedIndex)
    {
        _currentSortOrder = _ui.getSortOrder();
        _sortIndex = clickedIndex;
        QString clickedColumn = _ui.getClickedColumn(clickedIndex);

        if ((clickedColumn == "time") || (clickedColumn == "id")) {
            _sortIndex = _sortIndex - 1;
        }

        if (_prevIndex == clickedIndex) {
            if (_currentSortOrder == Qt::DescendingOrder) {
                _ui.setSorting(_sortIndex, clickedIndex, Qt::DescendingOrder);
            } else {
                _ui.setSorting(0, 0, Qt::AscendingOrder);
                _prevIndex = 0;
                _sortIndex = 0;
            }
        } else {
            _ui.setSorting(_sortIndex, clickedIndex, Qt::AscendingOrder);
            _prevIndex = clickedIndex;
        }
    }

    void setFilter()
    {
        _uniqueModel.toggleFilter();
    }

public:
    CanRawViewCtx _ctx;
    QElapsedTimer _timer;
    QStandardItemModel _tvModel;
    UniqueFilterModel _uniqueModel;
    bool _simStarted;
    CRVGuiInterface& _ui;
    bool docked{ true };

private:
    int _rowID{ 0 };
    int _prevIndex{ 0 };
    int _sortIndex{ 0 };
    Qt::SortOrder _currentSortOrder{ Qt::AscendingOrder };
    QStringList _columnsOrder;
    CanRawView* q_ptr;
};
#endif // CANRAWVIEW_P_H
