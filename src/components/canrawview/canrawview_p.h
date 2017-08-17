#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "crvfactory.hpp"
#include "crvgui.hpp"
#include "log.hpp"
#include "ui_canrawview.h"
#include <QHeaderView>
#include <QtCore/QElapsedTimer>
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>
#include <memory>

// class QElapsedTimer;

class CanRawViewPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

public:
    CanRawViewPrivate(CanRawView* q)
        : CanRawViewPrivate(q, mDefFactory)
    {
    }

    CanRawViewPrivate(CanRawView* q, CRVFactoryInterface& factory)
        : timer(std::make_unique<QElapsedTimer>())
        , simStarted(false)
        , mFactory(factory)
        , mUi(mFactory.createGui())
        , q_ptr(q)
    {
        tvModel.setHorizontalHeaderLabels({ "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" });

        // connect(ui->pbClear, &QPushButton::pressed, this, &CanRawViewPrivate::clear);
        // connect(ui->pbDockUndock, &QPushButton::pressed, this, &CanRawViewPrivate::dockUndock);

        // connect(
        // ui->tv->horizontalHeader(), &QHeaderView::sectionClicked, [=](int logicalIndex) { sort(logicalIndex); });
    }

    ~CanRawViewPrivate() {}

    void frameView(const QCanBusFrame& frame, const QString& direction)
    {
        // if (!simStarted) {
        // cds_debug("send/received frame while simulation stopped");
        // return;
        //}

        // auto payHex = frame.payload().toHex();
        // for (int ii = payHex.size(); ii >= 2; ii -= 2) {
        // payHex.insert(ii, ' ');
        //}

        // QList<QVariant> qvList;
        // QList<QStandardItem*> list;

        // qvList.append(rowID++);
        // qvList.append(QString::number((double)timer->elapsed() / 1000, 'f', 2).toDouble());
        // qvList.append(QString::number((double)timer->elapsed() / 1000, 'f', 2));
        // qvList.append(frame.frameId());
        // qvList.append(QString("0x" + QString::number(frame.frameId(), 16)));
        // qvList.append(direction);
        // qvList.append(QString::number(frame.payload().size()).toInt());
        // qvList.append(QString::fromUtf8(payHex.data(), payHex.size()));

        // for (QVariant qvitem : qvList) {
        // QStandardItem* item = new QStandardItem();
        // item->setData(qvitem, Qt::DisplayRole);
        // list.append(item);
        //}

        // tvModel.appendRow(list);

        // if (ui->freezeBox->isChecked() == false) {
        // ui->tv->scrollToBottom();
        //}
    }

    std::unique_ptr<QElapsedTimer> timer;
    QStandardItemModel tvModel;
    bool simStarted;
    CRVFactoryInterface& mFactory;
    std::unique_ptr<CRVGuiInterface> mUi;

private:
    CanRawView* q_ptr;
    int prevIndex = 0;
    int rowID = 0;
    CRVFactory mDefFactory;

private slots:
    /**
     * @brief clear
     *
     * This function is used to clear whole table
     */
    void clear() {
        tvModel.removeRows(0, tvModel.rowCount()); }

    void dockUndock()
    {
        Q_Q(CanRawView);
        emit q->dockUndock();
    }

    void sort(const int clickedIndex)
    {
        // int currentSortOrder = ui->tv->horizontalHeader()->sortIndicatorOrder();
        // int sortIndex = clickedIndex;

        // if ((ui->tv->model()->headerData(clickedIndex, Qt::Horizontal).toString() == "time")
        //|| (ui->tv->model()->headerData(clickedIndex, Qt::Horizontal).toString() == "id")) {
        // sortIndex = sortIndex - 1;
        //}

        // if (prevIndex == clickedIndex) {
        // if (currentSortOrder == Qt::DescendingOrder) {
        // ui->tv->sortByColumn(sortIndex, Qt::DescendingOrder);
        // ui->tv->horizontalHeader()->setSortIndicator(clickedIndex, Qt::DescendingOrder);
        // prevIndex = clickedIndex;
        //} else {
        // ui->tv->sortByColumn(0, Qt::AscendingOrder);
        // ui->tv->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);
        // prevIndex = 0;
        //}
        //} else {
        // ui->tv->sortByColumn(sortIndex, Qt::AscendingOrder);
        // ui->tv->horizontalHeader()->setSortIndicator(clickedIndex, Qt::AscendingOrder);
        // prevIndex = clickedIndex;
        //}
    }
};
#endif // CANRAWVIEW_P_H
