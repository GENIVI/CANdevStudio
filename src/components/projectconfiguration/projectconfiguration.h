#ifndef PROJECTCONFIGURATION_H
#define PROJECTCONFIGURATION_H

#include <QtCore/QObject>
#include <memory>

#include "candevice/candevice.h"
#include "candevice/canfactoryqt.hpp"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>
#include <nodes/NodeData>

class ProjectConfiguration : public QObject {
    Q_OBJECT
private:
    std::shared_ptr<QtNodes::FlowScene> graphScene;
    QtNodes::FlowView* graphView; // FIXME
    QAction* _start;
    QAction* _stop;

public:
    explicit ProjectConfiguration(QAction* start, QAction* stop);
    QWidget* getGraphView() const;

signals:
    void handleDock(QWidget* component);
    void componentWidgetCreated(QWidget* component);

private slots:
    void nodeCreatedCallback(QtNodes::Node& node);
    void nodeDeletedCallback(QtNodes::Node& node);
    void nodeDoubleClickedCallback(QtNodes::Node& node);
};

#endif // PROJECTCONFIGURATION_H
