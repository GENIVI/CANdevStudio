#ifndef CRVGUI_H
#define CRVGUI_H

#include "crvguiinterface.hpp"
#include "ui_canrawview.h"
#include <memory>

namespace Ui {
class CanRawViewPrivate;
}

struct CRVGui : public CRVGuiInterface {

    CRVGui()
        : ui(new Ui::CanRawViewPrivate)
        , widget(new QWidget)
    {
        ui->setupUi(widget);
    }

    virtual void setClearCbk(const clear_t& cb) override { QObject::connect(ui->pbClear, &QPushButton::pressed, cb); }

    virtual void setDockUndockCbk(const dockUndock_t& cb) override
    {
        QObject::connect(ui->pbDockUndock, &QPushButton::pressed, cb);
    }

    virtual void setSectionClikedCbk(const sectionClicked_t& cb) override
    {

        QObject::connect(ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, cb);
    }

    virtual QWidget* getMainWidget() override { return widget; }

    virtual void initTableView(QAbstractItemModel& tvModel) override
    {
        ui->tv->setModel(&tvModel);
        ui->tv->horizontalHeader()->setSectionsMovable(true);
        ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
        ui->tv->setColumnHidden(0, true);
        ui->tv->setColumnHidden(1, true);
        ui->tv->setColumnHidden(3, true);
    }

    virtual void updateScroll() override
    {
        if (ui->freezeBox->isChecked() == false) {
            ui->tv->scrollToBottom();
        }
    }

    virtual int getSortOrder() override { return ui->tv->horizontalHeader()->sortIndicatorOrder(); }

    virtual QString getClickedColumn(int ndx) override
    {
        return ui->tv->model()->headerData(ndx, Qt::Horizontal).toString();
    }

    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) override
    {
        ui->tv->sortByColumn(sortNdx, order);
        ui->tv->horizontalHeader()->setSortIndicator(clickedNdx, order);
    }

private:
    Ui::CanRawViewPrivate* ui;
    QWidget* widget;
};

#endif // CRVGUI_H
