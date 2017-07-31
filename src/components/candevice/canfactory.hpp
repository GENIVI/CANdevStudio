#ifndef CANFACTORY_HPP_C394TY5U
#define CANFACTORY_HPP_C394TY5U

#include <QCanBusDeviceInfo>
#include <QList>
#include <QString>

struct CanDeviceInterface;
struct CanFactoryInterface {
    virtual ~CanFactoryInterface() {}
    virtual CanDeviceInterface* create(const QString& backend, const QString& iface) = 0;
    virtual QList<QCanBusDeviceInfo> availableDevices(const QString& backend, QString* errorMessage = nullptr) const = 0;
};

#endif /* end of include guard: CANFACTORY_HPP_C394TY5U */
