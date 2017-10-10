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
        , _columnsOrder({ "rowID", "time", "id", "dir", "dlc", "data" })
        , q_ptr(q)
    {
        initProps();

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
        QJsonArray viewModelsArray;

        writeColumnsOrder(json);
        json["scrolling"] = _ui.isViewFrozen();
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

        QList<QStandardItem*> list;

        QString frameID = QString("0x" + QString::number(frame.frameId(), 16));
        QString time = QString::number((_timer.elapsed() / 1000.0), 'f', 2);

        list.append(new QStandardItem(QString::number(_rowID++)));
        list.append(new QStandardItem(std::move(time)));
        list.append(new QStandardItem(std::move(frameID)));
        list.append(new QStandardItem(direction));
        list.append(new QStandardItem(QString::number(frame.payload().size())));
        list.append(new QStandardItem(QString::fromUtf8(payHex.data(), payHex.size())));

        _tvModel.appendRow(list);

        // Sort after reception of each frame and appending it to _tvModel and update filter
        _ui.setSorting(_sortIndex, _ui.getSortOrder());
        _uniqueModel.updateFilter(frameID, time, direction);

        if (!_ui.isViewFrozen()) {
            _ui.scrollToBottom();
        }
    }

    bool restoreConfiguration(const QJsonObject& json)
    {
        if (columnAdopt(json) == false) {
            return false;
        }
        if (scrollingAdopt(json) == false) {
            return false;
        }
        return true;
    }

    /// \brief Used to get list of properties supported by component
    /// @return supported properties
    ComponentInterface::ComponentProperties getSupportedProperties() const
    {
        return _supportedProps;
    }

private:
    void writeColumnsOrder(QJsonObject& json) const
    {
        int ii = 0;
        QJsonArray columnArray;
        for (const auto& column : _columnsOrder) {
            if (_ui.isColumnHidden(ii) == false) {
                QJsonObject columnProper;
                int vIdx;
                int width;
                _ui.getColumnProper(ii, vIdx, width);
                columnProper["name"] = column;
                columnProper["vIdx"] = vIdx;
                columnProper["width"] = width;
                columnArray.append(columnProper);
            }
            ++ii;
        }
        json["viewColumns"] = std::move(columnArray);
    }

    /* In the future for create log file below code can be use it.
     * Temporary is commented.
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
    */

    bool scrollingAdopt(QJsonObject const& json)
    {
        auto scrolling = json.find("scrolling");
        if (scrolling == json.end()) {
            cds_error("Scrolling item not found it");
            return false;
        }

        if (scrolling.value().type() != QJsonValue::Bool) {
            cds_error("Scrolling format is different then bool");
            return false;
        }
        auto frozen = json["scrolling"].toBool();
        if (_ui.isViewFrozen() != frozen) {
            _ui.setViewFrozen(frozen);
        }

        cds_info("Scrolling was restored correctly");
        return true;
    }

    bool columnAdopt(QJsonObject const& json)
    {
        auto columnIter = json.find("viewColumns");
        if (columnIter == json.end()) {
            cds_error("Columns item not found it");
            return false;
        }

        if (columnIter.value().type() != QJsonValue::Array) {
            cds_error("Columns format is different then array");
            return false;
        }

        auto colArray = json["viewColumns"].toArray();
        if (colArray.size() != 5) {
            cds_error("Columns array size must by 5 not {}", std::to_string(colArray.size()));
            return false;
        }

        // Structure declaration that will be used to grab information about column settings
        struct ref {
            int id;
            int vIdxConf;
            int widthConf;
            bool operator<(const ref& rhs) const
            {
                return vIdxConf < rhs.vIdxConf;
            }
        };
        std::vector<ref> refContener;

        auto ii = 0;
        for (const auto& column : _columnsOrder) {
            if (_ui.isColumnHidden(ii) == false) {
                auto jj = 0;
                for (; jj < colArray.size(); ++jj) {
                    // Check column name
                    //==================
                    if (colArray[jj].isObject() == false) {
                        cds_error("Columns description does not an object.");
                        return false;
                    }
                    auto colObj = colArray[jj].toObject();
                    if (colObj.contains("name") == false) {
                        cds_error("Columns description does not contain name field.");
                        return false;
                    }
                    if (colObj["name"].isString() == false) {
                        cds_error("name does not a String format.");
                        return false;
                    }
                    // auto nameConf = colObj["name"].toString();
                    if (colObj["name"].toString() != column) {
                        // The name does not pass - get next
                        continue;
                    }

                    // Check vIdx
                    //===========
                    if (colObj.contains("vIdx") == false) {
                        cds_error("Columns description does not contain vIdx field.");
                        return false;
                    }
                    if (colObj["vIdx"].isDouble() == false) {
                        cds_error("vIdx does not a Number format.");
                        return false;
                    }
                    auto vIdxConf = colObj["vIdx"].toInt();

                    // Check width
                    //============
                    if (colObj.contains("width") == false) {
                        cds_error("Columns description does not contain width field.");
                        return false;
                    }
                    if (colObj["width"].isDouble() == false) {
                        cds_error("width does not a Number format.");
                        return false;
                    }
                    auto widthConf = colObj["width"].toInt();
                    refContener.push_back({ ii, vIdxConf, widthConf });
                    break;
                }
                if (jj == colArray.size()) {
                    cds_error("Requred parameter in column description was not found.");
                    return false;
                }
            }
            ++ii;
        }
        std::sort(refContener.rbegin(), refContener.rend());
        for (const auto& pp : refContener) {
            int width;
            int vIdx;
            _ui.getColumnProper(pp.id, vIdx, width);
            if ((vIdx != pp.vIdxConf) || (width != pp.widthConf)) {
                _ui.setColumnProper(vIdx, pp.vIdxConf, pp.widthConf);
            }
        }
        cds_info("Column proporties ware restored correctly");

        return true;
    }

    void initProps()
    {
        for (const auto& p: _supportedProps)
        {
            _props[p.first];
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

    /**
    *   @brief  Function sets current sort settings and calls actual sort function
    *   @param  clickedIndex index of last clicked column
    */
    void sort(const int clickedIndex)
    {
        _currentSortOrder = _ui.getSortOrder();
        _sortIndex = clickedIndex;

        if (_prevIndex == clickedIndex) {
            if (_currentSortOrder == Qt::DescendingOrder) {
                _ui.setSorting(_sortIndex, Qt::DescendingOrder);
            } else {
                _ui.setSorting(0, Qt::AscendingOrder);
                _prevIndex = 0;
                _sortIndex = 0;
            }
        } else {
            _ui.setSorting(_sortIndex, Qt::AscendingOrder);
            _prevIndex = clickedIndex;
        }
    }

    /**
    *   @brief  Function call turns unique filter model on and off
    */
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
    std::map<QString, QVariant> _props;

private:
    int _rowID{ 0 };
    int _prevIndex{ 0 };
    int _sortIndex{ 0 };
    Qt::SortOrder _currentSortOrder{ Qt::AscendingOrder };
    QStringList _columnsOrder;
    CanRawView* q_ptr;
    ComponentInterface::ComponentProperties _supportedProps;
};
#endif // CANRAWVIEW_P_H
