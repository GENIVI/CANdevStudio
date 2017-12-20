#ifndef __ICONLABEL_H
#define __ICONLABEL_H

#include <QLabel>
#include <QPainter>

class IconLabel : public QLabel
{
public:
    IconLabel(const QString &name, const QColor &headerColor1, const QColor &, const QColor &bgColor)
    {        
        QPixmap pix(_width, _height);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);

        auto color = bgColor;
        QPen p(color, 0);
        painter.setPen(p);

        painter.setRenderHint(QPainter::Antialiasing);

        painter.setBrush(QBrush(bgColor));
        QRectF boundary(0, 0, pix.width(), pix.height());
        painter.drawRoundedRect(boundary, _radius, _radius);
        painter.setBrush(QBrush(headerColor1));

        QRectF boundary2(1, 1, pix.width()-2, pix.height()/2 - 2);
        painter.drawRoundedRect(boundary2, _radius, _radius);

        painter.setPen({Qt::white, 1});
        QFont font({"Arial", 8});
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(8, 16, name);

        setMinimumSize(_width, _height);
        setMaximumSize(_width, _height);
        setObjectName(name);
        setPixmap(pix);

        setProperty("type", "IconLabel");
    }

private:
    const int _width = 140;
    const int _height = 50;
    const double _radius = 5.0;
};

#endif /* !__ICONLABEL_H */
