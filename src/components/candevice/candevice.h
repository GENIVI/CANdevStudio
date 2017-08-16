#ifndef __CANDEVICE_H
#define __CANDEVICE_H

#include <QScopedPointer>
#include <QtCore/QObject>
#include <QtSerialBus/QCanBusDevice>
#include <QtSerialBus/QCanBusFrame>

class CanDevicePrivate;
struct CanDeviceInterface;

/**
*   @brief The class provides abstraction layer for CAN BUS hardware
*/
class CanDevice : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanDevice)

public:
    /**
     * @brief Construct a @c CanDevice object using @c canDevice as its actual communication backend.
     *
     * @param canDevice Reference to an object implementing the @c CanDeviceInterface . The object must remain alive
     * throughout this object's lifetime.
     *
     * @note @c canDevice should really be a @c unique_ptr that's @c std::move -d into the constructor. However, our
     * mocking framework does not allow making custom instances of mocked classes. This has the added advantage of not
     * requiring any null-checks, which would've been necessary with a @c unique_ptr .
     */
    CanDevice(CanDeviceInterface& canDevice);
    ~CanDevice();

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
    CanDeviceInterface& canDevice_;
};

#endif /* !__CANDEVICE_H */
