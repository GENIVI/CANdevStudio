#ifndef CANRAWSENDER_P_H
#define CANRAWSENDER_P_H

#include "newlinemanager.h"
#include "ui_canrawsender.h"
#include <memory>
#include <QtGui/QStandardItemModel>
#include "newlinemanager.h"

namespace Ui {
class CanRawSenderPrivate;
}

class CanRawSenderPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawSender)

public:
    CanRawSenderPrivate(CanRawSender* q)
    : ui(std::make_unique<Ui::CanRawSenderPrivate>())
    , q_ptr(q)
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({ "Id", "Data", "Loop", "Interval", "" });
        ui->tv->setModel(&tvModel);
        ui->tv->setSelectionBehavior(QAbstractItemView::SelectRows);

        connect(ui->pbAdd, &QPushButton::pressed, this, &CanRawSenderPrivate::addNewItem);
        connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawSenderPrivate::dockUndock);
    }

    ~CanRawSenderPrivate() {}

    void SetSimulationState(bool state)
    {
        for (auto& iter : lines) {
            iter->SetSimulationState(state);
        }
    }

private:
    std::vector<std::unique_ptr<NewLineManager>> lines;
    std::unique_ptr<Ui::CanRawSenderPrivate> ui;
    QStandardItemModel tvModel;
    CanRawSender* q_ptr;

private slots:
    /**
     * @brief removeRowsSelectedByMouse
     *
     *  This function is used to remove selected (selected by mouse) rows.
     *
     */
    void removeRowsSelectedByMouse()
    {
        QModelIndexList IndexList = ui->tv->selectionModel()->selectedRows();
        std::list<QModelIndex> tmp = IndexList.toStdList();
        tmp.sort(); // List must to be sorted and reversed because erasing started from last row
        tmp.reverse();
        for (QModelIndex n : tmp) {
            tvModel.removeRow(n.row());
        }
    }

    void addNewItem()
    {
        QList<QStandardItem*> list{};
        tvModel.appendRow(list);
        auto newLine = std::make_unique<NewLineManager>(q_ptr);
        for (NewLineManager::ColName ii : NewLineManager::ColNameIterator()) {
            ui->tv->setIndexWidget(
                tvModel.index(tvModel.rowCount() - 1, static_cast<int>(ii)), newLine->GetColsWidget(ii));
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
