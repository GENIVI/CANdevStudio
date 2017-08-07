#include <CanDeviceManager.hpp>

#include <candevice/candevice.h>
#include <log.hpp>

#include <QString>

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
void CanDeviceManager::fetchAvailableDevices(const QString& backend)
{
    QString errorMessage;
    const auto devices = qtfactory.availableDevices(backend, &errorMessage);

    if (!errorMessage.isEmpty())
    {
        cds_error("Error fetching available devices: {0}", errorMessage.toStdString());
        return;
    }

    emit sendAvailableDevices(backend, devices);
}
#else
void CanDeviceManager::fetchAvailableDevices(const QString& backend)
{
    emit sendAvailableDevicesDummy(backend);
}
#endif

void CanDeviceManager::selectCANDevice(const QString& backend, const QString& name)
{
    QString errorMessage;
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
    const auto devices = qtfactory.availableDevices(backend, &errorMessage);
    if (!errorMessage.isEmpty())
    {
        cds_error("Error fetching available devices: {0}", errorMessage.toStdString());
        return;
    }

    const auto result = std::find_if(devices.begin(), devices.end(), [&name](const auto& dev)
    {
        return dev.name() == name;
    });

    if (result == devices.end())
    {
        cds_error("Failed to find {0} device on backend {1}", name.toStdString(), backend.toStdString());
        return;
    }
#endif

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
