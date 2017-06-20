#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include <QtCore/QObject>
#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusFrame>
#include <QtCore/QScopedPointer>
#include <QtCore/QVariant>
#include <QtCore/QVector>

class CanDevicePrivate
{
public:
    QString mBackend;
    QString mInterface;
    QVector<std::pair<QCanBusFrame, QVariant>> mSendQueue;
    QScopedPointer<QCanBusDevice> mDevice;
};

#endif /* !__CANDEVICE_P_H */
