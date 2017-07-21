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
    Q_DECLARE_PUBLIC(CanRawView)

public:
    CanRawViewPrivate(CanRawView* q)
    : ui(std::make_unique<Ui::CanRawViewPrivate>())
    , q_ptr(q)
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

	connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawViewPrivate::dockUndock);
    }

    ~CanRawViewPrivate()
    {
    }

    std::unique_ptr<Ui::CanRawViewPrivate> ui;
    QStandardItemModel tvModel;

private:
    CanRawView* q_ptr;

private slots:
    void clear()
    {
    }

    void dockUndock()
    {
        Q_Q(CanRawView);
        emit q->dockUndock();
    }
};
#endif // CANRAWVIEW_P_H
