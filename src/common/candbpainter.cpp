#include <QFile>
#include <componentinterface.h>
#include <log.h>
#include "candbpainter.h"

CanDbPainter::CanDbPainter(const QColor& headerColor, const ComponentInterface* component, qreal size)
    : NodePainter(headerColor)
    , _component(component)
    , _s(size)
    , _lightColor("#a5a5a5")
    , _darkColor("#a0a0a0")
{
}

void CanDbPainter::paint(QPainter* painter, QtNodes::NodeGeometry const& geom, QtNodes::NodeDataModel const* model,
    QtNodes::NodeGraphicsObject const& graphicsObject)
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
    qreal innerMod = _s * 0.6;
    QRectF innerEllipseRect(
        (geom.width() - _s - innerMod) / 2, (geom.height() - _s - innerMod) / 2 + 12, _s + innerMod, _s + innerMod);
    qreal outerMod = innerMod * 1.5;
    QRectF outerEllipseRect(
        (geom.width() - _s - outerMod) / 2, (geom.height() - _s - outerMod) / 2 + 12, _s + outerMod, _s + outerMod);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(gradientColor, _s * 0.1));

    painter->drawEllipse(outerEllipseRect);
    painter->drawEllipse(innerEllipseRect);

    _svg.render(painter, svgRect);
}

