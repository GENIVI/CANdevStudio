#ifndef CRVGUI_H
#define CRVGUI_H

#include "crvguiinterface.h"
#include "ui_canrawview.h"
#include <memory>

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
        ui->tv->setColumnHidden(1, true);
        ui->tv->setColumnHidden(3, true);
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

    virtual int getSortSection() override
    {
        return ui->tv->horizontalHeader()->sortIndicatorSection();
    }

    virtual QString getClickedColumn(int ndx) override
    {
        return ui->tv->model()->headerData(ndx, Qt::Horizontal).toString();
    }

    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) override
    {
        ui->tv->sortByColumn(sortNdx, order);
        ui->tv->horizontalHeader()->setSortIndicator(clickedNdx, order);
    }

    virtual QString getWindowTitle() override
    {
        return widget->windowTitle();
    }

    virtual bool isColumnHidden(int ndx) override
    {
        return ui->tv->isColumnHidden(ndx);
    }

    virtual void getColumnProper(int ndx, int& vIdx, int& width) override
    {
        auto horHeader = ui->tv->horizontalHeader();
        vIdx = horHeader->visualIndex(ndx);
        width = ui->tv->columnWidth(ndx);
    }

    virtual void setColumnProper(int vIdxFrom, int vIdxTo, int width) override
    {
        auto horHeader = ui->tv->horizontalHeader();
        ui->tv->setColumnWidth(horHeader->logicalIndex(vIdxFrom), width);
        horHeader->moveSection(vIdxFrom, vIdxTo);
    }

private:
    Ui::CanRawViewPrivate* ui;
    QWidget* widget;
};

#endif // CRVGUI_H
