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
    CanDeviceManager();
    virtual ~CanDeviceManager();

signals:
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
    void sendAvailableDevices(QString backend, QList<QCanBusDeviceInfo> devices);
#else
    void sendAvailableDevicesDummy(QString backend);
#endif
    void attachToViews(CanDevice* device);

public slots:
    void selectCANDevice(QString backend, QString name);
    void fetchAvailableDevices(QString backend);

private:
    CanFactoryQt qtfactory;
    CanDevice* device = nullptr;
};

#endif /* SRC_GUI_CANDEVICEMANAGER_HPP_ */
