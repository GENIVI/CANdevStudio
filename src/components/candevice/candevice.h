#ifndef __CANDEVICE_H
#define __CANDEVICE_H

#include <QtCore/qobject.h>
#include <QtSerialBus/qcanbusframe.h>
#include <QCanBusDevice>
#include <memory>
#include <QScopedPointer>
#include <QString>

class CanDevicePrivate;

class CanDevice : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanDevice)

public:
    CanDevice();
    ~CanDevice();
    bool init(const QString &backend, const QString &interface);
    bool start();

Q_SIGNALS:
    void frameReceived(const QCanBusFrame &frame);
    void frameSent(bool status, const QCanBusFrame &frame, const QVariant &context);

public Q_SLOTS:
    void sendFrame(const QCanBusFrame &frame, const QVariant &context);

private Q_SLOTS:
    void errorOccurred(QCanBusDevice::CanBusError error);
    void framesWritten(qint64 framesCnt);
    void framesReceived();

private:
    QScopedPointer<CanDevicePrivate> d_ptr;
};

#endif /* !__CANDEVICE_H */
