#ifndef NLMFACTORY_HPP
#define NLMFACTORY_HPP

#include "gui/nlmcheckbox.hpp"
#include "nlmfactoryinterface.hpp"
#include "gui/nlmlineedit.hpp"
#include "gui/nlmpushbutton.hpp"

struct NLMFactory : public NLMFactoryInterface {
    virtual ~NLMFactory() {}

    CheckBoxInterface* createCheckBox() override { return new NLMCheckBox(); }
    LineEditInterface* createLineEdit() override { return new NLMLineEdit; }
    PushButtonInterface* createPushButton() override { return new NLMPushButton; }
};
#endif // NLMFACTORY_HPP
