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
        Q_Q(ProjectConfig);

        auto dataModel = node.nodeDataModel();
        assert(nullptr != dataModel);

        auto iface = dynamic_cast<ComponentModelInterface*>(dataModel);
        iface->setCaption(dataModel->caption() + " #" + QString::number(_nodeCnt++));
        iface->handleModelCreation(q);
    }

    void nodeDeletedCallback(QtNodes::Node& node)
    {
        Q_Q(ProjectConfig);
        auto& component = getComponent(node);

        emit q->handleWidgetDeletion(component.mainWidget());
    }

    void nodeDoubleClickedCallback(QtNodes::Node& node)
    {
        Q_Q(ProjectConfig);
        auto& component = getComponent(node);

        emit q->handleWidgetShowing(component.mainWidget(), component.mainWidgetDocked());
    }

    void nodeContextMenuCallback(QtNodes::Node& node, const QPointF& pos)
    {
        QMenu contextMenu(tr("Node options"), this);

        QAction action1("Properties", this);
        connect(&action1, &QAction::triggered, [this, &node]()
        {
            auto& component = getComponent(node);
            auto conf = component.getQConfig();
            conf->setProperty("name", node.nodeDataModel()->caption());

            PropertyEditorDialog e(node.nodeDataModel()->name() + " properties", conf.get());
            if (e.exec() == QDialog::Accepted)
            {
                auto nodeCaption = conf->property("name");
                if(nodeCaption.isValid()) {
                    auto iface = dynamic_cast<ComponentModelInterface*>(node.nodeDataModel());
                    iface->setCaption(nodeCaption.toString());
                    node.nodeGraphicsObject().update();
                }

                component.setConfig(*conf);
                component.configChanged();
            }
        });

        contextMenu.addAction(&action1);

        auto pos1 = mapToGlobal(_graphView->mapFromScene(pos));
        pos1.setX(pos1.x() + 32); // FIXME: these values are hardcoded and should not be here
        pos1.setY(pos1.y() + 10); //        find the real cause of misalignment of context menu
        contextMenu.exec(pos1);
    }

private:
    QtNodes::FlowScene _graphScene;
    FlowViewWrapper* _graphView;
    std::unique_ptr<Ui::ProjectConfigPrivate> _ui;
    int _nodeCnt = 1;
    ProjectConfig* q_ptr;

    ComponentInterface& getComponent(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();
        assert(nullptr != dataModel);

        auto iface = dynamic_cast<ComponentModelInterface*>(dataModel);
        auto& component = iface->getComponent();
        return component;
    }
};
#endif // PROJECTCONFIG_P_H
