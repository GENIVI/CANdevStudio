#ifndef __ICONLABEL_H
#define __ICONLABEL_H

#include <QLabel>
#include <QLayoutItem>
#include <QPainter>
#include <QVariant>

class IconLabel : public QLabel {
public:
    IconLabel(const QString& name, const QColor& headerColor, const QColor& bgColor)
    {
        QPixmap pix(_width, _height);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);

        painter.setPen(Qt::NoPen);

        painter.setRenderHint(QPainter::Antialiasing);

        painter.setBrush(QBrush(bgColor));
        QRectF boundary(0, 0, pix.width(), pix.height());
        painter.drawRoundedRect(boundary, _radius, _radius);
        painter.setBrush(QBrush(headerColor));

        QRectF boundary2(0, 0, pix.width(), _labelHeight);
        painter.drawRoundedRect(boundary2, _radius, _radius);

        QRectF boundary3(0, _labelHeight / 2, pix.width(), _labelHeight / 2);
        painter.drawRect(boundary3);

        painter.setPen({ Qt::white, 1 });
        QFont font({ "Arial", 8 });
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(6, 15, name);

        setMinimumSize(_width, _height);
        setMaximumSize(_width, _height);
        setObjectName(name);
        setPixmap(pix);

        setProperty("type", "IconLabel");
    }

    QSize minimumSizeHint() const override
    {
        return { _width, _height };
    }

private:
    const int _width = 140;
    const int _height = 48;
    const int _labelHeight = 22;
    const double _radius = 3.0;
};

#endif /* !__ICONLABEL_H */
