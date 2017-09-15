#ifndef CRVGUI_H
#define CRVGUI_H

#include "crvguiinterface.h"
#include "ui_canrawview.h"
#include <iostream>
#include <memory>
enum class ColName { rowID = 0, time, id, dir, dlc, data };

struct DataTypes {
    enum class Type { uint = 0, hex, doubl, str };
};

Q_DECLARE_METATYPE(DataTypes::Type)

struct CRVGui : public CRVGuiInterface {

    CRVGui()
        : ui(new Ui::CanRawViewPrivate)
        , widget(new QWidget)
    {
        ui->setupUi(widget);
    }

    virtual void setClearCbk(const clear_t& cb) override
    {
        QObject::connect(ui->pbClear, &QPushButton::pressed, cb);
    }

    virtual void setDockUndockCbk(const dockUndock_t& cb) override
    {
        QObject::connect(ui->pbDockUndock, &QPushButton::toggled, cb);
    }

    virtual void setSectionClikedCbk(const sectionClicked_t& cb) override
    {
        QObject::connect(ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, cb);
    }

    virtual void setFilterCbk(const filter_t& cb) override
    {
        QObject::connect(ui->pbToggleFilter, &QPushButton::toggled, cb);
    }

    virtual QWidget* getMainWidget() override
    {
        return widget;
    }

    virtual void setModel(QAbstractItemModel* model) override
    {
        ui->tv->setModel(model);
    }

    virtual void initTableView(QAbstractItemModel& tvModel) override
    {
        ui->tv->setModel(&tvModel);
        ui->tv->horizontalHeader()->setSectionsMovable(true);
        ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
        ui->tv->setColumnHidden(0, true);

        tvModel.setHeaderData(
            static_cast<int>(ColName::rowID), Qt::Horizontal, QVariant::fromValue(DataTypes::Type::uint), Qt::UserRole);
        tvModel.setHeaderData(
            static_cast<int>(ColName::time), Qt::Horizontal, QVariant::fromValue(DataTypes::Type::doubl), Qt::UserRole);
        tvModel.setHeaderData(
            static_cast<int>(ColName::id), Qt::Horizontal, QVariant::fromValue(DataTypes::Type::hex), Qt::UserRole);
        tvModel.setHeaderData(
            static_cast<int>(ColName::dir), Qt::Horizontal, QVariant::fromValue(DataTypes::Type::str), Qt::UserRole);
        tvModel.setHeaderData(
            static_cast<int>(ColName::dlc), Qt::Horizontal, QVariant::fromValue(DataTypes::Type::uint), Qt::UserRole);
        tvModel.setHeaderData(
            static_cast<int>(ColName::data), Qt::Horizontal, QVariant::fromValue(DataTypes::Type::str), Qt::UserRole);
    }

    virtual bool isViewFrozen() override
    {
        return ui->freezeBox->isChecked();
    }

    virtual void scrollToBottom() override
    {
        ui->tv->scrollToBottom();
    }

    virtual Qt::SortOrder getSortOrder() override
    {
        return ui->tv->horizontalHeader()->sortIndicatorOrder();
    }

    virtual QString getClickedColumn(int ndx) override
    {
        return ui->tv->model()->headerData(ndx, Qt::Horizontal).toString();
    }

    virtual void setSorting(int sortNdx, Qt::SortOrder order) override
    {
        ui->tv->sortByColumn(sortNdx, order);
        ui->tv->horizontalHeader()->setSortIndicator(sortNdx, order);
    }

    virtual QString getWindowTitle() override
    {
        return widget->windowTitle();
    }

    virtual bool isColumnHidden(int ndx) override
    {
        return ui->tv->isColumnHidden(ndx);
    }

private:
    Ui::CanRawViewPrivate* ui;
    QWidget* widget;
};

#endif // CRVGUI_H
