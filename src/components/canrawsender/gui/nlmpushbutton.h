#ifndef NLMPUSHBUTTON_H
#define NLMPUSHBUTTON_H

#include <QPushButton>
#include <guiinterface/pushbuttoninterface.h>

struct NLMPushButton : public PushButtonInterface {

    NLMPushButton()
        : qPushButton(new QPushButton)
    {
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

private:
    QPushButton* qPushButton;
};
#endif // NLMPUSHBUTTON_H
