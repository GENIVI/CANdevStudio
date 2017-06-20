#include "canrawsender.h"
#include "canrawsender_p.h"

CanRawSender::CanRawSender(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CanRawSenderPrivate(this))
{
    Q_D(CanRawSender);

    d->setupUi();
    setLayout(&d->layout);
}

CanRawSender::~CanRawSender()
{

}
