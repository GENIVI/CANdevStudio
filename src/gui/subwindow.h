#ifndef __SUBWINDOW_H
#define __SUBWINDOW_H

#include <QMdiSubWindow>
#include <QPushButton>
#include <QSize>
#include <log.h>

struct SubWindow : public QMdiSubWindow {
    SubWindow(QWidget* parent)
        : QMdiSubWindow(parent)
    {
        setAttribute(Qt::WA_DeleteOnClose);
        setWindowIcon(QIcon(" "));
    }

    virtual void closeEvent(QCloseEvent* event) override
    {
        cds_debug("On closing");

        // Allow to close the window but prevent deletion of the widget
        setWidget(nullptr);
        QMdiSubWindow::closeEvent(event);
    }

    virtual QSize minimumSizeHint() const override
    {
        return { 200, 100 };
    }

    virtual QSize sizeHint() const override
    {
        QSize size;

        if (parentWidget()) {
            size.setHeight(parentWidget()->size().height() / 2);
            size.setWidth(parentWidget()->size().width() / 2);
        } else {
            cds_error("parentWidget() is NULL!");
        }

        return size;
    }
};

#endif /* !__SUBWINDOW_H */
