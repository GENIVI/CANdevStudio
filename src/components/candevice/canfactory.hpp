#ifndef CANFACTORY_HPP_C394TY5U
#define CANFACTORY_HPP_C394TY5U

#include <QtCore/QString>

struct CanDeviceInterface;
struct CanFactoryInterface {
    virtual ~CanFactoryInterface() {}
    virtual CanDeviceInterface* create(const QString& backend, const QString& iface) = 0;
};

#endif /* end of include guard: CANFACTORY_HPP_C394TY5U */
