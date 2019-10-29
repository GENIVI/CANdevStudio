#ifndef __CANDBPAINTER_H
#define __CANDBPAINTER_H

#include "nodepainter.h"
#include <QFile>
#include <QSvgRenderer>
#include <log.h>

struct CanDbPainter : public NodePainter {

    CanDbPainter(const QColor& headerColor, const ComponentInterface* component, qreal size)
        : NodePainter(headerColor)
        , _component(component)
        , _s(size)
    {
    }

    virtual void paint(QPainter* painter, QtNodes::NodeGeometry const& geom, QtNodes::NodeDataModel const* model,
        QtNodes::NodeGraphicsObject const& graphicsObject) override
    {
        NodePainter::paint(painter, geom, model, graphicsObject);

        auto config = _component->getQConfig();
        QColor color = QColor(config->property("color").toString());

        if (color != _color) {
            _color = color;

            QFile f(":/images/files/images/db_icon.svg");

            if (f.open(QFile::ReadOnly | QFile::Text)) {
                QString tmp = QString::fromLatin1(f.readAll());
                tmp = tmp.replace("fill:#000000", QString("fill:%1").arg(color.name(QColor::HexRgb)));

                _svg.load(tmp.toLatin1());
            } else {
                cds_error("Failed to load DB icon from resources");
            }
        }

        QRectF rect((geom.width() - _s)/2, (geom.height() - _s)/2 +12, _s, _s);
        _svg.render(painter, rect);
    }

private:
    const ComponentInterface* _component;
    QColor _color;
    QSvgRenderer _svg;
    qreal _s;
};

#endif /* !__CANDBPAINTER_H */
