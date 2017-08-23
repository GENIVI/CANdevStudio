#ifndef NLMPUSHBUTTONINTERFACE_HPP
#define NLMPUSHBUTTONINTERFACE_HPP

#include <QString>
#include <functional>
class QWidget;

struct NLMPushButtonInterface {
    virtual ~NLMPushButtonInterface() {}

    typedef std::function<void()> pressed_t;
    virtual void pressedCbk(const pressed_t& cb) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual void init(const QString& buttonName, bool isEnable) = 0;
    virtual void setDisabled(bool state) = 0;
    virtual bool isEnabled() = 0;
};
#endif // NLMPUSHBUTTONINTERFACE_HPP
