#ifndef CANSIGNALVIEWERGUIINT_H
#define CANSIGNALVIEWERGUIINT_H

#include <Qt>
#include <functional>

class QWidget;

struct CanSignalViewerGuiInt {
    virtual ~CanSignalViewerGuiInt() {}
    virtual QWidget* mainWidget() = 0;
};

#endif // CANSIGNALVIEWERGUIINT_H
