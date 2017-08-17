#ifndef CRVFACTORYINTERFACE_H
#define CRVFACTORYINTERFACE_H

class CRVGuiInterface;

struct CRVFactoryInterface {
    virtual CRVGuiInterface* createGui() = 0;
};

#endif // CRVFACTORYINTERFACE_H
