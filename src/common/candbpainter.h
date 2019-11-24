#ifndef __CANDBPAINTER_H
#define __CANDBPAINTER_H

#include "nodepainter.h"
#include <QSvgRenderer>

class ComponentInterface;

struct CanDbPainter : public NodePainter {

    CanDbPainter(const QColor& headerColor, const ComponentInterface* component, qreal size);

    virtual void paint(QPainter* painter, QtNodes::NodeGeometry const& geom, QtNodes::NodeDataModel const* model,
        QtNodes::NodeGraphicsObject const& graphicsObject) override;

private:
    const ComponentInterface* _component;
    QColor _nodeColor;
    QColor _prevNodeColor;
    QSvgRenderer _svg;
    qreal _s;
    const QColor _lightColor;
    const QColor _darkColor;
};

#endif /* !__CANDBPAINTER_H */
