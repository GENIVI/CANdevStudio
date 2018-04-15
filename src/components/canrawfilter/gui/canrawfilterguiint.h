#ifndef CANRAWFILTERGUIINT_H
#define CANRAWFILTERGUIINT_H

#include <Qt>
#include <functional>

class QWidget;

struct CanRawFilterGuiInt {
    virtual ~CanRawFilterGuiInt()
    {
    }

    virtual QWidget* mainWidget() = 0;
};

#endif // CANRAWFILTERGUIINT_H
