#ifndef __CANLOADPAINTER_H
#define __CANLOADPAINTER_H

#include "nodepainter.h"

struct CanLoadPainter : public NodePainter {

    CanLoadPainter(const QColor& headerColor, const uint8_t& load)
        : NodePainter(headerColor)
        , _load(load)
    {
    }

    virtual void paint(QPainter* painter, QtNodes::NodeGeometry const& geom, QtNodes::NodeDataModel const* model,
        QtNodes::NodeGraphicsObject const& graphicsObject) override
    {
        NodePainter::paint(painter, geom, model, graphicsObject);

        QtNodes::NodeStyle const& nodeStyle = model->nodeStyle();
        int s = 15;

        painter->setPen(QPen(nodeStyle.FontColorFaded, 2));
        painter->setBrush(Qt::NoBrush);

        QFont font({ "Arial", 8 });
        font.setBold(true);
        painter->setFont(font);
        QString val = QString::number(_load) + "%";
        QFontMetrics metrics(font);
        auto rect = metrics.boundingRect(val);

        QPoint p(geom.width() / 2, geom.height() / 2 + 12);
        painter->drawEllipse(p, s, s);

        painter->setPen(nodeStyle.FontColor);
        QPoint t(p.x() - rect.width() / 2 + 1, p.y() + 4);
        painter->drawText(t, val);
    }

private:
    const uint8_t& _load;
};

#endif /* !__CANLOADPAINTER_H */
