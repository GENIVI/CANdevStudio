#include <QtWidgets>

#include "dragwidget.h"
#include <log.h>


DragWidget::DragWidget(QWidget *parent)
    : QFrame(parent)
{
    setAcceptDrops(true);
}

void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void DragWidget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
    if (!child) {
        return;
    }

    QPixmap pixmap = *child->pixmap();
    if (!pixmap)
        return;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << QPoint(event->pos() - child->pos()) << child->objectName();

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("CANdevStudio/x-dnditemdata", itemData);

    QPoint cursor = QPoint(10,10);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot((event->pos() - child->pos())/2);
    cds_debug("QPoint event x:{} y:{}", event->pos().x(), event->pos().y());
    cds_debug("QPoint child x:{} y:{}", child->pos().x(), child->pos().y());
    cds_debug("QPoint       x:{} y:{}", (event->pos().x() - child->pos().x()), (event->pos().y() - child->pos().y()));

    QPixmap tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
    painter.end();

    child->setPixmap(tempPixmap);

    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
        child->close();
    } else {
        child->show();
        child->setPixmap(pixmap);
    }
}
