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

class ProjectConfigurationPrivate;

class ProjectConfiguration : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProjectConfiguration)

private:
    QAction* _start;
    QAction* _stop;
    QScopedPointer<ProjectConfigurationPrivate> d_ptr;

public:
    explicit ProjectConfiguration(QAction* start, QAction* stop);
    ~ProjectConfiguration();
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
