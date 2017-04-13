#include "canrawview.h"
#include "canrawview_p.h"

CanRawView::CanRawView(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CanRawViewPrivate())
{
    Q_D(CanRawView);

    d->setupUi();
    setLayout(d->layout.get());
}

CanRawView::~CanRawView()
{

}

void CanRawView::frameReceived(const QCanBusFrame &frame)
{

}

void CanRawView::frameSent(bool status, const QCanBusFrame &frame, const QVariant &context)
{

}
