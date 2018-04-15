#ifndef CANRAWFILTERGUIIMPL_H
#define CANRAWFILTERGUIIMPL_H

#include "ui_canrawfilter.h"
#include "canrawfilterguiint.h"
#include <QWidget>

struct CanRawFilterGuiImpl : public CanRawFilterGuiInt {
    CanRawFilterGuiImpl()
        : _ui(new Ui::CanRawFilterPrivate)
        , _widget(new QWidget)
    {
        _ui->setupUi(_widget);
    }

    virtual QWidget* mainWidget()
    {
        return _widget;
    }

private:
    Ui::CanRawFilterPrivate* _ui;
    QWidget* _widget;
};

#endif // CANRAWFILTERGUIIMPL_H
