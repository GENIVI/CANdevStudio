#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include "crsfactory.hpp"
#include "crsgui.hpp"
#include "newlinemanager.h"
#include "ui_canrawsender.h"
#include <QtGui/QStandardItemModel>
#include <memory>

namespace Ui {
class CanRawSenderPrivate;
}

class CanRawSenderPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawSender)

public:
    CanRawSenderPrivate(CanRawSender* q)
        : CanRawSenderPrivate(q, mDefFactory)
    {
    }

    CanRawSenderPrivate(CanRawSender* q,  CRSFactoryInterface& factory)
        : mFactory(factory)
        , q_ptr(q)
        , simulationState(false)
    {
        tvModel.setHorizontalHeaderLabels({ "Id", "Data", "Loop", "Interval", "" });

        mUi.reset(mFactory.createGui());
        mUi->initTableView(tvModel);

        mUi->setAddCbk(std::bind(&CanRawSenderPrivate::addNewItem, this));
        mUi->setRemoveCbk(std::bind(&CanRawSenderPrivate::removeRowsSelectedByMouse, this));
        mUi->setDockUndockCbk(std::bind(&CanRawSenderPrivate::dockUndock, this));
    }

    ~CanRawSenderPrivate() {}

    void SetSimulationState(bool state)
    {
        simulationState = state;
        for (auto& iter : lines) {
            iter->SetSimulationState(state);
        }
    }

    CRSFactoryInterface& mFactory;
    std::unique_ptr<CRSGuiInterface> mUi;

private:
    std::vector<std::unique_ptr<NewLineManager>> lines;
    QStandardItemModel tvModel;
    CanRawSender* q_ptr;
    bool simulationState;
    CRSFactory mDefFactory;


private slots:
    /**
     * @brief removeRowsSelectedByMouse
     *
     *  This function is used to remove selected (selected by mouse) rows.
     *
     */
    void removeRowsSelectedByMouse()
    {

        QModelIndexList IndexList = mUi->getSelectedRows();
        std::list<QModelIndex> tmp = IndexList.toStdList();
        tmp.sort(); // List must to be sorted and reversed because erasing started from last row
        tmp.reverse();
        for (QModelIndex n : tmp) {
            tvModel.removeRow(n.row()); // Delete line from table view
            lines.erase(lines.begin() + n.row()); // Delete lines also from collection
            // TODO: check if works when the collums was sorted before
        }

    }

    void addNewItem()
    {

        QList<QStandardItem*> list{};
        tvModel.appendRow(list);
        auto newLine = std::make_unique<NewLineManager>(q_ptr, simulationState);
        for (NewLineManager::ColName ii : NewLineManager::ColNameIterator()) {
            mUi->setIndexWidget(tvModel.index(tvModel.rowCount() - 1, static_cast<int>(ii)), newLine->GetColsWidget(ii));
        }
        lines.push_back(std::move(newLine));

    }

    void dockUndock()
    {
        Q_Q(CanRawSender);
        emit q->dockUndock();
    }
};

#endif // CANRAWSENDER_P_H
