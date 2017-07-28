#include <CanDeviceManager.hpp>

#include <candevice/candevice.h>
#include <log.hpp>

#include <QString>

CanDeviceManager::CanDeviceManager()
{

}

CanDeviceManager::~CanDeviceManager()
{

}

void CanDeviceManager::fetchAvailableDevices(QString backend)
{
    QString errorMessage;
    auto devices = qtfactory.availableDevices(backend, &errorMessage);

    if (!errorMessage.isEmpty())
    {
        cds_error("Error fetching available devices: {0}", errorMessage.toStdString());
        return;
    }

    emit sendAvailableDevices(backend, devices);
}

void CanDeviceManager::selectCANDevice(QString backend, QString name)
{
    QString errorMessage;
    auto devices = qtfactory.availableDevices(backend, &errorMessage);
    if (!errorMessage.isEmpty())
    {
        cds_error("Error fetching available devices: {0}", errorMessage.toStdString());
        return;
    }

    auto result = std::find_if(devices.begin(), devices.end(), [&name](const auto& dev){ return dev.name() == name; });

    if (result == devices.end())
    {
        cds_error("Failed to find {0} device on backend {1}", name.toStdString(), backend.toStdString());

        return;
    }

    if (device)
    {
        device->deleteLater();
        device = nullptr;
    }

    device = new CanDevice(qtfactory);
    emit attachToViews(device);

    device->init(backend, name);
    device->start();
}
