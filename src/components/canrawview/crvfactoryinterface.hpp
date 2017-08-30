#ifndef CRVFACTORYINTERFACE_H
#define CRVFACTORYINTERFACE_H

struct CRVGuiInterface;

struct CRVFactoryInterface {
    virtual CRVGuiInterface* createGui() = 0;
};

#endif // CRVFACTORYINTERFACE_H
