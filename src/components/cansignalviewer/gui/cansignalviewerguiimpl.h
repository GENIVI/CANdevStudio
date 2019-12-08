#ifndef CANSIGNALVIEWERGUIIMPL_H
#define CANSIGNALVIEWERGUIIMPL_H

#include "ui_cansignalviewer.h"
#include "cansignalviewerguiint.h"
#include <QWidget>

struct CanSignalViewerGuiImpl : public CanSignalViewerGuiInt {
    CanSignalViewerGuiImpl()
        : _ui(new Ui::CanSignalViewerPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);
    }

    virtual QWidget* mainWidget()
    {
        return _widget;
    }

private:
    Ui::CanSignalViewerPrivate* _ui;
    QWidget* _widget;
};

#endif // CANSIGNALVIEWERGUIIMPL_H
