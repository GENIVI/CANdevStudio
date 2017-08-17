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

    QWidget* getMainWidget() override
    {
        return widget;
    }

    void initTableView(QAbstractItemModel& tvModel) override 
    {
        ui->tv->setModel(&tvModel);
        ui->tv->horizontalHeader()->setSectionsMovable(true);
        ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
        ui->tv->setColumnHidden(0, true);
        ui->tv->setColumnHidden(1, true);
        ui->tv->setColumnHidden(3, true);
    }

private:
    Ui::CanRawViewPrivate *ui;
    QWidget *widget;
};

#endif // CRVGUI_H
