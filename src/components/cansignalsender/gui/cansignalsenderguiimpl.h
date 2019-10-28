#ifndef CANSIGNALSENDERGUIIMPL_H
#define CANSIGNALSENDERGUIIMPL_H

#include "ui_cansignalsender.h"
#include "cansignalsenderguiint.h"
#include <QWidget>

struct CanSignalSenderGuiImpl : public CanSignalSenderGuiInt {
    CanSignalSenderGuiImpl()
        : _ui(new Ui::CanSignalSenderPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);
    }

    virtual QWidget* mainWidget()
    {
        return _widget;
    }

private:
    Ui::CanSignalSenderPrivate* _ui;
    QWidget* _widget;
};

#endif // CANSIGNALSENDERGUIIMPL_H
