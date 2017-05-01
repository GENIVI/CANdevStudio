#include "cansignalview.h"
#include "cansignalview_p.h"
#include <QList>
#include <QStandardItem>
#include <QString>

CanSignalView::CanSignalView(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CanSignalViewPrivate())
{
    Q_D(CanSignalView);

    d->setupUi(this);
}

CanSignalView::~CanSignalView()
{
}

void CanSignalView::signalReceived(const QString& name, const QByteArray& val)
{
    Q_D(CanSignalView);

    d->insertRow("RX", name, QString::number(val.toUInt()));
}

void CanSignalView::signalSent(const QString& name, const QByteArray& val)
{
    Q_D(CanSignalView);

    d->insertRow("TX", name, QString::number(val.toUInt()));
}

void CanSignalView::clear()
{
    Q_D(CanSignalView);

    d->clear();
}

void CanSignalView::start()
{
    Q_D(CanSignalView);

    d->startTimer();
}
