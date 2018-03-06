#ifndef __NODEPAINTER_H
#define __NODEPAINTER_H

#include <QPainter>
#include <QtWidgets/QGraphicsEffect>
#include <nodes/Node>

struct NodePainter : public QtNodes::NodePainterDelegate {

    NodePainter() = default;

    NodePainter(const QColor& headerColor1, const QColor& headerColor2)
        : _headerColor1(headerColor1)
        , _headerColor2(headerColor2)
    {
    }

    virtual void paint(QPainter* painter, QtNodes::NodeGeometry const& geom, QtNodes::NodeDataModel const* model,
        QtNodes::NodeGraphicsObject const& graphicsObject) override
    {
        QtNodes::NodeStyle const& nodeStyle = model->nodeStyle();

        auto color = nodeStyle.NormalBoundaryColor;

        QPen p(color, nodeStyle.PenWidth);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(_headerColor1));

        float diam = nodeStyle.ConnectionPointDiameter;

        QRectF boundary;
        QRectF boundary2;
        double radius;
        if (geom.hovered()) {
            boundary = QRectF(-diam + 1, -diam + 1, 2.0 * diam + geom.width() - 2, 21);
            boundary2 = QRectF(-diam + 1, -diam + 11, 2.0 * diam + geom.width() - 2, 12);
            radius = 2.3;
        } else if(graphicsObject.isSelected()) {
            boundary = QRectF(-diam + 0.75, -diam + 0.75, 2.0 * diam + geom.width() - 1.5, 21.25);
            boundary2 = QRectF(-diam + 0.75, -diam + 11, 2.0 * diam + geom.width() - 1.5, 12);
            radius = 2.1;
        } else {
            boundary = QRectF(-diam - 0.75, -diam - 0.75, 2.0 * diam + geom.width() + 1.5, 22.75);
            boundary2 = QRectF(-diam - 0.75, -diam + 11, 2.0 * diam + geom.width() + 1.5, 12);
            radius = 3;
        }

        painter->drawRoundedRect(boundary, radius, radius);
        painter->drawRect(boundary2);

        painter->setPen({ Qt::white, 1 });
        QFont font({ "Arial", 10 });
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(-diam + 6, -diam + 16, model->caption());
    }

private:
    const QColor _headerColor1;
    const QColor _headerColor2;
};

#endif /* !__NODEPAINTER_H */
