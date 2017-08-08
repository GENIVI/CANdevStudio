#ifndef CANFACTORYQT_HPP_Z8TCG4MR
#define CANFACTORYQT_HPP_Z8TCG4MR

#include "candeviceqt.hpp"
#include "canfactory.hpp"

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
#include <QCanBusDeviceInfo>
#endif
#include <QList>

struct CanFactoryQt : public CanFactoryInterface {
    virtual CanDeviceInterface* create(const QString& backend, const QString& iface) override
    {
        return new CanDeviceQt(backend, iface);
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
    virtual QList<QCanBusDeviceInfo> availableDevices(const QString& backend, QString* errorMessage = nullptr) const
    {
        return QCanBus::instance()->availableDevices(backend, errorMessage);
    }
#endif
};

#endif /* end of include guard: CANFACTORYQT_HPP_Z8TCG4MR */
