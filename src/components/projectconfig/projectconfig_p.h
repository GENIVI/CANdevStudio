#ifndef PROJECTCONFIG_P_H
#define PROJECTCONFIG_P_H

#include "flowviewwrapper.h"
#include "modeltoolbutton.h"
#include "ui_projectconfig.h"
#include <QtWidgets/QPushButton>
#include <candevice/candevicemodel.h>
#include <canrawsender/canrawsendermodel.h>
#include <canrawview/canrawviewmodel.h>
#include <log.h>
#include <modelvisitor.h> // apply_model_visitor
#include <nodes/Node>

namespace Ui {
class ProjectConfigPrivate;
}

class ProjectConfigPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProjectConfig)

public:
    ProjectConfigPrivate(ProjectConfig* q)
        : _graphView(new FlowViewWrapper(&_graphScene))
        , _ui(std::make_unique<Ui::ProjectConfigPrivate>())
        , q_ptr(q)
    {
        auto& modelRegistry = _graphScene.registry();
        modelRegistry.registerModel<CanDeviceModel>();
        modelRegistry.registerModel<CanRawSenderModel>();
        modelRegistry.registerModel<CanRawViewModel>();

        connect(&_graphScene, &QtNodes::FlowScene::nodeCreated, this, &ProjectConfigPrivate::nodeCreatedCallback);
        connect(&_graphScene, &QtNodes::FlowScene::nodeDeleted, this, &ProjectConfigPrivate::nodeDeletedCallback);
        connect(&_graphScene, &QtNodes::FlowScene::nodeDoubleClicked, this,
            &ProjectConfigPrivate::nodeDoubleClickedCallback);

        _ui->setupUi(this);
        _ui->layout->addWidget(_graphView);
    }

    ~ProjectConfigPrivate()
    {
    }

    QByteArray save() const
    {
        return _graphScene.saveToMemory();
    }

    void load(const QByteArray& data)
    {
        return _graphScene.loadFromMemory(data);
    }

    void clearGraphView()
    {
        return _graphScene.clearScene();
    };

    void nodeCreatedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        Q_Q(ProjectConfig);

        apply_model_visitor(*dataModel, [this](CanRawViewModel& m) { handleWidgetCreation(m.getComponent()); },
            [this, dataModel, q](CanRawSenderModel& m) { handleWidgetCreation(m.getComponent()); },
            [this](CanDeviceModel&) {});
    }

    void nodeDeletedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        apply_model_visitor(*dataModel,
            [this, dataModel](CanRawViewModel& m) { handleWidgetDeletion(m.getComponent().getMainWidget()); },
            [this, dataModel](CanRawSenderModel& m) { handleWidgetDeletion(m.getComponent().getMainWidget()); },
            [this](CanDeviceModel&) {});
    }

    void nodeDoubleClickedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        apply_model_visitor(*dataModel,
            [this, dataModel](CanRawViewModel& m) { handleWidgetShowing(m.getComponent().getMainWidget()); },
            [this, dataModel](CanRawSenderModel& m) { handleWidgetShowing(m.getComponent().getMainWidget()); },
            [this](CanDeviceModel&) {});
    }

private:
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

    QtNodes::FlowScene _graphScene;
    FlowViewWrapper* _graphView;
    std::unique_ptr<Ui::ProjectConfigPrivate> _ui;
    ProjectConfig* q_ptr;
};
#endif // PROJECTCONFIG_P_H
