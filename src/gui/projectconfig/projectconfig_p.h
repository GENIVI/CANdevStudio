#ifndef PROJECTCONFIG_P_H
#define PROJECTCONFIG_P_H

#include "flowviewwrapper.h"
#include "iconlabel.h"
#include "modeltoolbutton.h"
#include "pcimpl.h"
#include "ui_projectconfig.h"
#include <QMenu>
#include <QtWidgets/QPushButton>
#include <log.h>
#include <modelvisitor.h> // apply_model_visitor
#include <nodes/Node>
#include <propertyeditordialog.h>
#include <bcastmsgs.h>

#include "componentinterface.h"
#include "plugins.hpp"

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
        , _plugins(_graphScene.registry())
    {
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

        _ui->scrollArea->setMinimumSize(165, 0);
        _ui->scrollArea->setMaximumSize(165, 10000);
        _plugins.initSections(*_ui);

        _pcInt.setConfigChangedCbk([this](QtNodes::Node& node) {
            QJsonObject msg = initBcast(node);
            msg["msg"] = BcastMsg::ConfigChanged;

            auto& component = getComponent(node);
            auto propObj = component.getQConfig();
            auto prop = propObj->property("exposedProperties");

            QJsonObject confObj;
            for (const auto& p : prop.toStringList()) {
                confObj[p] = propObj->property(p.toStdString().c_str()).toString();
            }

            msg["config"] = confObj;

            emit simBcast(msg);
        });
    }

    void addModelIcons()
    {
        QColor bgColor;

        if (_darkMode) {
            bgColor = QColor(94, 94, 94);
        } else {
            bgColor = QColor(255, 255, 255);
        }

        _plugins.clearSections();
        _plugins.addWidgets(bgColor);
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
        iface.initModel(node, _nodeCnt, _darkMode);

        connect(q, &ProjectConfig::startSimulation, &iface, &ComponentModelInterface::startSimulation);
        connect(q, &ProjectConfig::stopSimulation, &iface, &ComponentModelInterface::stopSimulation);
        connect(&iface, &ComponentModelInterface::handleDock, q, &ProjectConfig::handleDock);
        connect(&iface, &ComponentModelInterface::simBcastSnd, this, &ProjectConfigPrivate::simBcast);
        connect(this, &ProjectConfigPrivate::simBcast, &iface, &ComponentModelInterface::simBcastRcv);

        node.nodeGraphicsObject().setOpacity(node.nodeDataModel()->nodeStyle().Opacity);
        addShadow(node);
        node.nodeGraphicsObject().update();

        _nodeCnt++;

        cds_debug("Node '{}' created", node.nodeDataModel()->caption().toStdString());

        QJsonObject msg = initBcast(node);
        msg["msg"] = BcastMsg::NodeCreated;

        emit simBcast(msg);
    }

    void nodeDeletedCallback(QtNodes::Node& node)
    {
        Q_Q(ProjectConfig);
        auto& component = getComponent(node);

        cds_debug("Node '{}' deleted", node.nodeDataModel()->caption().toStdString());

        emit q->handleWidgetDeletion(component.mainWidget());

        QJsonObject msg = initBcast(node);
        msg["msg"] = BcastMsg::NodeDeleted;

        emit simBcast(msg);
    }

    void nodeDoubleClickedCallback(QtNodes::Node& node)
    {
        auto& component = getComponent(node);

        cds_debug("Node '{}' double clicked", node.nodeDataModel()->caption().toStdString());

        if (component.mainWidget() != nullptr) {
            openWidget(node);
        } else {
            if (!_simStarted) {
                _pcInt.openProperties(node);
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
        connect(&actionProperties, &QAction::triggered, [this, &node]() { _pcInt.openProperties(node); });

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

        _pcInt.showContextMenu(contextMenu, mapToGlobal(_graphView->mapFromScene(pos)));
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

signals:
    void simBcast(const QJsonObject& msg, const QVariant& param = QVariant());

private:
    void openWidget(QtNodes::Node& node)
    {
        Q_Q(ProjectConfig);
        auto& component = getComponent(node);

        emit q->handleWidgetShowing(component.mainWidget(), component.mainWidgetDocked());
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

    QJsonObject initBcast(const QtNodes::Node& node)
    {
        QJsonObject msg;
        msg["id"] = node.id().toString();
        msg["name"] = node.nodeDataModel()->name();
        msg["caption"] = node.nodeDataModel()->caption();

        return msg;
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
    Plugins _plugins;
};
#endif // PROJECTCONFIG_P_H
