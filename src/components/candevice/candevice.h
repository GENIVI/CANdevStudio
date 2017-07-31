#ifndef __CANDEVICE_H
#define __CANDEVICE_H

#include <QScopedPointer>
#include <QtCore/QObject>
#include <QtSerialBus/QCanBusDevice>
#include <QtSerialBus/QCanBusFrame>

class CanDevicePrivate;
struct CanFactoryInterface;

/**
*   @brief The class provides abstraction layer for CAN BUS hardware
*/
class CanDevice : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanDevice)

public:
    CanDevice(CanFactoryInterface& factory);
    ~CanDevice();

    /**
    *   @brief  Configures CAN BUS backend and interface
    *
    *   This function is used to configure QtCanBus class. 
    *
    *   @param  backend one of backends supported by QtCanBus class
    *   @param  iface CAN BUS interface index (e.g. can0 for socketcan backend)
    *   @return true on success, false of failure
    */
    bool init(const QString& backend, const QString& iface);
    bool start();

Q_SIGNALS:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);

public Q_SLOTS:
    void sendFrame(const QCanBusFrame& frame);

private Q_SLOTS:
    void errorOccurred(int error);
    void framesWritten(qint64 framesCnt);
    void framesReceived();

private:
    QScopedPointer<CanDevicePrivate> d_ptr;
    CanFactoryInterface& mFactory;
};

#endif /* !__CANDEVICE_H */
