#ifndef CANRAWVIEW_P_H
#define CANRAWVIEW_P_H

#include "canrawviewbackend.hpp"
#include "log.hpp"
#include "ui_canrawview.h"
#include "uibackend.hpp"

#include <QHeaderView>
#include <QtCore/QElapsedTimer>
#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>

#include <cassert> // assert
#include <memory>  // unique_ptr
#include <utility> // move


class CanRawView;

class CanRawViewPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CanRawView)

public:

    CanRawViewPrivate(CanRawView* q)
        : CanRawViewPrivate{q, std::make_unique<CanRawViewBackend>()}
    {
        assert(nullptr != q);
        assert(nullptr != backend);
    }

    CanRawViewPrivate(CanRawView* q, std::unique_ptr<UIBackend<CanRawView>> backend)
        : timer{std::make_unique<QElapsedTimer>()}
        , backend{std::move(backend)}
        , q_ptr{q}
        , simStarted{false}
    {
        assert(nullptr != q);
        assert(nullptr != backend);

        tvModel.setHorizontalHeaderLabels({ "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" });

        backend->initTableView(tvModel);
        backend->setClearCbk([this] { clear(); });
        backend->setDockUndockCbk([this] { dockUndock(); });
        backend->setSectionClikedCbk([this](auto index) { sort(index); });
    }

    ~CanRawViewPrivate() = default;

    void frameView(const QCanBusFrame& frame, const QString& direction)
    {
        if ( ! simStarted)
        {
            cds_debug("send/received frame while simulation stopped");
            return;
        }

        auto payHex = frame.payload().toHex();
        for (int ii = payHex.size(); ii >= 2; ii -= 2) {
            payHex.insert(ii, ' ');
        }

        QList<QVariant> qvList;
        QList<QStandardItem*> list;

        qvList.append(rowID++);
        qvList.append(QString::number((double)timer->elapsed() / 1000, 'f', 2).toDouble());
        qvList.append(QString::number((double)timer->elapsed() / 1000, 'f', 2));
        qvList.append(frame.frameId());
        qvList.append(QString("0x" + QString::number(frame.frameId(), 16)));
        qvList.append(direction);
        qvList.append(QString::number(frame.payload().size()).toInt());
        qvList.append(QString::fromUtf8(payHex.data(), payHex.size()));

        for (QVariant qvitem : qvList) {
            QStandardItem* item = new QStandardItem();
            item->setData(qvitem, Qt::DisplayRole);
            list.append(item);
        }

        tvModel.appendRow(list);

        backend->updateScroll();
    }

    std::unique_ptr<QElapsedTimer> timer;
    std::unique_ptr<UIBackend<CanRawView>> backend;
    bool simStarted = false;
    QStandardItemModel tvModel;

private:

    CanRawView* q_ptr = nullptr;
    int prevIndex = 0;
    int rowID = 0;

private slots:

    /** Clears whole table. */
    void clear() { tvModel.removeRows(0, tvModel.rowCount()); }

    void dockUndock()
    {
        Q_Q(CanRawView);

        emit q->dockUndock();
    }

    void sort(int clickedIndex)
    {
        const int currentSortOrder = backend->getSortOrder();
        int sortIndex = clickedIndex;

        QString clickedColumn = backend->getClickedColumn(clickedIndex);

        if ((clickedColumn == "time") || (clickedColumn == "id")) {
            sortIndex = sortIndex - 1;
        }

        if (prevIndex == clickedIndex) {
            if (currentSortOrder == Qt::DescendingOrder) {
                backend->setSorting(sortIndex, clickedIndex, Qt::DescendingOrder);
                prevIndex = clickedIndex;
            } else {
                backend->setSorting(0, 0, Qt::AscendingOrder);
                prevIndex = 0;
            }
        } else {
            backend->setSorting(sortIndex, clickedIndex, Qt::AscendingOrder);
            prevIndex = clickedIndex;
        }
    }
};
#endif // CANRAWVIEW_P_H
