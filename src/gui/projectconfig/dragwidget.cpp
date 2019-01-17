#include <QtWidgets>

#include "dragwidget.h"
#include <log.h>

DragWidget::DragWidget(QWidget* parent)
    : QFrame(parent)
{
    setAcceptDrops(true);
}

void DragWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat("CANdevStudio/x-dnditemdata")) {
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

void DragWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget* el = childAt(event->pos());

    if (!el) {
        return;
    }

    QVariant type = el->property("type");
    if (!type.isValid() || type.toString() != "IconLabel") {
        cds_debug("Dragging disabled for {}", el->objectName().toStdString());
        return;
    }

    QLabel* child = static_cast<QLabel*>(childAt(event->pos()));
    if (!child) {
        return;
    }

    const QPixmap* pixmap = child->pixmap();
    if (!pixmap)
        return;

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << QPoint(event->pos() - child->pos()) << child->objectName();

    QMimeData* mimeData = new QMimeData;
    mimeData->setData("CANdevStudio/x-dnditemdata", itemData);

    QDrag* drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(*pixmap);
    drag->setHotSpot({ 0, 0 });

    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
        child->close();
    } else {
        child->show();
        child->setPixmap(*pixmap);
    }
}
