#ifndef FLOWVIEWWRAPPER_H
#define FLOWVIEWWRAPPER_H

#include <nodes/FlowScene>
#include <nodes/FlowView>

#include <log.h>

struct FlowViewWrapper : public QtNodes::FlowView {
    FlowViewWrapper(QtNodes::FlowScene* scene)
        : QtNodes::FlowView(scene)

    {
        cds_debug("FlowViewWrapper");
        setAcceptDrops(true);
    }

    void dragMoveEvent(QDragMoveEvent *event) override { cds_debug("dragMoveEvent"); }
    void dropEvent(QDropEvent *event) override { cds_debug("dropEvent"); }
    void addNode() { cds_debug("addNode"); }
};

#endif // FLOWVIEWWRAPPER_H
