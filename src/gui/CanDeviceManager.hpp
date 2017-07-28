#ifndef SRC_GUI_CANDEVICEMANAGER_HPP_
#define SRC_GUI_CANDEVICEMANAGER_HPP_

#include <candevice/canfactoryqt.hpp>
#include <candevice/candevice.h>

#include <QCanBusDeviceInfo>
#include <QList>
#include <QObject>

class CanDeviceManager : public QObject
{
    Q_OBJECT

public:
    CanDeviceManager();
    virtual ~CanDeviceManager();

signals:
    void sendAvailableDevices(QString backend, QList<QCanBusDeviceInfo> devices);
    void attachToViews(CanDevice* device);

public slots:
    void fetchAvailableDevices(QString backend);
    void selectCANDevice(QString backend, QString name);

private:
    CanFactoryQt qtfactory;
    CanDevice* device = nullptr;
};

#endif /* SRC_GUI_CANDEVICEMANAGER_HPP_ */
