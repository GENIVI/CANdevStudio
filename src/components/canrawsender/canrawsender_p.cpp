#include "canrawsender_p.h"
#include "canrawsender.h"
#include <QJsonArray>

void CanRawSenderPrivate::setSimulationState(bool state)
{
    simulationState = state;
    for (auto& iter : lines) {
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

    for (const auto& lineItem : lines) {
        QJsonObject lineObject;
        lineItem->Line2Json(lineObject);
        lineArray.append(std::move(lineObject));
    }
    json["content"] = std::move(lineArray);
}

int CanRawSenderPrivate::getLineCount() const
{
    return lines.size();
}

void CanRawSenderPrivate::writeColumnsOrder(QJsonObject& json) const
{
    QJsonArray columnList;
    for (const auto& column : columnsOrder) {
        columnList.append(column);
    }
    json["columns"] = std::move(columnList);
}

void CanRawSenderPrivate::writeSortingRules(QJsonObject& json) const
{
    json["currentIndex"] = currentIndex;
}

void CanRawSenderPrivate::removeRowsSelectedByMouse()
{
    QModelIndexList IndexList = mUi.getSelectedRows();
    std::list<QModelIndex> tmp = IndexList.toStdList();

    tmp.sort(); // List must to be sorted and reversed because erasing started from last row
    tmp.reverse();

    for (QModelIndex n : tmp) {
        tvModel.removeRow(n.row()); // Delete line from table view
        lines.erase(lines.begin() + n.row()); // Delete lines also from collection
        // TODO: check if works when the collums was sorted before
    }
}

void CanRawSenderPrivate::addNewItem()
{
    QList<QStandardItem*> list{};
    tvModel.appendRow(list);
    auto newLine = std::make_unique<NewLineManager>(canRawSender, simulationState, nlmFactory);
    for (NewLineManager::ColName ii : NewLineManager::ColNameIterator()) {
        mUi.setIndexWidget(tvModel.index(tvModel.rowCount() - 1, static_cast<int>(ii)), newLine->GetColsWidget(ii));
    }
    lines.push_back(std::move(newLine));
}

void CanRawSenderPrivate::dockUndock()
{
    emit canRawSender->dockUndock();
}
