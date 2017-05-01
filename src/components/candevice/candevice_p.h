#ifndef __CANDEVICE_P_H
#define __CANDEVICE_P_H

#include <QCanBus>
#include <QCanBusFrame>
#include <QScopedPointer>
#include <QVariant>
#include <QVector>
#include <QtCore/qobject.h>

class CanDevicePrivate {
public:
    QString mBackend;
    QString mInterface;
    QVector<std::pair<QCanBusFrame, QVariant> > mSendQueue;
    QScopedPointer<QCanBusDevice> mDevice;
};

#endif /* !__CANDEVICE_P_H */
