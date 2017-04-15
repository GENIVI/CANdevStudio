#include "cansignalcoder.h"
#include "cansignalcoder_p.h"

CanSignalCoder::CanSignalCoder(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CanSignalCoderPrivate())
{
    Q_D(CanSignalCoder);

    d->signalMap.emplace<quint32, CanSignalCoderPrivate::SignalDesc>(0x123, {"VehicleSpeed", 0x1ff, 0, 0});
    //d->signalMap[0x123] = {"SteeringWheelAngle", 0x7fe00, 9, 0};
}

CanSignalCoder::~CanSignalCoder()
{
}

void CanSignalCoder::frameReceived(const QCanBusFrame &frame)
{
    Q_D(CanSignalCoder);
}

void CanSignalCoder::signalReceived(const QString &name, const QByteArray &value)
{
    Q_D(CanSignalCoder);
}
