#ifndef CRSFACTORYINTERFACE_HPP
#define CRSFACTORYINTERFACE_HPP

class CRSGuiInterface;

struct CRSFactoryInterface {
    virtual CRSGuiInterface* createGui() = 0;
};

#endif // CRSFACTORYINTERFACE_HPP
