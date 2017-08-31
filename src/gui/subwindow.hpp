#ifndef __SUBWINDOW_HPP
#define __SUBWINDOW_HPP

#include <QMdiSubWindow>
#include <QPushButton>

struct SubWindow : public QMdiSubWindow {
    SubWindow(QWidget* widget)
        : QMdiSubWindow()
    {
        setWidget(widget);
    }

    virtual void closeEvent(QCloseEvent* event) override
    {
        cds_debug("On closing");

        // Allow to close the window but prevent deletion of the widget
        setWidget(nullptr);
        QMdiSubWindow::closeEvent(event);
    }
};

#endif /* !__SUBWINDOW_HPP */
