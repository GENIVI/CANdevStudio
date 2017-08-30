#ifndef CRVFACTORY_H
#define CRVFACTORY_H

#include "crvfactoryinterface.hpp"
#include "crvgui.hpp"

struct CRVFactory : public CRVFactoryInterface {
    virtual ~CRVFactory() {}
    virtual CRVGuiInterface* createGui() override {
        return new CRVGui;
    }
};

#endif // CRVFACTORY_H
