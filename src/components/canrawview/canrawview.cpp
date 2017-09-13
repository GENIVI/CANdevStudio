#include "canrawview.h"
#include "canrawview_p.h"
#include "log.h"
#include <QtCore/QElapsedTimer>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSerialBus/QCanBusFrame>

CanRawView::CanRawView()
    : d_ptr(new CanRawViewPrivate(this))
{
}

CanRawView::CanRawView(CanRawViewCtx&& ctx)
    : d_ptr(new CanRawViewPrivate(this, std::move(ctx)))
{
}

CanRawView::~CanRawView()
{
}

void CanRawView::startSimulation()
{
    Q_D(CanRawView);

    d->_timer.restart();
    d->_simStarted = true;
    d->clear();
}

void CanRawView::stopSimulation()
{
    Q_D(CanRawView);

    d->_simStarted = false;
}

void CanRawView::frameReceived(const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    d->frameView(frame, "RX");
}

void CanRawView::frameSent(bool status, const QCanBusFrame& frame)
{
    Q_D(CanRawView);

    if (status) {
        d->frameView(frame, "TX");
    }
}

QWidget* CanRawView::getMainWidget()
{
    Q_D(CanRawView);

    return d->_ui.getMainWidget();
}

void CanRawView::setConfig(QJsonObject&)
{
    // TODO
}

QJsonObject CanRawView::getConfig() const
{
    // Q_D(CanRawView);
    QJsonObject config;

    d_ptr->saveSettings(config);

    return config;
}

void CanRawView::setDockUndockClbk(const std::function<void()>& cb)
{
    Q_D(CanRawView);

    d->_ui.setDockUndockCbk(cb);
}

bool CanRawView::mainWidgetDocked() const
{
    return d_ptr->docked;
}
