#ifndef CHECKBOXINTERFACE_H
#define CHECKBOXINTERFACE_H

#include <functional>
class QWidget;

struct CheckBoxInterface {
    virtual ~CheckBoxInterface()
    {
    }

    typedef std::function<void(bool)> toggled_t;
    virtual void toggledCbk(const toggled_t& cb) = 0;
    virtual QWidget* mainWidget() = 0;
    virtual bool getState() = 0;
    virtual void setState(bool state) = 0;
    virtual void setDisabled(bool state) = 0;
};
#endif // CHECKBOXINTERFACE_H
