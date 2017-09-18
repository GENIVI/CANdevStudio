#include <QtWidgets>

#include "dragwidget.h"
#include <log.h>


DragWidget::DragWidget(QWidget *parent)
    : QFrame(parent)
{
    cds_debug("DragWidget::DragWidget");
    setAcceptDrops(true);
}

void DragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    cds_debug("DragWidget::dragEnterEvent");
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

void DragWidget::dragMoveEvent(QDragMoveEvent *event)
{
    cds_debug("DragWidget::dragMoveEvent");
//    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }
//    } else {
//        event->ignore();
//    }
}

void DragWidget::dropEvent(QDropEvent *event)
{
    cds_debug("DragWidget::dropEvent");
//    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
//        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
//        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

//        QPixmap pixmap;
//        QPoint offset;
//        dataStream >> pixmap >> offset;

//        QLabel *newIcon = new QLabel(this);
//        newIcon->setPixmap(pixmap);
//        newIcon->move(event->pos() - offset);
//        newIcon->show();
//        newIcon->setAttribute(Qt::WA_DeleteOnClose);

//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }
//    } else {
//        event->ignore();
//    }
}

void DragWidget::mousePressEvent(QMouseEvent *event)
{
    cds_debug("DragWidget::mousePressEvent");
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
    if (!child) {
        cds_debug("DragWidget::mousePressEvent return");
        return;
    }

    // QPixmap pixmap = QPixmap("/home/dasa/Downloads/car.png");
    QPixmap pixmap = *child->pixmap();
    if (!pixmap)
        return;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos());

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());

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
