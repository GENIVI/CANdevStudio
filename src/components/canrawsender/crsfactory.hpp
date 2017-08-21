#ifndef CRSFACTORY_HPP
#define CRSFACTORY_HPP

#include "crsfactoryinterface.hpp"
#include "crsgui.hpp"

struct CRSFactory : public CRSFactoryInterface {
    virtual ~CRSFactory() {}
    virtual CRSGuiInterface* createGui() override {
        return new CRSGui;
    }
};
#endif // CRSFACTORY_HPP
