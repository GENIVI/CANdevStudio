#ifndef PROJECTCONFIG_P_H
#define PROJECTCONFIG_P_H

#include "canrawsendermodel.h"
#include "canrawviewmodel.h"
#include "flowviewwrapper.h"
#include "modeltoolbutton.h"
#include "ui_projectconfig.h"
#include <QtWidgets/QPushButton>
#include <QMenu>
#include <log.h>
#include <modelvisitor.h> // apply_model_visitor
#include <nodes/Node>
#include <projectconfig/candevicemodel.h>
#include <propertyeditor/propertyeditordialog.h>

namespace Ui {
class ProjectConfigPrivate;
}

class ProjectConfigPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProjectConfig)

public:
    ProjectConfigPrivate(ProjectConfig* q, QWidget* parent)
        : QWidget(parent), _graphView(new FlowViewWrapper(&_graphScene))
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
        connect(&_graphScene, &QtNodes::FlowScene::nodeContextMenu, this,
            &ProjectConfigPrivate::nodeContextMenuCallback);

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
        auto& component = getComponent(node);

        handleWidgetCreation(component);
    }

    void nodeDeletedCallback(QtNodes::Node& node)
    {
        auto& component = getComponent(node);

        handleWidgetDeletion(component.getMainWidget());
    }

    void nodeDoubleClickedCallback(QtNodes::Node& node)
    {
        auto& component = getComponent(node);

        handleWidgetShowing(component.getMainWidget(), component.mainWidgetDocked());
    }

    void nodeContextMenuCallback(QtNodes::Node& node, const QPointF& pos)
    {
        QMenu contextMenu(tr("Node options"), this);

        QAction action1("Properties", this);
        connect(&action1, &QAction::triggered, [this, &node]()
        {
            auto& component = getComponent(node);
            auto conf = component.getQConfig();

            PropertyEditorDialog e(conf.get());
            e.exec();
            //FIXME: act on accepted
        });

        contextMenu.addAction(&action1);

        auto pos1 = mapToGlobal(_graphView->mapFromScene(pos));
        pos1.setX(pos1.x() + 32); // FIXME: these values are hardcoded and should not be here
        pos1.setY(pos1.y() + 10); //        find the real cause of misalignment of context menu
        contextMenu.exec(pos1);
    }

private:
    void handleWidgetDeletion(QWidget* widget)
    {
        if (!widget)
            return;

        if (widget->parentWidget()) {

            widget->parentWidget()->close();
        } // else path not needed
    }

    void handleWidgetShowing(QWidget* widget, bool docked)
    {
        Q_Q(ProjectConfig);

        if (!widget)
            return;

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

    void handleWidgetCreation(ComponentInterface& view)
    {
        Q_Q(ProjectConfig);

        QWidget* widget = view.getMainWidget();
        connect(q, &ProjectConfig::startSimulation, std::bind(&ComponentInterface::startSimulation, &view));
        connect(q, &ProjectConfig::stopSimulation, std::bind(&ComponentInterface::stopSimulation, &view));
        view.setDockUndockClbk([widget, q] { emit q->handleDock(widget); });
    }

    ComponentInterface& getComponent(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();
        assert(nullptr != dataModel);
        auto iface = dynamic_cast<ComponentModelInterface*>(dataModel);
        auto& component = iface->getComponent();
        return component;
    }

    QtNodes::FlowScene _graphScene;
    FlowViewWrapper* _graphView;
    std::unique_ptr<Ui::ProjectConfigPrivate> _ui;
    ProjectConfig* q_ptr;
};
#endif // PROJECTCONFIG_P_H
