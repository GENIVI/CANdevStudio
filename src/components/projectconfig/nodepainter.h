#ifndef __NODEPAINTER_H
#define __NODEPAINTER_H

#include <QPainter>
#include <nodes/Node>

struct NodePainterSettings {
    QColor color1;
    QColor color2;
};

struct NodePainter : public QtNodes::NodePainterDelegate {

    NodePainter() = default;

    NodePainter(const NodePainterSettings& settings)
        : _settings(settings)
    {
    }

    virtual void paint(
        QPainter* painter, QtNodes::NodeGeometry const& geom, QtNodes::NodeDataModel const* model) override
    {
        QtNodes::NodeStyle const& nodeStyle = model->nodeStyle();

        auto color = nodeStyle.NormalBoundaryColor;

        QPen p(color, nodeStyle.PenWidth);
        painter->setPen(p);

        QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(geom.width(), 0));

        gradient.setColorAt(0.0, _settings.color1);
        gradient.setColorAt(0.3, _settings.color1);
        gradient.setColorAt(1.0, _settings.color2);

        painter->setBrush(gradient);

        float diam = nodeStyle.ConnectionPointDiameter;

        QRectF boundary(-diam + 1, -diam + 1, 2.0 * diam + geom.width() - 2, (2.0 * diam + geom.height()) / 3 - 1);

        double const radius = 3.0;

        painter->drawRoundedRect(boundary, radius, radius);

        painter->setPen({Qt::white, 1});
        QFont font({"Arial", 10});
        font.setBold(true);
        painter->setFont(font);
        painter->drawText(-diam + 6, -diam + 16, model->caption());
    }

    private : NodePainterSettings _settings;
};

#endif /* !__NODEPAINTER_H */
