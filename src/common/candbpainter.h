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

        QtNodes::NodeStyle const& nodeStyle = model->nodeStyle();

        auto config = _component->getQConfig();
        QString colorStr = config->property("color").toString();
        if (colorStr.length() == 0) {
            // Do not draw DB icon if DB is not set
            return;
        }

        QColor gradientColor = QColor(colorStr);

        if (nodeStyle.GradientColor0 == QColor("white")) {
            _nodeColor = _lightColor;
        } else {
            _nodeColor = _darkColor;
        }

        if (_nodeColor != _prevNodeColor) {
            _prevNodeColor = _nodeColor;

            QFile f(":/images/files/images/db_icon.svg");

            if (f.open(QFile::ReadOnly | QFile::Text)) {
                QString tmp = QString::fromLatin1(f.readAll());
                tmp = tmp.replace("fill:#000000", QString("fill:%1").arg(_nodeColor.name(QColor::HexRgb)));

                _svg.load(tmp.toLatin1());
            } else {
                cds_error("Failed to load DB icon from resources");
            }
        }

        QRectF svgRect((geom.width() - _s) / 2, (geom.height() - _s) / 2 + 12, _s, _s);
        qreal bgMod = _s * 0.6;
        QRectF bgRect((geom.width() - _s - bgMod) / 2, (geom.height() - _s - bgMod) / 2 + 12, _s + bgMod, _s + bgMod);
        qreal grMod = bgMod + (_s * 0.6);
        QRectF grRect((geom.width() - _s - grMod) / 2, (geom.height() - _s - grMod) / 2 + 12, _s + grMod, _s + grMod);

        QRadialGradient gr(grRect.x() + grRect.width() / 2, grRect.y() + grRect.width() / 2, grRect.width() / 2);
        gr.setColorAt(0, gradientColor);
        if (_s < 20) {
            gr.setColorAt(0.7, gradientColor);
        } else {
            gr.setColorAt(0.4, gradientColor);
        }
        gr.setColorAt(1, nodeStyle.GradientColor0);
        painter->fillRect(grRect, gr);

        painter->setPen(QPen(_nodeColor, _s * 0.1));
        painter->setBrush(nodeStyle.GradientColor0);
        painter->drawEllipse(bgRect);

        _svg.render(painter, svgRect);
    }

private:
    const ComponentInterface* _component;
    QColor _nodeColor;
    QColor _prevNodeColor;
    QSvgRenderer _svg;
    qreal _s;
    const QColor _lightColor{ "#a5a5a5" };
    const QColor _darkColor{ "#a0a0a0" };
};

#endif /* !__CANDBPAINTER_H */
