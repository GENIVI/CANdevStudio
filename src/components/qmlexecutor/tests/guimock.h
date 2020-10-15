#ifndef GUIMOC_H
#define GUIMOC_H

#include <catch.hpp>
#include <context.h>
#include <fakeit.hpp>

#include "gui/qmlexecutorguiint.h"

/**
* @brief fakeit is broken so wrap broken solution to get it working with qt signals
*/
class GuiMock : public QMLExecutorGuiInt
{
    Q_OBJECT
public:
    GuiMock(QObject* parent = nullptr)
        : QMLExecutorGuiInt(parent)
    { }

public:
    void loadQML(const QUrl& url) override
    {
        mock.get().loadQML(url);
    }

    void setModel(CANBusModel* model) override
    {
        mock.get().setModel(model);
    }

    QWidget* mainWidget() override
    {
        return mock.get().mainWidget();
    }

    void updateUIColor() override
    {
        return mock.get().updateUIColor();
    }

public:
    fakeit::Mock<QMLExecutorGuiInt> mock;
};

#endif
