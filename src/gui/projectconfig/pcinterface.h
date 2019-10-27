#ifndef __PCINTERFACE_H
#define __PCINTERFACE_H

#include <functional>
#include <QPointF>

namespace QtNodes {
class FlowScene;
class Node;
}

class QPoint;
class QPointF;
class QMenu;

struct PCInterface {
    virtual ~PCInterface()
    {
    }

    using node_t = std::function<void(QtNodes::Node&)>;
    using menu_t = std::function<void(QtNodes::Node&, const QPointF&)>;

    virtual void setNodeCreatedCallback(QtNodes::FlowScene* scene, const node_t& cb) = 0;
    virtual void setNodeDeletedCallback(QtNodes::FlowScene* scene, const node_t& cb) = 0;
    virtual void setNodeDoubleClickedCallback(QtNodes::FlowScene* scene, const node_t& cb) = 0;
    virtual void setNodeContextMenuCallback(QtNodes::FlowScene* scene, const menu_t& cb) = 0;
    virtual void setConfigChangedCbk(const node_t cb) = 0;
    virtual void openProperties(QtNodes::Node& node) = 0;
    virtual void showContextMenu(QMenu &menu, const QPoint& pos) = 0;
};


#endif /* !__PCINTERFACE_H */
