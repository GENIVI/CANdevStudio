#ifndef CANSIGNALSENDERGUIINT_H
#define CANSIGNALSENDERGUIINT_H

#include <Qt>
#include <functional>

class QWidget;

struct CanSignalSenderGuiInt {
    virtual ~CanSignalSenderGuiInt() {}
    virtual QWidget* mainWidget() = 0;
};

#endif // CANSIGNALSENDERGUIINT_H
