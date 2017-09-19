#ifndef CRSGUI_H
#define CRSGUI_H

#include "crsguiinterface.h"
#include "ui_canrawsender.h"
#include <memory>
#include <editdelegate.h>

namespace Ui {
class CanRawSenderPrivate;
}

struct CRSGui : public CRSGuiInterface {
    CRSGui()
        : ui(new Ui::CanRawSenderPrivate)
        , widget(new QWidget)
    {
        ui->setupUi(widget);
    }

    void setAddCbk(const add_t& cb) override
    {
        QObject::connect(ui->pbAdd, &QPushButton::pressed, cb);
    }

    void setRemoveCbk(const remove_t& cb) override
    {
        QObject::connect(ui->pbRemove, &QPushButton::pressed, cb);
    }

    void setDockUndockCbk(const dockUndock_t& cb) override
    {
        QObject::connect(ui->pbDockUndock, &QPushButton::toggled, cb);
    }

    QWidget* getMainWidget() override
    {
        return widget;
    }

    void initTableView(QAbstractItemModel& _tvModel) override
    {
        EditDelegate* delegate = new EditDelegate;

        ui->tv->setModel(&_tvModel);
        ui->tv->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tv->setItemDelegate(delegate);
        ui->tv->setEditTriggers(QAbstractItemView::AllEditTriggers);
        ui->tv->setItemDelegateForColumn(4, delegate);
    }

    QModelIndexList getSelectedRows() override
    {
        return ui->tv->selectionModel()->selectedRows();
    }

    void setIndexWidget(const QModelIndex& index, QWidget* widget) override
    {
        ui->tv->setIndexWidget(index, widget);
    }
    
    void setWidgetPersistent(const QModelIndex& index) override
    {
        ui->tv->openPersistentEditor(index);
    }

private:
    Ui::CanRawSenderPrivate* ui;
    QWidget* widget;
};
#endif // CRSGUI_H
