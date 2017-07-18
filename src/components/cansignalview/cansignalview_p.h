#ifndef CANSIGNALVIEW_P_H
#define CANSIGNALVIEW_P_H

#include "ui_cansignalview.h"
#include <QtGui/QStandardItemModel>
#include <memory>

namespace Ui {
class CanSignalViewPrivate;
}

class CanSignalViewPrivate : public QWidget {
    Q_OBJECT

public:
    CanSignalViewPrivate()
    : ui(std::make_unique<Ui::CanSignalViewPrivate>())
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({ tr("time"), tr("name"), tr("value") });
        ui->tv->setModel(&tvModel);
        ui->tv->verticalHeader()->hideSection(0);
        ui->tv->setColumnWidth(0, 36);
        ui->tv->setColumnWidth(1, 170);
        ui->tv->setColumnWidth(2, 92);

        connect(ui->pbClear, &QPushButton::pressed, this, &CanSignalViewPrivate::clear);
    }

    ~CanSignalViewPrivate()
    {
    }

    std::unique_ptr<Ui::CanSignalViewPrivate> ui;
    QStandardItemModel tvModel;

private slots:
    void clear()
    {
    }
};

#endif // CANSIGNALVIEW_P_H
