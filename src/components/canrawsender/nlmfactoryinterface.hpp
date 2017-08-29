#ifndef NLMFACTORYINTERFACE_HPP
#define NLMFACTORYINTERFACE_HPP

struct CheckBoxInterface;
struct LineEditInterface;
struct PushButtonInterface;

struct NLMFactoryInterface {
    virtual CheckBoxInterface* createCheckBox() = 0;
    virtual LineEditInterface* createLineEdit() = 0;
    virtual PushButtonInterface* createPushButton() = 0;
};
#endif // NLMFACTORYINTERFACE_HPP
