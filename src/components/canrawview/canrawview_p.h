#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "ui_canrawview.h"
#include <QtGui/QStandardItemModel>

namespace Ui {
class CanRawViewPrivate;
}

class CanRawViewPrivate : public QWidget {
    Q_OBJECT

public:
    CanRawViewPrivate()
    : ui(new Ui::CanRawViewPrivate)
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({"time", "id", "dir", "dlc", "data"});
        ui->tv->setModel(&tvModel);
        ui->tv->setColumnWidth(0, 36);
        ui->tv->setColumnWidth(1, 92);
        ui->tv->setColumnWidth(2, 27);
        ui->tv->setColumnWidth(3, 25);
        ui->tv->setColumnWidth(4, 178);


        connect(ui->pbClear, &QPushButton::pressed, this, &CanRawViewPrivate::clear);
    }

    ~CanRawViewPrivate()
    {
        delete ui;
    }

    Ui::CanRawViewPrivate *ui;
    QStandardItemModel tvModel;

private slots:
    void clear()
    {
    }
};
#endif // CANRAWVIEW_P_H
