#include "canrawsender_p.h"
#include "canrawsender.h"
#include "log.h"
#include <QJsonArray>

void CanRawSenderPrivate::setSimulationState(bool state)
{
    _simulationState = state;
    for (auto& iter : _lines) {
        iter->SetSimulationState(state);
    }
}

void CanRawSenderPrivate::saveSettings(QJsonObject& json) const
{
    QJsonObject jSortingObject;
    QJsonArray lineArray;
    writeColumnsOrder(json);
    writeSortingRules(jSortingObject);
    json["sorting"] = std::move(jSortingObject);

    for (const auto& lineItem : _lines) {
        QJsonObject lineObject;
        lineItem->Line2Json(lineObject);
        lineArray.append(std::move(lineObject));
    }

    json["content"] = std::move(lineArray);
}

int CanRawSenderPrivate::getLineCount() const
{
    return _lines.size();
}

void CanRawSenderPrivate::writeColumnsOrder(QJsonObject& json) const
{
    QJsonArray columnList;
    for (const auto& column : _columnsOrder) {
        columnList.append(column);
    }
    json["senderColumns"] = std::move(columnList);
}

void CanRawSenderPrivate::writeSortingRules(QJsonObject& json) const
{
    json["currentIndex"] = _currentIndex;
}

void CanRawSenderPrivate::removeRowsSelectedByMouse()
{
    QModelIndexList IndexList = _ui.getSelectedRows();
    std::list<QModelIndex> tmp{IndexList.begin(), IndexList.end()};

    tmp.sort(); // List must to be sorted and reversed because erasing started from last row
    tmp.reverse();

    for (QModelIndex n : tmp) {
        _tvModel.removeRow(n.row()); // Delete line from table view
        _lines.erase(_lines.begin() + n.row()); // Delete lines also from collection
        // TODO: check if works when the collums was sorted before
    }
}

ComponentInterface::ComponentProperties CanRawSenderPrivate::getSupportedProperties() const
{
    return _supportedProps;
}

void CanRawSenderPrivate::addNewItem()
{
    QList<QStandardItem*> list{};
    _tvModel.appendRow(list);
    auto newLine = std::make_unique<NewLineManager>(q_ptr, _simulationState, _nlmFactory);

    using It = NewLineManager::ColNameIterator;

    for (NewLineManager::ColName ii : It{ NewLineManager::ColName::IdLine }) {
        _ui.setIndexWidget(
            _tvModel.index(_tvModel.rowCount() - 1, static_cast<int>(ii)), newLine->GetColsWidget(It{ ii }));
    }
    _lines.push_back(std::move(newLine));
}

bool CanRawSenderPrivate::columnAdopt(QJsonObject const& json)
{
    auto columnIter = json.find("senderColumns");
    if (columnIter == json.end()) {
        cds_error("Columns item not found");
        return false;
    }

    if (columnIter.value().type() != QJsonValue::Array) {
        cds_error("Columns format is different than array");
        return false;
    }

    auto colArray = json["senderColumns"].toArray();
    if (colArray.size() < 5) {
        cds_error("Columns array size is {} - must be at least 5!", std::to_string(colArray.size()));
        return false;
    }
    if (colArray.contains("Id") == false) {
        cds_error("Columns array does not contain Id field.");
        return false;
    }
    if (colArray.contains("Data") == false) {
        cds_error("Columns array does not contain Data field.");
        return false;
    }
    if (colArray.contains("Remote") == false) {
        // Backward compability.
        cds_warn("Columns array does not contain Remote field.");
    }
    if (colArray.contains("Loop") == false) {
        cds_error("Columns array does not contain Loop field.");
        return false;
    }
    if (colArray.contains("Interval") == false) {
        cds_error("Columns array does not contain Interval field.");
        return false;
    }

    cds_info("Columns validation is finished successfully.");
    return true;
}

bool CanRawSenderPrivate::sortingAdopt(QJsonObject const& json)
{
    auto sortingIter = json.find("sorting");
    if (sortingIter == json.end()) {
        cds_error("Sorting item not found");
        return false;
    }
    if (sortingIter.value().type() != QJsonValue::Object) {
        cds_error("Sorting format is different than object");
        return false;
    }

    auto sortingObj = json["sorting"].toObject();
    if (sortingObj.count() != 1) {
        cds_error("Sorting object count {} is different than 1.", std::to_string(sortingObj.count()));
        return false;
    }
    if (sortingObj.contains("currentIndex") == false) {
        cds_error("Sorting object does not contain currentIndex.");
        return false;
    }
    if (sortingObj["currentIndex"].isDouble() == false) {
        cds_error("currentIndex format in sorting object is incorrect.");
        return false;
    }

    cds_info("Sorting validation is finished successfully.");

    auto newIdx = sortingObj["currentIndex"].toInt();
    if ((newIdx < 0) || (newIdx > 5)) {
        cds_error("currentIndex data '{}' is out of range.", std::to_string(newIdx));
        return false;
    }

    _currentIndex = newIdx;
    cds_debug("currentIndex data is adopted new value = {}.", std::to_string(_currentIndex));

    return true;
}

bool CanRawSenderPrivate::contentAdopt(QJsonObject const& json)
{
    QString data, id, interval;
    bool remote = false;
    bool loop = false;
    bool send = false;

    auto contentIter = json.find("content");
    if (contentIter == json.end()) {
        cds_error("Content item not found");
        return false;
    }
    if (contentIter.value().type() != QJsonValue::Array) {
        cds_error("Content format is different than array");
        return false;
    }

    // Lambda expression to restore string field
    auto restoreStringField = [] (const QJsonObject& obj, const QString& fieldName, QString& fieldValue) -> bool {
        if (obj.contains(fieldName)) {
            if (obj[fieldName].isString() == true) {
                fieldValue = obj[fieldName].toString();
            } else {
                cds_error("{} field does not contain a string format.", fieldName.toStdString());
                return false;
            }
        } else {
            cds_info("{} is not available.", fieldName.toStdString());
        }
        return true;
    };

    // Lambda expression to restore bool field
    auto restoreBoolField = [] (const QJsonObject& obj, const QString& fieldName, bool& fieldValue) -> bool {
        if (obj.contains(fieldName)) {
            if (obj[fieldName].isBool() == true) {
                fieldValue = obj[fieldName].toBool();
            } else {
                cds_error("{} field does not contain a bool format.", fieldName.toStdString());
                return false;
            }
        } else {
            cds_info("{} is not available.", fieldName.toStdString());
        }
        return true;
    };

    auto contentArray = json["content"].toArray();
    for (auto ii = 0; ii < contentArray.count(); ++ii) {
        auto line = contentArray[ii].toObject();

        data.clear();        
        id.clear();        
        interval.clear();
        remote = false;
        loop = false;

        if (!restoreStringField(line, "data", data))
            return false;
        if (!restoreStringField(line, "id", id))
            return false;
        if (!restoreStringField(line, "interval", interval))
            return false;
        if (!restoreBoolField(line, "remote", remote))
            return false;
        if (!restoreBoolField(line, "loop", loop))
            return false;
        if (!restoreBoolField(line, "send", send))
            return false;

        // Add new lines with dependencies
        addNewItem();
        if (_lines.back()->RestoreLine(id, data, remote, interval, loop, send) == false) {
            _tvModel.removeRow(_lines.size() - 1); // Delete line from table view
            _lines.erase(_lines.end() - 1); // Delete lines also from collection
            cds_warn("Problem with a validation of line occurred.");
        } else {
            cds_info("New line was adopted correctly.");
        }
    }

    return true;
}

bool CanRawSenderPrivate::restoreConfiguration(const QJsonObject& json)
{
    if (columnAdopt(json) == false) {
        return false;
    }
    if (contentAdopt(json) == false) {
        return false;
    }
    if (sortingAdopt(json) == false) {
        return false;
    }

    return true;
}
