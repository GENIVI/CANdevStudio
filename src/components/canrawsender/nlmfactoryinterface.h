#ifndef NLMFACTORYINTERFACE_H
#define NLMFACTORYINTERFACE_H

struct CheckBoxInterface;
struct LineEditInterface;
struct PushButtonInterface;

struct NLMFactoryInterface {
    virtual ~NLMFactoryInterface()
    {
    }
    virtual CheckBoxInterface* createCheckBox() = 0;
    virtual LineEditInterface* createLineEdit() = 0;
    virtual PushButtonInterface* createPushButton() = 0;
};
#endif // NLMFACTORYINTERFACE_H
