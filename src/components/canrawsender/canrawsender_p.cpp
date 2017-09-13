#include "canrawsender_p.h"
#include "canrawsender.h"
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
    json["columns"] = std::move(columnList);
}

void CanRawSenderPrivate::writeSortingRules(QJsonObject& json) const
{
    json["currentIndex"] = _currentIndex;
}

void CanRawSenderPrivate::removeRowsSelectedByMouse()
{
    QModelIndexList IndexList = _ui.getSelectedRows();
    std::list<QModelIndex> tmp = IndexList.toStdList();

    tmp.sort(); // List must to be sorted and reversed because erasing started from last row
    tmp.reverse();

    for (QModelIndex n : tmp) {
        _tvModel.removeRow(n.row()); // Delete line from table view
        _lines.erase(_lines.begin() + n.row()); // Delete lines also from collection
        // TODO: check if works when the collums was sorted before
    }
}

void CanRawSenderPrivate::addNewItem()
{
    QList<QStandardItem*> list{};
    _tvModel.appendRow(list);
    auto newLine = std::make_unique<NewLineManager>(q_ptr, _simulationState, _nlmFactory);
    for (NewLineManager::ColName ii : NewLineManager::ColNameIterator()) {
        _ui.setIndexWidget(_tvModel.index(_tvModel.rowCount() - 1, static_cast<int>(ii)), newLine->GetColsWidget(ii));
    }
    _lines.push_back(std::move(newLine));
}
