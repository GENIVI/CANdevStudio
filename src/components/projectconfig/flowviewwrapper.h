#ifndef FLOWVIEWWRAPPER_H
#define FLOWVIEWWRAPPER_H

#include <nodes/FlowScene>
#include <nodes/FlowView>

struct FlowViewWrapper : public QtNodes::FlowView {
    FlowViewWrapper(QtNodes::FlowScene* scene)
        : QtNodes::FlowView(scene)

    {
        setAcceptDrops(true);
    }
};

#endif // FLOWVIEWWRAPPER_H
