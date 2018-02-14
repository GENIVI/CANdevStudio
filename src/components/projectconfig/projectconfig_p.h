#ifndef PROJECTCONFIG_P_H
#define PROJECTCONFIG_P_H

#include "canrawsendermodel.h"
#include "canrawviewmodel.h"
#include "flowviewwrapper.h"
#include "iconlabel.h"
#include "modeltoolbutton.h"
#include "pcimpl.h"
#include "ui_projectconfig.h"
#include <QMenu>
#include <QtWidgets/QPushButton>
#include <candevicemodel.h>
#include <log.h>
#include <modelvisitor.h> // apply_model_visitor
#include <nodes/Node>
#include <propertyeditordialog.h>

namespace Ui {
class ProjectConfigPrivate;
}

class ProjectConfigPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProjectConfig)

public:
    ProjectConfigPrivate(ProjectConfig* q, QWidget* parent, ProjectConfigCtx&& ctx = ProjectConfigCtx(new PCImpl()))
        : QWidget(parent)
        , _ctx(std::move(ctx))
        , _pcInt(_ctx.get<PCInterface>())
        , _graphView(new FlowViewWrapper(&_graphScene))
        , _ui(std::make_unique<Ui::ProjectConfigPrivate>())
        , q_ptr(q)
    {
        auto& modelRegistry = _graphScene.registry();
        modelRegistry.registerModel<CanDeviceModel>();
        modelRegistry.registerModel<CanRawSenderModel>();
        modelRegistry.registerModel<CanRawViewModel>();

        _pcInt.setNodeCreatedCallback(
            &_graphScene, std::bind(&ProjectConfigPrivate::nodeCreatedCallback, this, std::placeholders::_1));
        _pcInt.setNodeDeletedCallback(
            &_graphScene, std::bind(&ProjectConfigPrivate::nodeDeletedCallback, this, std::placeholders::_1));
        _pcInt.setNodeDoubleClickedCallback(
            &_graphScene, std::bind(&ProjectConfigPrivate::nodeDoubleClickedCallback, this, std::placeholders::_1));
        _pcInt.setNodeContextMenuCallback(&_graphScene,
            std::bind(
                &ProjectConfigPrivate::nodeContextMenuCallback, this, std::placeholders::_1, std::placeholders::_2));

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

        if (_darkMode) {
            bgColor = QColor(94, 94, 94);
        } else {
            bgColor = QColor(255, 255, 255);
        }

        QLayoutItem* item;
        while ((item = _ui->deviceWidget->layout()->takeAt(0)) != nullptr) {
            delete item;
        }
        while ((item = _ui->rawWidget->layout()->takeAt(0)) != nullptr) {
            delete item;
        }

        _ui->deviceWidget->layout()->addWidget(
            new IconLabel("CanDevice", CanDeviceModel::headerColor1(), CanDeviceModel::headerColor2(), bgColor));
        _ui->rawWidget->layout()->addWidget(new IconLabel(
            "CanRawSender", CanRawSenderModel::headerColor1(), CanRawSenderModel::headerColor2(), bgColor));
        _ui->rawWidget->layout()->addWidget(
            new IconLabel("CanRawView", CanRawViewModel::headerColor1(), CanRawViewModel::headerColor2(), bgColor));
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

        if (!iface.restored()) {
            iface.setCaption(node.nodeDataModel()->caption() + " #" + QString::number(_nodeCnt));
        }

        // For some reason QWidget title is being set to name instead of caption.
        // TODO: investigate why
        iface.setCaption(node.nodeDataModel()->caption());

        if (iface.hasSeparateThread()) {
            // Thread will be deleted during node deletion
            iface.handleModelCreation(q, new QThread());
        } else {
            iface.handleModelCreation(q);
        }

        iface.setColorMode(_darkMode);

        node.nodeGraphicsObject().setOpacity(node.nodeDataModel()->nodeStyle().Opacity);
        addShadow(node);
        node.nodeGraphicsObject().update();

        _nodeCnt++;

        cds_debug("Node '{}' created", node.nodeDataModel()->caption().toStdString());
    }

    void nodeDeletedCallback(QtNodes::Node& node)
    {
        Q_Q(ProjectConfig);
        auto& component = getComponent(node);

        cds_debug("Node '{}' deleted", node.nodeDataModel()->caption().toStdString());

        emit q->handleWidgetDeletion(component.mainWidget());
    }

    void nodeDoubleClickedCallback(QtNodes::Node& node)
    {
        auto& component = getComponent(node);

        cds_debug("Node '{}' double clicked", node.nodeDataModel()->caption().toStdString());

        if (component.mainWidget() != nullptr) {
            openWidget(node);
        } else {
            if (!_simStarted) {
                openProperties(node);
            }
        }
    }

    void nodeContextMenuCallback(QtNodes::Node& node, const QPointF& pos)
    {
        auto& component = getComponent(node);
        cds_debug("Node '{}' context menu", node.nodeDataModel()->caption().toStdString());

        QMenu contextMenu(tr("Node options"), this);

        QAction actionOpen("Open", this);
        connect(&actionOpen, &QAction::triggered, [this, &node]() { openWidget(node); });

        QAction actionProperties("Properties", this);
        connect(&actionProperties, &QAction::triggered, [this, &node]() { openProperties(node); });

        if (_simStarted) {
            actionProperties.setDisabled(true);
        }

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

        _graphScene.iterateOverNodes([this](QtNodes::Node* node) {
            auto& iface = getComponentModel(*node);
            iface.setColorMode(_darkMode);
            node->nodeGraphicsObject().update();
            addShadow(*node);
        });
    }

private:
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
            auto& iface = getComponentModel(node);
            conf = e.properties();
            auto nodeCaption = conf->property("name");
            if (nodeCaption.isValid()) {
                iface.setCaption(nodeCaption.toString());
                node.nodeGraphicsObject().update();
            }

            component.setConfig(*conf);
            component.configChanged();
        }
    }

    ComponentInterface& getComponent(QtNodes::Node& node)
    {
        auto& iface = getComponentModel(node);
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

    void addShadow(QtNodes::Node& node)
    {
        auto const& nodeStyle = node.nodeDataModel()->nodeStyle();
        auto effect = new QGraphicsDropShadowEffect;

        effect->setOffset(4, 4);
        effect->setBlurRadius(20);
        effect->setColor(nodeStyle.ShadowColor);
        node.nodeGraphicsObject().setGraphicsEffect(effect);
    }

public:
    bool _simStarted{ false };

private:
    ProjectConfigCtx _ctx;
    PCInterface& _pcInt;
    QtNodes::FlowScene _graphScene;
    FlowViewWrapper* _graphView;
    std::unique_ptr<Ui::ProjectConfigPrivate> _ui;
    int _nodeCnt = 1;
    ProjectConfig* q_ptr;
    bool _darkMode;
};
#endif // PROJECTCONFIG_P_H
