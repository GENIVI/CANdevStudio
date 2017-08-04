#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "log.hpp"
#include "ui_canrawview.h"
#include <QtCore/QElapsedTimer>
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>
#include <memory>
#include <iostream>
#include <QHeaderView>

namespace Ui {
class CanRawViewPrivate;
}

class QElapsedTimer;

class CanRawViewPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

public:
    CanRawViewPrivate(CanRawView* q)
        : ui(std::make_unique<Ui::CanRawViewPrivate>())
        , timer(std::make_unique<QElapsedTimer>())
        , simStarted(false)
        , q_ptr(q)
    {
        ui->setupUi(this);

        tvModel.setHorizontalHeaderLabels({ "rowID", "time", "id", "dir", "dlc", "data" });
        ui->tv->setModel(&tvModel);
        ui->tv->horizontalHeader()->setSectionsMovable(true);
        //ui->tv->setColumnHidden(0, true);

        connect(ui->pbClear, &QPushButton::pressed, this, &CanRawViewPrivate::clear);
        connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawViewPrivate::dockUndock);

        connect(ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, [=]( const int &logicalIndex ) { sort( logicalIndex, clickedIndexes ); });
        connect(&tvModel, &QAbstractItemModel::rowsInserted, [=]() { update( clickedIndexes ); });

    }

    ~CanRawViewPrivate() {}

    void frameView(const QCanBusFrame& frame, const QString& direction)
    {
        if (!simStarted) {
            cds_debug("send/received frame while simulation stopped");
            return;
        }

        auto payHex = frame.payload().toHex();
        for (int ii = payHex.size(); ii >= 2; ii -= 2) {
            payHex.insert(ii, ' ');
        }
        
        static int rowID = 0;
        QList<QStandardItem*> list;
        list.append(new QStandardItem(QString::number(rowID++)));
        list.append(new QStandardItem(QString::number((double)timer->elapsed() / 1000, 'f', 2)));
        list.append(new QStandardItem("0x" + QString::number(frame.frameId(), 16)));
        list.append(new QStandardItem(direction));
        list.append(new QStandardItem(QString::number(frame.payload().size())));
        list.append(new QStandardItem(QString::fromUtf8(payHex.data(), payHex.size())));

        tvModel.appendRow(list);

        if (ui->freezeBox->isChecked() == false) {
            ui->tv->scrollToBottom();
        }
    }

    std::unique_ptr<Ui::CanRawViewPrivate> ui;
    std::unique_ptr<QElapsedTimer> timer;
    QStandardItemModel tvModel;
    bool simStarted;
    QList<int> clickedIndexes = {0,0,0};

private:
    CanRawView* q_ptr;

private slots:
    void clear() {}

    void dockUndock()
    {
        Q_Q(CanRawView);
        emit q->dockUndock();
    }

    void update(QList<int> &clickedIndexes)
    {
    }

    void sort(const int currentIndex, QList<int> &clickedIndexes)
    {
        clickedIndexes.removeFirst();
        clickedIndexes.append(currentIndex);

        if ((clickedIndexes[2] == clickedIndexes[1]) && (clickedIndexes[1] == clickedIndexes[0]))
        {
            ui->tv->sortByColumn(0,Qt::AscendingOrder);
            clickedIndexes = {0,0,0};
        }
        else if (clickedIndexes[2] == clickedIndexes[1])
        {
            ui->tv->sortByColumn(currentIndex,Qt::DescendingOrder);
            ui->tv->horizontalHeader()->setSortIndicator(currentIndex,Qt::DescendingOrder);
        }
        else
        {
            ui->tv->sortByColumn(currentIndex,Qt::AscendingOrder);
            ui->tv->horizontalHeader()->setSortIndicator(currentIndex,Qt::AscendingOrder);
        }
    }
};
#endif // CANRAWVIEW_P_H
