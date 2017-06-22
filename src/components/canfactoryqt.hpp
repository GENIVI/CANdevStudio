#ifndef CANFACTORYQT_HPP_Z8TCG4MR
#define CANFACTORYQT_HPP_Z8TCG4MR

#include "candeviceqt.hpp"
#include "canfactory.hpp"

struct CanFactoryQt : public CanFactoryInterface {
    virtual CanDeviceInterface* create(const QString& backend, const QString& iface) override
    {
        return new CanDeviceQt(backend, iface);
    }
};

#endif /* end of include guard: CANFACTORYQT_HPP_Z8TCG4MR */
