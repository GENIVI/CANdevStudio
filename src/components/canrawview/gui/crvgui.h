#ifndef CRVGUI_H
#define CRVGUI_H

#include "crv_enums.h"
#include "crvguiinterface.h"
#include "ui_canrawview.h"
#include <QStandardItemModel>
#include <QWidget>
#include <log.h>
#include <memory>

struct CRVGui : public CRVGuiInterface {

    CRVGui()
        : ui(new Ui::CanRawViewPrivate)
        , widget(new QWidget)
    {
        ui->setupUi(widget);
        widget->setMinimumSize(ui->tv->minimumSize());
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

    virtual QWidget* mainWidget() override
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
        ui->tv->setColumnWidth(2, 85);
    }

    virtual bool isViewFrozen() override
    {
        return ui->freezeBox->isChecked();
    }

    virtual void setViewFrozen(bool state) override
    {
        if (ui->freezeBox->isChecked() != state) {
            ui->freezeBox->setChecked(state);
        }
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

    virtual void getColumnProper(int ndx, int& vIdx) override
    {
        auto horHeader = ui->tv->horizontalHeader();
        vIdx = horHeader->visualIndex(ndx);
    }

    virtual void setColumnProper(int vIdxFrom, int vIdxTo) override
    {
        auto horHeader = ui->tv->horizontalHeader();
        horHeader->moveSection(vIdxFrom, vIdxTo);
    }

private:
    Ui::CanRawViewPrivate* ui;
    QWidget* widget;
};

#endif // CRVGUI_H
