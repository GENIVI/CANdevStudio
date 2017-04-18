#include "cansignalsender.h"
#include "cansignalsender_p.h"

CanSignalSender::CanSignalSender(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CanSignalSenderPrivate)
{
    Q_D(CanSignalSender);

    d->setupUi();
    setLayout(d->layout.get());
}

CanSignalSender::~CanSignalSender()
{

}
