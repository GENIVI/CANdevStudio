#include "canrawsender.h"
#include "canrawsender_p.h"

CanRawSender::CanRawSender(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanRawSenderPrivate(this))
{
    Q_D(CanRawSender);

    setLayout(d->ui->layout);
}

CanRawSender::~CanRawSender()
{
}
