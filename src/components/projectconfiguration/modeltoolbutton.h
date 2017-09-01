#ifndef MODELTOOLBUTTON_H
#define MODELTOOLBUTTON_H
#include <QtWidgets/QToolButton>
struct ModelToolButton : public QToolButton {
    ModelToolButton(QWidget* parent = nullptr)
        : QToolButton(parent)
    {
        setAcceptDrops(true);
    }
};

#endif // MODELTOOLBUTTON_H
