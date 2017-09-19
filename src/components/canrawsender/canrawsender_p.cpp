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
/*
    QJsonObject jSortingObject;
    QJsonArray lineArray;
    writeColumnsOrder(json);
    writeSortingRules(jSortingObject);
    json["sorting"] = std::move(jSortingObject);

    for (const auto& lineItem : _lines) {
        QJsonObject lineObject;
        lineItem->Line2Json(lineObject);QCheckBox
        lineArray.append(std::move(lineObject));
    }
    json["content"] = std::move(lineArray);
*/
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
        //_lines.erase(_lines.begin() + n.row()); // Delete lines also from collection
        // TODO: check if works when the collums was sorted before
    }
}

void CanRawSenderPrivate::addNewItem()
{
    static int rowID = 0;
    

    QList<QStandardItem*> list{};
    list.append(new QStandardItem(QString::number(rowID)));
    list.append(new QStandardItem(QString::number(0)));
    list.append(new QStandardItem(QString::number(0)));
    list.append(new QStandardItem(QString::number(0)));


    list.append(new QStandardItem(QString::number(0))); // checkbox
    list.append(new QStandardItem(QString::number(9))); // send button
    
    _tvModel.appendRow(list);

    int lastRowIndex = _tvModel.rowCount() - 1;
    QModelIndex index1 = _tvModel.index(lastRowIndex,4);
    QModelIndex index2 = _tvModel.index(lastRowIndex,5);
    _ui.setWidgetPersistent(index1);
    _ui.setWidgetPersistent(index2);
    rowID++;
}
