#ifndef __PCIMPL_H
#define __PCIMPL_H

#include <QObject>
#include "pcinterface.h"

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
};

#endif /* !__PCIMPL_H */
