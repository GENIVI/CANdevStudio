#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "ui_canrawview.h"
#include <QtGui/QStandardItemModel>
#include <memory>

namespace Ui {
class CanRawViewPrivate;
}

class CanRawViewPrivate : public QWidget {
    Q_OBJECT

public:
    CanRawViewPrivate()
    : ui(std::make_unique<Ui::CanRawViewPrivate>())
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({"time", "id", "dir", "dlc", "data"});
        ui->tv->setModel(&tvModel);
        ui->tv->setColumnWidth(0, 36);
        ui->tv->setColumnWidth(1, 92);
        ui->tv->setColumnWidth(2, 27);
        ui->tv->setColumnWidth(3, 25);
        ui->tv->setColumnWidth(4, 178);

        ui->tv->setSortingEnabled(true);
        ui->tv->horizontalHeader()->setSectionsMovable(true);

        connect(ui->pbClear, &QPushButton::pressed, this, &CanRawViewPrivate::clear);
    }

    ~CanRawViewPrivate()
    {
    }

    std::unique_ptr<Ui::CanRawViewPrivate> ui;
    QStandardItemModel tvModel;

private slots:
    void clear()
    {
    }
};
#endif // CANRAWVIEW_P_H
