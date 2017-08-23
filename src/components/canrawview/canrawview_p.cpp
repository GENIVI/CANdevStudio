
#include "uibackend.hpp"
#include "canrawview_p.h"
#include "canrawview.h"
#include "canrawviewbackend.hpp"
#include "log.hpp"

#include <QtGui/QStandardItemModel>
#include <QtSerialBus/QCanBusFrame>

#include <cassert> // assert
#include <memory>  // unique_ptr
#include <utility> // move

const UIBackend<CanRawView>& CanRawViewPrivate::backend() const
{
    assert(nullptr != uiHandle);

    return *uiHandle;
}

UIBackend<CanRawView>& CanRawViewPrivate::backend()
{
    assert(nullptr != uiHandle);

    return *uiHandle;
}

void CanRawViewPrivate::init()
{
    tvModel.setHorizontalHeaderLabels({ "rowID", "timeDouble", "time", "idInt", "id", "dir", "dlc", "data" });

    backend().initTableView(tvModel);
    backend().setClearCbk([this] { this->clear(); });
    backend().setDockUndockCbk([this] { this->dockUndock(); });
    backend().setSectionClikedCbk([this](auto index) { this->sort(index); });
}

CanRawViewPrivate::CanRawViewPrivate(CanRawView* q)
    : timer{std::make_unique<QElapsedTimer>()}
    , simStarted{false}
    , q_ptr{q}
    , uiRep{std::make_unique<CanRawViewBackend>()} 
    , uiHandle{uiRep.get()}
{
    assert(nullptr != q_ptr);
    assert(nullptr != uiHandle);
    assert(nullptr != timer.get());

    init();
}

CanRawViewPrivate::CanRawViewPrivate(CanRawView* q, UIBackend<CanRawView>& backend)
    : timer{std::make_unique<QElapsedTimer>()}
    , simStarted{false}
    , q_ptr{q}
    , uiHandle{&backend}
{
    assert(nullptr != q_ptr);
    assert(nullptr != uiHandle);
    assert(nullptr != timer.get());

    init();
}

void CanRawViewPrivate::frameView(const QCanBusFrame& frame, const QString& direction)
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

    backend().updateScroll();
}

void CanRawViewPrivate::clear()
{
    tvModel.removeRows(0, tvModel.rowCount());
}

void CanRawViewPrivate::dockUndock()
{
    Q_Q(CanRawView);

    emit q->dockUndock();
}

void CanRawViewPrivate::sort(int clickedIndex)
{
    const int currentSortOrder = backend().getSortOrder();
    int sortIndex = clickedIndex;

    QString clickedColumn = backend().getClickedColumn(clickedIndex);

    if ((clickedColumn == "time") || (clickedColumn == "id")) {
        sortIndex = sortIndex - 1;
    }

    if (prevIndex == clickedIndex) {
        if (currentSortOrder == Qt::DescendingOrder) {
            backend().setSorting(sortIndex, clickedIndex, Qt::DescendingOrder);
            prevIndex = clickedIndex;
        } else {
            backend().setSorting(0, 0, Qt::AscendingOrder);
            prevIndex = 0;
        }
    } else {
        backend().setSorting(sortIndex, clickedIndex, Qt::AscendingOrder);
        prevIndex = clickedIndex;
    }
}

