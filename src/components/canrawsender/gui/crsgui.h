#ifndef CRSGUI_H
#define CRSGUI_H

#include "crs_enums.h"
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

        ui->tv->horizontalHeader()->setSectionsMovable(true);
        ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
        ui->tv->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tv->setColumnHidden(0, true);

        _tvModel.setHeaderData(0, Qt::Horizontal, QVariant::fromValue(CRS_ColType::uint_type), Qt::UserRole); // rowID
        _tvModel.setHeaderData(
            1, Qt::Horizontal, QVariant::fromValue(CRS_ColType::uint_type), Qt::UserRole); // frame ID
        _tvModel.setHeaderData(2, Qt::Horizontal, QVariant::fromValue(CRS_ColType::hex_type), Qt::UserRole); // data
        _tvModel.setHeaderData(
            3, Qt::Horizontal, QVariant::fromValue(CRS_ColType::uint_type), Qt::UserRole); // interval
        _tvModel.setHeaderData(
            4, Qt::Horizontal, QVariant::fromValue(CRS_ColType::bool_type), Qt::UserRole); // checkbox
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

    void setModel(QAbstractItemModel* _tvModel) override
    {
        ui->tv->setModel(_tvModel);
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
