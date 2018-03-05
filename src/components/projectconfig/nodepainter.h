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

    virtual void paint(
        QPainter* painter, QtNodes::NodeGeometry const& geom, QtNodes::NodeDataModel const* model) override
    {
        QtNodes::NodeStyle const& nodeStyle = model->nodeStyle();

        auto color = nodeStyle.NormalBoundaryColor;

        QPen p(color, nodeStyle.PenWidth);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(_headerColor1));

        float diam = nodeStyle.ConnectionPointDiameter;

        if (geom.hovered()) {
            QRectF boundary(-diam + 0.8, -diam + 0.8, 2.0 * diam + geom.width() - 1.6, 22.2);
            double const radius = 2.2;

            painter->drawRoundedRect(boundary, radius, radius);
        } else {
            QRectF boundary(-diam, -diam, 2.0 * diam + geom.width(), 23);
            double const radius = 3;

            painter->drawRoundedRect(boundary, radius, radius);
        }


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
