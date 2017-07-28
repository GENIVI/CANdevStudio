#ifndef CANFACTORYQT_HPP_Z8TCG4MR
#define CANFACTORYQT_HPP_Z8TCG4MR

#include "candeviceqt.hpp"
#include "canfactory.hpp"

#include <QCanBusDeviceInfo>
#include <QList>

struct CanFactoryQt : public CanFactoryInterface {
    virtual CanDeviceInterface* create(const QString& backend, const QString& iface) override
    {
        return new CanDeviceQt(backend, iface);
    }

    virtual QList<QCanBusDeviceInfo> availableDevices(const QString& backend, QString* errorMessage = nullptr) const
    {
        return QCanBus::instance()->availableDevices(backend, errorMessage);
    }
};

#endif /* end of include guard: CANFACTORYQT_HPP_Z8TCG4MR */
