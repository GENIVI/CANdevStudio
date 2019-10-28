#ifndef __PCIMPL_H
#define __PCIMPL_H

#include <QObject>
#include <QMenu>
#include "pcinterface.h"
#include "componentinterface.h"
#include "componentmodel.h"
#include <propertyeditordialog.h>

class PCImpl : public PCInterface {
public:
    virtual void setNodeCreatedCallback(QtNodes::FlowScene* scene, const node_t& cb) override 
    {
        QObject::connect(scene, &QtNodes::FlowScene::nodeCreated, cb);
    }

    virtual void setNodeDeletedCallback(QtNodes::FlowScene* scene, const node_t& cb) override
    {
        QObject::connect(scene, &QtNodes::FlowScene::nodeDeleted, cb);
    }

    virtual void setNodeDoubleClickedCallback(QtNodes::FlowScene* scene, const node_t& cb) override
    {
        QObject::connect(scene, &QtNodes::FlowScene::nodeDoubleClicked, cb);
    }

    virtual void setNodeContextMenuCallback(QtNodes::FlowScene* scene, const menu_t& cb) override
    {
        QObject::connect(scene, &QtNodes::FlowScene::nodeContextMenu, cb);
    }

    virtual void setConfigChangedCbk(const node_t cb) override
    {
        _configUpdated = cb;
    }

    virtual void openProperties(QtNodes::Node& node) override
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

            _configUpdated(node);
        }
    }

    virtual void showContextMenu(QMenu &menu, const QPoint& pos) override
    {
        auto pos1 = pos;
        pos1.setX(pos1.x() + 32); // FIXME: these values are hardcoded and should not be here
        pos1.setY(pos1.y() + 10); //        find the real cause of misalignment of context menu
        menu.exec(pos1);
    }

private:
    node_t _configUpdated;

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
};

#endif /* !__PCIMPL_H */
