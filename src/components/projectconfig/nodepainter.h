#ifndef __NODEPAINTER_H
#define __NODEPAINTER_H

#include <QPainter>
#include <nodes/Node>
#include <QtWidgets/QGraphicsEffect>

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
        painter->setPen(p);
        painter->setBrush(QBrush(_headerColor1));

        float diam = nodeStyle.ConnectionPointDiameter;

        QRectF boundary(-diam + 1, -diam + 1, 2.0 * diam + geom.width() - 2, 22);

        double const radius = 3.0;

        painter->drawRoundedRect(boundary, radius, radius);

        painter->setPen({Qt::white, 1});
        QFont font({"Arial", 10});
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(-diam + 6, -diam + 16, model->caption());
    }

private:
    const QColor _headerColor1;
    const QColor _headerColor2;
};

#endif /* !__NODEPAINTER_H */
