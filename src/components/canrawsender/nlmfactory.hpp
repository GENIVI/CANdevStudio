#ifndef NLMFACTORY_HPP
#define NLMFACTORY_HPP

#include "nlmcheckbox.hpp"
#include "nlmfactoryinterface.hpp"
#include "nlmlineedit.hpp"
#include "nlmpushbutton.hpp"

struct NLMFactory : public NLMFactoryInterface {
    virtual ~NLMFactory() {}

    NLMCheckBoxInterface* createCheckBox() override { return new NLMCheckBox(); }
    NLMLineEditInterface* createLineEdit() override { return new NLMLineEdit; }
    NLMPushButtonInterface* createPushButton() override { return new NLMPushButton; }
};
#endif // NLMFACTORY_HPP
