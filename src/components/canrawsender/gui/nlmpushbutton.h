#ifndef NLMPUSHBUTTON_H
#define NLMPUSHBUTTON_H

#include <QPushButton>
#include <guiinterface/pushbuttoninterface.h>

struct NLMPushButton : public PushButtonInterface {

    NLMPushButton()
        : qPushButton(new QPushButton)
    {
        qPushButton->setProperty("type", "nlmItem");
    }

    void pressedCbk(const pressed_t& cb) override
    {
        QObject::connect(qPushButton, &QPushButton::pressed, cb);
    }

    QWidget* mainWidget() override
    {
        return qPushButton;
    }

    void init(const QString& buttonName, bool isEnable) override
    {
        qPushButton->setText(buttonName);
        qPushButton->setFlat(true);
        setDisabled(!isEnable);
    }

    void setDisabled(bool state) override
    {
        qPushButton->setDisabled(state);
    }

    bool isEnabled() override
    {
        return qPushButton->isEnabled();
    }

    void setCheckable(bool checked) override
    {
        qPushButton->setCheckable(checked);
    }

    bool checkable() override
    {
        return qPushButton->isCheckable();
    }

    bool checked() override
    {
        return qPushButton->isChecked();
    }

    void setChecked(bool checked) override
    {
        if(checked != qPushButton->isChecked()) {
            qPushButton->animateClick();
        }
    }


private:
    QPushButton* qPushButton;
};
#endif // NLMPUSHBUTTON_H
