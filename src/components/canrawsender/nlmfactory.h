#ifndef NLMFACTORY_H
#define NLMFACTORY_H

#include "gui/nlmcheckbox.h"
#include "gui/nlmlineedit.h"
#include "gui/nlmpushbutton.h"
#include "nlmfactoryinterface.h"

struct NLMFactory : public NLMFactoryInterface {
    virtual ~NLMFactory()
    {
    }

    CheckBoxInterface* createCheckBox() override
    {
        return new NLMCheckBox();
    }
    LineEditInterface* createLineEdit() override
    {
        return new NLMLineEdit;
    }
    PushButtonInterface* createPushButton() override
    {
        return new NLMPushButton;
    }
};
#endif // NLMFACTORY_H
