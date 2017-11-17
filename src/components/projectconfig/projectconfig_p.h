#ifndef PROJECTCONFIG_P_H
#define PROJECTCONFIG_P_H

#include "canrawsendermodel.h"
#include "canrawviewmodel.h"
#include "flowviewwrapper.h"
#include "modeltoolbutton.h"
#include "ui_projectconfig.h"
#include <QMenu>
#include <QtWidgets/QPushButton>
#include <log.h>
#include <modelvisitor.h> // apply_model_visitor
#include <nodes/Node>
#include <projectconfig/candevicemodel.h>
#include <propertyeditor/propertyeditordialog.h>
#include "iconlabel.h"

namespace Ui {
class ProjectConfigPrivate;
}

class ProjectConfigPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProjectConfig)

public:
    ProjectConfigPrivate(ProjectConfig* q, QWidget* parent)
        : QWidget(parent)
        , _graphView(new FlowViewWrapper(&_graphScene))
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
        connect(
            &_graphScene, &QtNodes::FlowScene::nodeContextMenu, this, &ProjectConfigPrivate::nodeContextMenuCallback);

        _ui->setupUi(this);
        _ui->layout->addWidget(_graphView);

        connect(_ui->pbDeviceLayer, &QPushButton::toggled, _ui->deviceWidget, &QWidget::setVisible);
        _ui->pbDeviceLayer->setChecked(true);
        connect(_ui->pbRawLayer, &QPushButton::toggled, _ui->rawWidget, &QWidget::setVisible);
        _ui->pbRawLayer->setChecked(true);
    }

    void addModelIcons()
    {
        QColor bgColor;

        if(_darkMode) {
            bgColor = QColor(94, 94, 94);
        } else {
            bgColor = QColor(255, 255, 255);
        }

        QLayoutItem *item;
        while ((item = _ui->deviceWidget->layout()->takeAt(0)) != nullptr)
        {
            delete item;
        }
        while ((item = _ui->rawWidget->layout()->takeAt(0)) != nullptr)
        {
            delete item;
        }

        _ui->deviceWidget->layout()->addWidget(new IconLabel("CanDevice", CanDeviceModel::headerColor1(), CanDeviceModel::headerColor2(), bgColor));
        _ui->rawWidget->layout()->addWidget(new IconLabel("CanRawSender", CanRawSenderModel::headerColor1(), CanRawSenderModel::headerColor2(), bgColor));
        _ui->rawWidget->layout()->addWidget(new IconLabel("CanRawView", CanRawViewModel::headerColor1(), CanRawViewModel::headerColor2(), bgColor));
    }

    ~ProjectConfigPrivate() {}

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

        auto& iface = getComponentModel(node);
        iface.handleModelCreation(q);
        iface.setColorMode(_darkMode);

        if (!iface.restored()) {
            iface.setCaption(node.nodeDataModel()->caption() + " #" + QString::number(_nodeCnt));
        }

        node.nodeGraphicsObject().setOpacity(node.nodeDataModel()->nodeStyle().Opacity);
        node.nodeGraphicsObject().update();

        _nodeCnt++;
    }

    void nodeDeletedCallback(QtNodes::Node& node)
    {
        Q_Q(ProjectConfig);
        auto& component = getComponent(node);

        emit q->handleWidgetDeletion(component.mainWidget());
    }

    void nodeDoubleClickedCallback(QtNodes::Node& node)
    {
        auto& component = getComponent(node);

        if (component.mainWidget() != nullptr) {
            openWidget(node);
        } else {
            openProperties(node);
        }
    }

    void nodeContextMenuCallback(QtNodes::Node& node, const QPointF& pos)
    {
        auto& component = getComponent(node);
        QMenu contextMenu(tr("Node options"), this);

        QAction actionOpen("Open", this);
        connect(&actionOpen, &QAction::triggered, [this, &node]() { openWidget(node); });

        QAction actionProperties("Properties", this);
        connect(&actionProperties, &QAction::triggered, [this, &node]() { openProperties(node); });

        QAction actionDelete("Delete", this);
        connect(&actionDelete, &QAction::triggered, [this, &node]() { _graphScene.removeNode(node); });

        if (component.mainWidget() != nullptr) {
            contextMenu.addAction(&actionOpen);
            contextMenu.addAction(&actionProperties);
            contextMenu.setDefaultAction(&actionOpen);
        } else {
            contextMenu.addAction(&actionProperties);
            contextMenu.setDefaultAction(&actionProperties);
        }

        contextMenu.addAction(&actionDelete);

        auto pos1 = mapToGlobal(_graphView->mapFromScene(pos));
        pos1.setX(pos1.x() + 32); // FIXME: these values are hardcoded and should not be here
        pos1.setY(pos1.y() + 10); //        find the real cause of misalignment of context menu
        contextMenu.exec(pos1);
    }

    void updateNodeStyle(bool darkMode)
    {
        _darkMode = darkMode;

        _graphScene.iterateOverNodes([this](QtNodes::Node* node){
            auto &iface = getComponentModel(*node);
            iface.setColorMode(_darkMode);
            node->nodeGraphicsObject().update();
        });
    }

private:
    QtNodes::FlowScene _graphScene;
    FlowViewWrapper* _graphView;
    std::unique_ptr<Ui::ProjectConfigPrivate> _ui;
    int _nodeCnt = 1;
    ProjectConfig* q_ptr;
    bool _darkMode;

    void openWidget(QtNodes::Node& node)
    {
        Q_Q(ProjectConfig);
        auto& component = getComponent(node);

        emit q->handleWidgetShowing(component.mainWidget(), component.mainWidgetDocked());
    }

    void openProperties(QtNodes::Node& node)
    {
        auto& component = getComponent(node);
        auto conf = component.getQConfig();
        conf->setProperty("name", node.nodeDataModel()->caption());

        PropertyEditorDialog e(node.nodeDataModel()->name() + " properties", *conf.get());
        if (e.exec() == QDialog::Accepted) {
            conf = e.properties();
            auto nodeCaption = conf->property("name");
            if (nodeCaption.isValid()) {
                auto& iface = getComponentModel(node);
                iface.setCaption(nodeCaption.toString());
                node.nodeGraphicsObject().update();
            }

            component.setConfig(*conf);
            component.configChanged();
        }
    }

    ComponentInterface& getComponent(QtNodes::Node& node)
    {
        auto &iface = getComponentModel(node);
        auto& component = iface.getComponent();
        return component;
    }

    ComponentModelInterface& getComponentModel(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();
        assert(nullptr != dataModel);

        auto iface = dynamic_cast<ComponentModelInterface*>(dataModel);
        return *iface;
    }
};
#endif // PROJECTCONFIG_P_H
