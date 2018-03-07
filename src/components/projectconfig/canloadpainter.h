#ifndef __CANLOADPAINTER_H
#define __CANLOADPAINTER_H

#include "nodepainter.h"

struct CanLoadPainter : public NodePainter {

    CanLoadPainter() = default;

    CanLoadPainter(const QColor& headerColor1, const QColor& headerColor2, const uint8_t& load)
        : NodePainter(headerColor1, headerColor2)
        , _load(load)
    {
    }

    virtual void paint(QPainter* painter, QtNodes::NodeGeometry const& geom, QtNodes::NodeDataModel const* model,
        QtNodes::NodeGraphicsObject const& graphicsObject) override
    {
        NodePainter::paint(painter, geom, model, graphicsObject);

        int s = 20;

        //QRadialGradient radialGrad(QPointF(geom.width(), 0), s);
        //radialGrad.setColorAt(0, Qt::red);
        //radialGrad.setColorAt(0.5, Qt::blue);
        //radialGrad.setColorAt(1, Qt::green);
        //radialGrad.setSpread(QGradient::PadSpread);

        //painter->setPen(QPen(Qt::black, 5));
        //painter->setBrush(radialGrad);

        QPoint p(geom.width(), 0);
        painter->drawEllipse(p, s, s);
        painter->drawText(p, QString::number(_load));
    }

private:
    const uint8_t &_load;
};

#endif /* !__CANLOADPAINTER_H */
