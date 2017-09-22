#ifndef CRSGUI_H
#define CRSGUI_H

#include "crsguiinterface.h"
#include "ui_canrawsender.h"
#include <editdelegate.h>
#include <memory>

class SortModel;

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

    void setSectionClikedCbk(const sectionClicked_t& cb) override
    {
        QObject::connect(ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, cb);
    }

    QWidget* mainWidget() override
    {
        return widget;
    }

    void initTableView(QAbstractItemModel& _tvModel, CanRawSender* ptr) override
    {
        EditDelegate* delegate = new EditDelegate(&_tvModel, ptr, nullptr);

        ui->tv->setModel(&_tvModel);
        ui->tv->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tv->setItemDelegate(delegate);
        ui->tv->setEditTriggers(QAbstractItemView::DoubleClicked);
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

    void setModel(QAbstractItemModel* model) override
    {
        ui->tv->setModel(model);
    }

    Qt::SortOrder getSortOrder() override
    {
        return ui->tv->horizontalHeader()->sortIndicatorOrder();
    }

    QString getClickedColumn(int ndx) override
    {
        return ui->tv->model()->headerData(ndx, Qt::Horizontal).toString();
    }

    void setSorting(int sortNdx, Qt::SortOrder order) override
    {
        ui->tv->sortByColumn(sortNdx, order);
        ui->tv->horizontalHeader()->setSortIndicator(sortNdx, order);
    }

private:
    Ui::CanRawSenderPrivate* ui;
    QWidget* widget;
};
#endif // CRSGUI_H
