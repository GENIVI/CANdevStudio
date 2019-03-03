#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "crvsortmodel.h"
#include "gui/crvgui.h"
#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>
#include <log.h>
#include <memory>

namespace {
const int32_t rowCountMax = 2000;
}

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
        _tvModelUnique.setHorizontalHeaderLabels(_columnsOrder);

        _ui.initTableView(_tvModel);
        _tvModelSort.setSourceModel(&_tvModel);
        _tvModelUniqueSort.setSourceModel(&_tvModelUnique);
        _ui.setModel(&_tvModelSort);

        _ui.setClearCbk(std::bind(&CanRawViewPrivate::clear, this));
        _ui.setSectionClikedCbk(std::bind(&CanRawViewPrivate::sort, this, std::placeholders::_1));
        _ui.setFilterCbk(std::bind(&CanRawViewPrivate::setFilter, this, std::placeholders::_1));
        _ui.setDockUndockCbk([this] {
            docked = !docked;
            emit q_ptr->mainWidgetDockToggled(_ui.mainWidget());
        });

        _tvModel.setHeaderData(0, Qt::Horizontal, QVariant::fromValue(CRV_ColType::uint_type), Qt::UserRole); // rowID
        _tvModel.setHeaderData(1, Qt::Horizontal, QVariant::fromValue(CRV_ColType::double_type), Qt::UserRole); // time
        _tvModel.setHeaderData(2, Qt::Horizontal, QVariant::fromValue(CRV_ColType::hex_type), Qt::UserRole); // frame ID
        _tvModel.setHeaderData(
            3, Qt::Horizontal, QVariant::fromValue(CRV_ColType::str_type), Qt::UserRole); // direction
        _tvModel.setHeaderData(4, Qt::Horizontal, QVariant::fromValue(CRV_ColType::uint_type), Qt::UserRole); // length
        _tvModel.setHeaderData(5, Qt::Horizontal, QVariant::fromValue(CRV_ColType::str_type), Qt::UserRole); // data

        _tvModelUnique.setHeaderData(
            0, Qt::Horizontal, QVariant::fromValue(CRV_ColType::uint_type), Qt::UserRole); // rowID
        _tvModelUnique.setHeaderData(
            1, Qt::Horizontal, QVariant::fromValue(CRV_ColType::double_type), Qt::UserRole); // time
        _tvModelUnique.setHeaderData(
            2, Qt::Horizontal, QVariant::fromValue(CRV_ColType::hex_type), Qt::UserRole); // frame ID
        _tvModelUnique.setHeaderData(
            3, Qt::Horizontal, QVariant::fromValue(CRV_ColType::str_type), Qt::UserRole); // direction
        _tvModelUnique.setHeaderData(
            4, Qt::Horizontal, QVariant::fromValue(CRV_ColType::uint_type), Qt::UserRole); // length
        _tvModelUnique.setHeaderData(
            5, Qt::Horizontal, QVariant::fromValue(CRV_ColType::str_type), Qt::UserRole); // data
    }

    ~CanRawViewPrivate() {}

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

        int idPadding = 3;
        if (frame.hasExtendedFrameFormat()) {
            idPadding = 8;
        }

        QString frameID = QString("0x" + QString::number(frame.frameId(), 16).rightJustified(idPadding, '0'));
        QString time = QString::number((_timer.elapsed() / 1000.0), 'f', 2);
        QString size = QString::number(frame.payload().size());
        QString data = QString::fromUtf8(payHex.data(), payHex.size());

        QList<QStandardItem*> list;

        if (_tvModel.rowCount() < rowCountMax) {
            list.append(new QStandardItem(QString::number(_rowID)));
            list.append(new QStandardItem(time));
            list.append(new QStandardItem(frameID));
            list.append(new QStandardItem(direction));
            list.append(new QStandardItem(size));
            list.append(new QStandardItem(data));
        } else {
            list = _tvModel.takeRow(0);
            auto it = list.begin();

            (*it++)->setText(QString::number(_rowID));
            (*it++)->setText(time);
            (*it++)->setText(frameID);
            (*it++)->setText(direction);
            (*it++)->setText(size);
            (*it++)->setText(data);
        }

        _tvModel.appendRow(list);

        quint32 fId = frame.frameId();
        if (frame.hasExtendedFrameFormat()) {
            // Frame id has 29 bits. Set 31 bit if we are dealing with EFF.
            // This will separate frames in unique view
            fId |= 0x80000000;
        }

        if (direction == "RX") {
            if (_uniqueRxMap.count(fId)) {
                auto& row = _uniqueRxMap[fId];

                std::get<0>(row)->setText(QString::number(_rowID));
                std::get<1>(row)->setText(time);
                std::get<2>(row)->setText(frameID);
                std::get<3>(row)->setText(direction);
                std::get<4>(row)->setText(size);
                std::get<5>(row)->setText(data);
            } else {
                auto rowEl = new QStandardItem(QString::number(_rowID));
                auto timeEl = new QStandardItem(time);
                auto frameEl = new QStandardItem(frameID);
                auto dirEl = new QStandardItem(direction);
                auto sizeEl = new QStandardItem(size);
                auto dataEl = new QStandardItem(data);

                _tvModelUnique.appendRow({ rowEl, timeEl, frameEl, dirEl, sizeEl, dataEl });
                _uniqueRxMap[fId] = std::make_tuple(rowEl, timeEl, frameEl, dirEl, sizeEl, dataEl);
            }
        } else if (direction == "TX") {
            if (_uniqueTxMap.count(fId)) {
                auto& row = _uniqueTxMap[fId];

                std::get<0>(row)->setText(QString::number(_rowID));
                std::get<1>(row)->setText(time);
                std::get<2>(row)->setText(frameID);
                std::get<3>(row)->setText(direction);
                std::get<4>(row)->setText(size);
                std::get<5>(row)->setText(data);
            } else {
                auto rowEl = new QStandardItem(QString::number(_rowID));
                auto timeEl = new QStandardItem(time);
                auto frameEl = new QStandardItem(frameID);
                auto dirEl = new QStandardItem(direction);
                auto sizeEl = new QStandardItem(size);
                auto dataEl = new QStandardItem(data);

                _tvModelUnique.appendRow({ rowEl, timeEl, frameEl, dirEl, sizeEl, dataEl });
                _uniqueTxMap[fId] = std::make_tuple(rowEl, timeEl, frameEl, dirEl, sizeEl, dataEl);
            }
        } else {
            cds_warn("Invalid direction string: {}", direction.toStdString());
        }

        if (!_ui.isViewFrozen()) {
            _ui.scrollToBottom();
        }

        _rowID++;
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
                _ui.getColumnProper(ii, vIdx);
                columnProper["name"] = column;
                columnProper["vIdx"] = vIdx;
                columnArray.append(columnProper);
            }
            ++ii;
        }
        json["viewColumns"] = std::move(columnArray);
    }

    /* In the future to create log file below code can be used.
     * Temporarily commented.
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
            cds_error("Scrolling item not found");
            return false;
        }

        if (scrolling.value().type() != QJsonValue::Bool) {
            cds_error("Scrolling format is different than bool");
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
            cds_error("Columns item not found");
            return false;
        }

        if (columnIter.value().type() != QJsonValue::Array) {
            cds_error("Columns format is different than array");
            return false;
        }

        auto colArray = json["viewColumns"].toArray();
        if (colArray.size() != 5) {
            cds_error("Columns array size must be 5 not {}", std::to_string(colArray.size()));
            return false;
        }

        // Structure declaration that will be used to grab information about column settings
        struct ref {
            int id;
            int vIdxConf;
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
                        cds_error("Columns description is not an object.");
                        return false;
                    }
                    auto colObj = colArray[jj].toObject();
                    if (colObj.contains("name") == false) {
                        cds_error("Columns description does not contain name field.");
                        return false;
                    }
                    if (colObj["name"].isString() == false) {
                        cds_error("name is not a String format.");
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
                        cds_error("vIdx is not a Number format.");
                        return false;
                    }
                    auto vIdxConf = colObj["vIdx"].toInt();

                    refContener.push_back({ ii, vIdxConf });
                    break;
                }
                if (jj == colArray.size()) {
                    cds_error("Required parameter in column description was not found.");
                    return false;
                }
            }
            ++ii;
        }
        std::sort(refContener.rbegin(), refContener.rend());
        for (const auto& pp : refContener) {
            int vIdx;
            _ui.getColumnProper(pp.id, vIdx);
            if (vIdx != pp.vIdxConf) {
                _ui.setColumnProper(vIdx, pp.vIdxConf);
            }
        }
        cds_info("Column properties were restored correctly");

        return true;
    }

    void initProps()
    {
        for (const auto& p : _supportedProps) {
            _props[ComponentInterface::propertyName(p)];
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
        _tvModelUnique.removeRows(0, _tvModelUnique.rowCount());
        _uniqueRxMap.clear();
        _uniqueTxMap.clear();
        _rowID = 0;
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
    void setFilter(bool enabled)
    {
        _tvModelSort.setFilterActive(enabled);
        _tvModelUniqueSort.setFilterActive(enabled);

        if (enabled) {
            _ui.setModel(&_tvModelUniqueSort);
        } else {
            _ui.setModel(&_tvModelSort);
        }

        _currentSortOrder = _ui.getSortOrder();
        _ui.setSorting(_sortIndex, _currentSortOrder);
    }

public:
    CanRawViewCtx _ctx;
    QElapsedTimer _timer;
    QStandardItemModel _tvModel;
    QStandardItemModel _tvModelUnique;
    bool _simStarted;
    CRVGuiInterface& _ui;
    bool docked{ true };
    std::map<QString, QVariant> _props;
    CRVSortModel _tvModelSort;
    CRVSortModel _tvModelUniqueSort;

private:
    int _rowID{ 0 };
    int _prevIndex{ 0 };
    int _sortIndex{ 0 };
    Qt::SortOrder _currentSortOrder{ Qt::AscendingOrder };
    QStringList _columnsOrder;
    CanRawView* q_ptr;
    std::map<uint32_t,
        std::tuple<QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*>>
        _uniqueTxMap;
    std::map<uint32_t,
        std::tuple<QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*, QStandardItem*>>
        _uniqueRxMap;

    const QString _nameProperty = "name";

    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr))
    };
    // clang-format on
};
#endif // CANRAWVIEW_P_H
