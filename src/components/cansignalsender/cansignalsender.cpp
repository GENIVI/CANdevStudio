#include "cansignalsender.h"
#include "cansignalsender_p.h"

CanSignalSender::CanSignalSender(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanSignalSenderPrivate(this))
{
    Q_D(CanSignalSender);

    d->setupUi(this);
}

CanSignalSender::~CanSignalSender()
{
}

void CanSignalSender::start()
{
    Q_D(CanSignalSender);

    d->started = true;
}

void CanSignalSender::stop()
{
    Q_D(CanSignalSender);

    d->started = false;
}
