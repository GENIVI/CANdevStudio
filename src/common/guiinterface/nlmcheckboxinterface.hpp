#ifndef NLMCHECKBOXINTERFACE_HPP
#define NLMCHECKBOXINTERFACE_HPP

#include <functional>
class QWidget;

struct NLMCheckBoxInterface {
    virtual ~NLMCheckBoxInterface() {}

    typedef std::function<void()> released_t;
    virtual void releasedCbk(const released_t& cb) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual bool getState() = 0;
};
#endif // NLMCHECKBOXINTERFACE_HPP
