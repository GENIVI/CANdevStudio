#ifndef CANFACTORY_HPP_C394TY5U
#define CANFACTORY_HPP_C394TY5U

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
#include <QCanBusDeviceInfo>
#endif

#include <QList>
#include <QString>

struct CanDeviceInterface;
struct CanFactoryInterface {
    virtual ~CanFactoryInterface() {}
    virtual CanDeviceInterface* create(const QString& backend, const QString& iface) = 0;
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 1)
    virtual QList<QCanBusDeviceInfo> availableDevices(const QString& backend, QString* errorMessage = nullptr) const = 0;
#endif
};

#endif /* end of include guard: CANFACTORY_HPP_C394TY5U */
