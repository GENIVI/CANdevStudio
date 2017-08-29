#ifndef NLMFACTORYINTERFACE_HPP
#define NLMFACTORYINTERFACE_HPP

class CheckBoxInterface;
class LineEditInterface;
class PushButtonInterface;

struct NLMFactoryInterface {
    virtual CheckBoxInterface* createCheckBox() = 0;
    virtual LineEditInterface* createLineEdit() = 0;
    virtual PushButtonInterface* createPushButton() = 0;
};
#endif // NLMFACTORYINTERFACE_HPP
