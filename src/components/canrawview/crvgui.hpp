#ifndef CRVGUI_H
#define CRVGUI_H

#include "crvguiinterface.hpp"
#include "ui_canrawview.h"
#include <memory>

namespace Ui {
class CanRawViewPrivate;
}

struct CRVGui : public CRVGuiInterface {

    void setupUi(QWidget *widget)
    {
        ui->setupUi(widget);
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
    std::unique_ptr<Ui::CanRawViewPrivate> ui;
};

#endif // CRVGUI_H
