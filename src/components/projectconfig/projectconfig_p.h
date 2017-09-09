#ifndef PROJECTCONFIG_P_H
#define PROJECTCONFIG_P_H

#include "flowviewwrapper.h"
#include "modeltoolbutton.h"
#include "modelvisitor.h" // apply_model_visitor
#include "ui_projectconfig.h"
#include <cassert> // assert

#include "log.h"

#include <QAction>

#include <QPushButton>

#include <candevice/candevicemodel.h>
#include <canrawsender/canrawsendermodel.h>
#include <canrawview/canrawviewmodel.h>

namespace Ui {
class ProjectConfigPrivate;
}

class ProjectConfigPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProjectConfig)

public:
    ProjectConfigPrivate(ProjectConfig* q)
        : ui(std::make_unique<Ui::ProjectConfigPrivate>())
        , q_ptr(q)
    {
        auto modelRegistry = std::make_shared<QtNodes::DataModelRegistry>();
        modelRegistry->registerModel<CanDeviceModel>();
        modelRegistry->registerModel<CanRawSenderModel>();
        modelRegistry->registerModel<CanRawViewModel>();

        graphScene = std::make_shared<QtNodes::FlowScene>(modelRegistry);
        graphView = new FlowViewWrapper(graphScene.get());

        connect(graphScene.get(), &QtNodes::FlowScene::nodeCreated, this,
            &ProjectConfigPrivate::nodeCreatedCallback);
        connect(graphScene.get(), &QtNodes::FlowScene::nodeDeleted, this,
            &ProjectConfigPrivate::nodeDeletedCallback);
        connect(graphScene.get(), &QtNodes::FlowScene::nodeDoubleClicked, this,
            &ProjectConfigPrivate::nodeDoubleClickedCallback);

        ui->setupUi(this);
        ui->layout->addWidget(graphView);
        // ui->layout->toolbar->addWidget(.....);
    }

    ~ProjectConfigPrivate()
    {
    }

    std::unique_ptr<Ui::ProjectConfigPrivate> ui;

private:
    std::shared_ptr<QtNodes::FlowScene> graphScene;
    FlowViewWrapper* graphView; // FIXME

    void handleWidgetDeletion(QWidget* widget)
    {
        assert(nullptr != widget);
        if (widget->parentWidget()) {

            widget->parentWidget()->close();
        } // else path not needed
    }

    void handleWidgetShowing(QWidget* widget)
    {
        assert(nullptr != widget);
        Q_Q(ProjectConfig);
        bool docked = false;
        // TODO: Temporary solution. To be changed once MainWindow is refactored
        QPushButton* undockButton = widget->findChild<QPushButton*>("pbDockUndock");
        if (undockButton) {
            docked = !undockButton->isChecked();
        } else {
            cds_debug("Undock button for '{}' widget not found", widget->windowTitle().toStdString());
        }

        // Add widget to MDI area when showing for the first time
        // Widget will be also added to MDI area after closing it in undocked state
        if (!widget->isVisible() && docked) {
            cds_debug("Adding '{}' widget to MDI", widget->windowTitle().toStdString());
            emit q->componentWidgetCreated(widget);
        }

        if (widget->parentWidget()) {
            cds_debug("Widget is a part of MDI");
            widget->hide();
            widget->show();
        } else {
            cds_debug("Widget not a part of MDI");
            widget->show();
            widget->activateWindow();
        }
    }

    template <typename T> void handleWidgetCreation(T& view)
    {
        Q_Q(ProjectConfig);

        QWidget* widget = view.getMainWidget();
        connect(q, &ProjectConfig::startSimulation, &view, &T::startSimulation);
        connect(q, &ProjectConfig::stopSimulation, &view, &T::stopSimulation);
        connect(&view, &T::dockUndock, this, [this, widget, q] { emit q->handleDock(widget); });
    }

public:
    QByteArray save() const
    {
        return graphScene->saveToMemory();
    }

    void load(const QByteArray& data)
    {
        return graphScene->loadFromMemory(data);
    }

    void clearGraphView()
    {
        return graphScene->clearScene();
    };

    void nodeCreatedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        Q_Q(ProjectConfig);

        apply_model_visitor(*dataModel, [this](CanRawViewModel& m) { handleWidgetCreation(m.canRawView); },
            [this, dataModel, q](CanRawSenderModel& m) { handleWidgetCreation(m.canRawSender); },
            [this](CanDeviceModel&) {});
    }

    void nodeDeletedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        apply_model_visitor(*dataModel,
            [this, dataModel](CanRawViewModel& m) { handleWidgetDeletion(m.canRawView.getMainWidget()); },
            [this, dataModel](CanRawSenderModel& m) { handleWidgetDeletion(m.canRawSender.getMainWidget()); },
            [this](CanDeviceModel&) {});
    }

    void nodeDoubleClickedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        apply_model_visitor(*dataModel,
            [this, dataModel](CanRawViewModel& m) { handleWidgetShowing(m.canRawView.getMainWidget()); },
            [this, dataModel](CanRawSenderModel& m) { handleWidgetShowing(m.canRawSender.getMainWidget()); },
            [this](CanDeviceModel&) {});
    }

private:
    ProjectConfig* q_ptr;
};
#endif // PROJECTCONFIG_P_H
