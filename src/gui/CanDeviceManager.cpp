#include <CanDeviceManager.hpp>

#include <candevice/candevice.h>
#include <iostream>

CanDeviceManager::CanDeviceManager()
{

}

CanDeviceManager::~CanDeviceManager()
{

}

void CanDeviceManager::fetchAvailableDevices(QString backend)
{
    QString errorMessage; // FIXME: log error
    auto devices = qtfactory.availableDevices(backend, &errorMessage);

    emit sendAvailableDevices(backend, devices);
}

void CanDeviceManager::selectCANDevice(QString backend, QString name)
{
    QString errorMessage; // FIXME: log error
    auto devices = qtfactory.availableDevices(backend, &errorMessage);

    auto result = std::find_if(devices.begin(), devices.end(), [&name](const auto& dev){ return dev.name() == name; });

    if (result == devices.end())
    {
        // FIXME: log error, show selection dialog again
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
