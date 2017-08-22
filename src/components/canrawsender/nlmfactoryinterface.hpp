#ifndef NLMFACTORYINTERFACE_HPP
#define NLMFACTORYINTERFACE_HPP

class NLMCheckBoxInterface;
class NLMLineEditInterface;
class NLMPushButtonInterface;

struct NLMFactoryInterface {
    virtual NLMCheckBoxInterface* createCheckBox() = 0;
    virtual NLMLineEditInterface* createLineEdit() = 0;
    virtual NLMPushButtonInterface* createPushButton() = 0;
};
#endif // NLMFACTORYINTERFACE_HPP
