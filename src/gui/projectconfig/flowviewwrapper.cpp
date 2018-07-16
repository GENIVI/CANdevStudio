#include <QtWidgets>
#include <nodes/Node>
#include <log.h>

#include "flowviewwrapper.h"

void FlowViewWrapper::dragMoveEvent(QDragMoveEvent *) {}

void FlowViewWrapper::dropEvent(QDropEvent *event)
{
    QByteArray itemData = event->mimeData()->data("CANdevStudio/x-dnditemdata");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    QPoint offset;
    QString nodeType;
    dataStream >> offset >> nodeType;

    addNode(nodeType, event->pos());
}

void FlowViewWrapper::addNode(const QString& modelName, const QPoint& pos)
{
    cds_debug("addNode");
    auto type = _scene->registry().create(modelName);
    if (type) {
        auto& node = _scene->createNode(std::move(type));
        node.nodeGraphicsObject().setPos(mapToScene(pos));
    } else {
        cds_warn("Failed to create {} node", modelName.toStdString());
    }
}
