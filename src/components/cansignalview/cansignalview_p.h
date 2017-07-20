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
    Q_DECLARE_PUBLIC(CanSignalView);

public:
    CanSignalViewPrivate(CanSignalView* q)
    : ui(std::make_unique<Ui::CanSignalViewPrivate>())
    , q_ptr(q)
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({ tr("time"), tr("name"), tr("value") });
        ui->tv->setModel(&tvModel);
        ui->tv->verticalHeader()->hideSection(0);
        ui->tv->setColumnWidth(0, 36);
        ui->tv->setColumnWidth(1, 170);
        ui->tv->setColumnWidth(2, 92);

        connect(ui->pbClear, &QPushButton::pressed, this, &CanSignalViewPrivate::clear);

        connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanSignalViewPrivate::dockUndock);

    }

    ~CanSignalViewPrivate()
    {
    }

    std::unique_ptr<Ui::CanSignalViewPrivate> ui;
    QStandardItemModel tvModel;

private:
    CanSignalView* q_ptr;

private slots:
    void clear()
    {
    }

    void dockUndock()
    {
        Q_Q(CanSignalView);
        emit q->dockUndock();
    }
};

#endif // CANSIGNALVIEW_P_H
