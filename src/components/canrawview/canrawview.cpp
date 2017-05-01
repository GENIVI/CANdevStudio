#include "canrawview.h"
#include "canrawview_p.h"
#include <QCanBusFrame>
#include <QList>
#include <QStandardItem>
#include <QStringList>

CanRawView::CanRawView(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanRawViewPrivate())
{
    Q_D(CanRawView);

    d->setupUi(this);
}

CanRawView::~CanRawView()
{
}

void CanRawView::frameReceived(const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    d->instertRow("RX", frame.frameId(), frame.payload().size(), frame.payload().toHex());
}

void CanRawView::frameSent(bool status, const QCanBusFrame& frame, const QVariant&)
{
    Q_D(CanRawView);

    if (status) {
        d->instertRow("TX", frame.frameId(), frame.payload().size(), frame.payload().toHex());
    }
}

void CanRawView::clear()
{
    Q_D(CanRawView);

    d->clear();
}

void CanRawView::start()
{
    Q_D(CanRawView);

    d->startTimer();
}
