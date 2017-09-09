#ifndef PUSHBUTTONINTERFACE_H
#define PUSHBUTTONINTERFACE_H

#include <QString>
#include <functional>
class QWidget;

struct PushButtonInterface {
    virtual ~PushButtonInterface()
    {
    }

    typedef std::function<void()> pressed_t;
    virtual void pressedCbk(const pressed_t& cb) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual void init(const QString& buttonName, bool isEnable) = 0;
    virtual void setDisabled(bool state) = 0;
    virtual bool isEnabled() = 0;
};
#endif // PUSHBUTTONINTERFACE_H
