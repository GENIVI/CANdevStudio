#ifndef __SUBWINDOW_H
#define __SUBWINDOW_H

#include <QMdiSubWindow>
#include <QPushButton>

struct SubWindow : public QMdiSubWindow {
    virtual void closeEvent(QCloseEvent* event) override
    {
        cds_debug("On closing");

        // Allow to close the window but prevent deletion of the widget
        setWidget(nullptr);
        QMdiSubWindow::closeEvent(event);
    }
};

#endif /* !__SUBWINDOW_H */
