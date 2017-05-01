#include "canrawsender.h"
#include "canrawsender_p.h"
#include <memory>

CanRawSender::CanRawSender(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanRawSenderPrivate(this))
{
    Q_D(CanRawSender);

    d->setupUi(this);
}

CanRawSender::~CanRawSender()
{
}

void CanRawSender::start()
{
    Q_D(CanRawSender);

    d->started = true;
}

void CanRawSender::stop()
{
    Q_D(CanRawSender);

    d->started = false;
}
