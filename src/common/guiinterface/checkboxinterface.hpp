#ifndef CHECKBOXINTERFACE_HPP
#define CHECKBOXINTERFACE_HPP

#include <functional>
class QWidget;

struct CheckBoxInterface {
    virtual ~CheckBoxInterface()
    {
    }

    typedef std::function<void()> released_t;
    virtual void releasedCbk(const released_t& cb) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual bool getState() = 0;
};
#endif // CHECKBOXINTERFACE_HPP
