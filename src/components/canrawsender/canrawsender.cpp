#include "canrawsender.h"
#include "canrawsender_p.h"

CanRawSender::CanRawSender()
    : d_ptr(new CanRawSenderPrivate(this))
{
    Q_D(CanRawSender);
}

CanRawSender::CanRawSender(CRSFactoryInterface& factory)
    : d_ptr(new CanRawSenderPrivate(this, factory))
{
    Q_D(CanRawSender);
}

CanRawSender::~CanRawSender() {}

void CanRawSender::closeEvent(QCloseEvent* e)
{
    if (parentWidget()) {
        parentWidget()->hide();
    } else {
        hide();
    }
    e->ignore();
}
void CanRawSender::startSimulation()
{
    d_ptr->SetSimulationState(true);
}

void CanRawSender::stopSimulation()
{
    d_ptr->SetSimulationState(false);
}

QWidget* CanRawSender::getMainWidget()
{
    Q_D(CanRawSender);

    return d->mUi->getMainWidget();
}
