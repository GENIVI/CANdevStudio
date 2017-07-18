#include "cansignalsender.h"
#include "cansignalsender_p.h"

CanSignalSender::CanSignalSender(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanSignalSenderPrivate(this))
{
    Q_D(CanSignalSender);

    setLayout(d->ui->layout);
}

CanSignalSender::~CanSignalSender()
{
}
