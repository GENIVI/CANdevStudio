#include "canrawsender.h"
#include "canrawsender_p.h"
#include <cassert>

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
    auto parentWidget = getMainWidget();
    if (parentWidget) {
        parentWidget->hide();
    }
/*
    } else {
        hide();
    }
*/
    e->ignore();
}
void CanRawSender::startSimulation()
{
    assert(d_ptr != nullptr);
    d_ptr->SetSimulationState(true);
}

void CanRawSender::stopSimulation()
{
    assert(d_ptr != nullptr);
    d_ptr->SetSimulationState(false);
}

QWidget* CanRawSender::getMainWidget()
{
    Q_D(CanRawSender);

    return d->mUi->getMainWidget();
}
