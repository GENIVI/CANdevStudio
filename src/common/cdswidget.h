#ifndef CDSWIDGET_H
#define CDSWIDGET_H
#include <QWidget>
#include <QCloseEvent>

struct CDSWidget : public QWidget {
    void closeEvent(QCloseEvent* e)
    {
        if (parentWidget()) {
            parentWidget()->hide();
        } else {
            hide();
        }
        e->ignore();
    }
};

#endif // CDSWIDGET_H
