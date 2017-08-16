#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtSerialBus/QCanBus>
#include <QtSerialBus/QCanBusFrame>

class CanDevicePrivate {
public:
    QString mBackend;
    QString mInterface;
    QVector<QCanBusFrame> mSendQueue;
};

#endif /* !__CANDEVICE_P_H */
