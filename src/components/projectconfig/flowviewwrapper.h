#ifndef FLOWVIEWWRAPPER_H
#define FLOWVIEWWRAPPER_H

#include <nodes/FlowScene>
#include <nodes/FlowView>

struct FlowViewWrapper : public QtNodes::FlowView {
    FlowViewWrapper(QtNodes::FlowScene* scene)
        : QtNodes::FlowView(scene)
        , _scene(scene)
    {
        setAcceptDrops(true);
    }

    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void addNode(const QString& modelName, const QPoint& pos);
private:
    QtNodes::FlowScene* _scene;
};

#endif // FLOWVIEWWRAPPER_H
