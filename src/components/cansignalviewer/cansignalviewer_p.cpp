#include "cansignalviewer_p.h"
#include "sortenums.h"
#include <log.h>
#include <datamodeltypes/datadirection.h>

namespace {
const int32_t rowCountMax = 2000;
}

CanSignalViewerPrivate::CanSignalViewerPrivate(CanSignalViewer* q, CanSignalViewerCtx&& ctx)
    : _ctx(std::move(ctx))
    , _ui(_ctx.get<CanSignalViewerGuiInt>())
    , _columnsOrder({ "rowID", "time", "dir", "id", "signal", "value" })
    , q_ptr(q)
{
    initProps();

    _tvModel.setHorizontalHeaderLabels(_columnsOrder);
    _tvModelUnique.setHorizontalHeaderLabels(_columnsOrder);

    _ui.initTableView(_tvModel);
    _tvModelSort.setSourceModel(&_tvModel);
    _tvModelUniqueSort.setSourceModel(&_tvModelUnique);
    _ui.setModel(&_tvModelSort);

    _ui.setClearCbk(std::bind(&CanSignalViewerPrivate::clear, this));
    _ui.setSectionClikedCbk(std::bind(&CanSignalViewerPrivate::sort, this, std::placeholders::_1));
    _ui.setFilterCbk(std::bind(&CanSignalViewerPrivate::setFilter, this, std::placeholders::_1));
    _ui.setDockUndockCbk([this] {
        _docked = !_docked;
        emit q_ptr->mainWidgetDockToggled(_ui.mainWidget());
    });

    _tvModel.setHeaderData(0, Qt::Horizontal, QVariant::fromValue(ColType::uint_type), Qt::UserRole); // rowID
    _tvModel.setHeaderData(1, Qt::Horizontal, QVariant::fromValue(ColType::double_type), Qt::UserRole); // time
    _tvModel.setHeaderData(2, Qt::Horizontal, QVariant::fromValue(ColType::str_type), Qt::UserRole); // dir
    _tvModel.setHeaderData(3, Qt::Horizontal, QVariant::fromValue(ColType::hex_type), Qt::UserRole); // frame ID
    _tvModel.setHeaderData(4, Qt::Horizontal, QVariant::fromValue(ColType::str_type), Qt::UserRole); // signal
    _tvModel.setHeaderData(5, Qt::Horizontal, QVariant::fromValue(ColType::double_type), Qt::UserRole); // value

    _tvModelUnique.setHeaderData(0, Qt::Horizontal, QVariant::fromValue(ColType::uint_type), Qt::UserRole); // rowID
    _tvModelUnique.setHeaderData(1, Qt::Horizontal, QVariant::fromValue(ColType::double_type), Qt::UserRole); // time
    _tvModelUnique.setHeaderData(2, Qt::Horizontal, QVariant::fromValue(ColType::str_type), Qt::UserRole); // dir
    _tvModelUnique.setHeaderData(3, Qt::Horizontal, QVariant::fromValue(ColType::hex_type), Qt::UserRole); // frame ID
    _tvModelUnique.setHeaderData(4, Qt::Horizontal, QVariant::fromValue(ColType::str_type), Qt::UserRole); // signal
    _tvModelUnique.setHeaderData(5, Qt::Horizontal, QVariant::fromValue(ColType::double_type), Qt::UserRole); // value
}

void CanSignalViewerPrivate::initProps()
{
    for (const auto& p : _supportedProps) {
        _props[ComponentInterface::propertyName(p)];
    }
}

ComponentInterface::ComponentProperties CanSignalViewerPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

QJsonObject CanSignalViewerPrivate::getSettings()
{
    QJsonObject json;

    for (const auto& p : _props) {
        json[p.first] = QJsonValue::fromVariant(p.second);
    }

    return json;
}

void CanSignalViewerPrivate::setSettings(const QJsonObject& json)
{
    for (const auto& p : _supportedProps) {
        const QString& propName = ComponentInterface::propertyName(p);
        if (json.contains(propName))
            _props[propName] = json[propName].toVariant();
    }
}

void CanSignalViewerPrivate::clear()
{
    _tvModel.removeRows(0, _tvModel.rowCount());
    _tvModelUnique.removeRows(0, _tvModelUnique.rowCount());
    _uniqueRxMap.clear();
    _uniqueTxMap.clear();
    _rowID = 0;
}

void CanSignalViewerPrivate::sort(const int clickedIndex)
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

void CanSignalViewerPrivate::setFilter(bool enabled)
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

void CanSignalViewerPrivate::addSignal(const QString& name, const QVariant& val, const Direction& dir)
{
    auto nameSplit = name.split('_');

    if (nameSplit.size() < 2) {
        cds_error("Wrong signal name: {}", name.toStdString());
        return;
    }

    bool idConv = false;
    uint32_t id = nameSplit[0].toUInt(&idConv, 16);
    if (!idConv) {
        cds_error("Failed to convert singal id ({})", nameSplit[0].toStdString());
        return;
    }

    int idPadding = 3;
    if (id > 0x7ff) {
        idPadding = 8;
    }

    QString frameID = QString("0x" + QString::number(id, 16).rightJustified(idPadding, '0'));
    QString time = QString::number((_timer.elapsed() / 1000.0), 'f', 2);
    QString sigName = name.mid(name.indexOf("_") + 1);
    QString direction;
    QString value = val.toString();

    if (dir == Direction::TX) {
        direction = "TX";
    } else if (dir == Direction::RX) {
        direction = "RX";
    } else {
        cds_error("Undefined direction of Signal");
        return;
    }

    QList<QStandardItem*> list;

    if (_tvModel.rowCount() < rowCountMax) {
        list.append(new QStandardItem(QString::number(_rowID)));
        list.append(new QStandardItem(time));
        list.append(new QStandardItem(direction));
        list.append(new QStandardItem(frameID));
        list.append(new QStandardItem(sigName));
        list.append(new QStandardItem(value));
    } else {
        list = _tvModel.takeRow(0);
        auto it = list.begin();

        (*it++)->setText(QString::number(_rowID));
        (*it++)->setText(time);
        (*it++)->setText(direction);
        (*it++)->setText(frameID);
        (*it++)->setText(sigName);
        (*it++)->setText(value);
    }

    _tvModel.appendRow(list);

    // DBC requires that all messages with extended frame format
    // has 29th, most signigicant bit set. There is no possibility
    // That frames with id > 0x7ff will be sent in EEF.

    if (dir == Direction::RX) {
        if (_uniqueRxMap.count(name)) {
            auto& row = _uniqueRxMap[name];

            std::get<0>(row)->setText(QString::number(_rowID));
            std::get<1>(row)->setText(time);
            std::get<2>(row)->setText(direction);
            std::get<3>(row)->setText(frameID);
            std::get<4>(row)->setText(sigName);
            std::get<5>(row)->setText(value);
        } else {
            auto rowEl = new QStandardItem(QString::number(_rowID));
            auto timeEl = new QStandardItem(time);
            auto dirEl = new QStandardItem(direction);
            auto frameEl = new QStandardItem(frameID);
            auto sigNameEl = new QStandardItem(sigName);
            auto valueEl = new QStandardItem(value);

            _tvModelUnique.appendRow({ rowEl, timeEl, dirEl, frameEl, sigNameEl, valueEl });
            _uniqueRxMap[name] = std::make_tuple(rowEl, timeEl, dirEl, frameEl, sigNameEl, valueEl);
        }
    } else if (dir == Direction::TX) {
        if (_uniqueTxMap.count(name)) {
            auto& row = _uniqueTxMap[name];

            std::get<0>(row)->setText(QString::number(_rowID));
            std::get<1>(row)->setText(time);
            std::get<2>(row)->setText(direction);
            std::get<3>(row)->setText(frameID);
            std::get<4>(row)->setText(sigName);
            std::get<5>(row)->setText(value);
        } else {
            auto rowEl = new QStandardItem(QString::number(_rowID));
            auto timeEl = new QStandardItem(time);
            auto dirEl = new QStandardItem(direction);
            auto frameEl = new QStandardItem(frameID);
            auto sigNameEl = new QStandardItem(sigName);
            auto valueEl = new QStandardItem(value);

            _tvModelUnique.appendRow({ rowEl, timeEl, dirEl, frameEl, sigNameEl, valueEl });
            _uniqueTxMap[name] = std::make_tuple(rowEl, timeEl, dirEl, frameEl, sigNameEl, valueEl);
        }
    } else {
        cds_warn("Invalid direction string: {}", direction.toStdString());
    }

    if (!_ui.isViewFrozen()) {
        _ui.scrollToBottom();
    }

    _rowID++;
}

