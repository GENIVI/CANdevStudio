#ifndef SRC_GUI_CANDEVICEMANAGER_HPP_
#define SRC_GUI_CANDEVICEMANAGER_HPP_

#include <candevice/canfactoryqt.hpp>
#include <candevice/candevice.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
#include <QCanBusDeviceInfo>
#endif
#include <QList>
#include <QObject>

class CanDeviceManager : public QObject
{
    Q_OBJECT

public:
    CanDeviceManager() = default;
    virtual ~CanDeviceManager() = default;

signals:
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
    void sendAvailableDevices(const QString& backend, const QList<QCanBusDeviceInfo>& devices);
#else
    void sendAvailableDevicesDummy(const QString& backend);
#endif
    /**
     * Request attaching of CAN device to prepared CanRawView and CanRawSender
     * @param device Pointer to source CanDevice. Ownership is not transferred.
     */
    void attachToViews(CanDevice* device);

public slots:
    void selectCANDevice(const QString& backend, const QString& name);
    void fetchAvailableDevices(const QString& backend);

private:
    CanFactoryQt qtfactory;
    CanDevice* device = nullptr;
};

#endif /* SRC_GUI_CANDEVICEMANAGER_HPP_ */
