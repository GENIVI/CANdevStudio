#ifndef PROJECTCONFIGURATION_H
#define PROJECTCONFIGURATION_H

#include <QtCore/QObject>
#include <memory>

#include "candevice/candevice.h"
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

public:
    explicit ProjectConfiguration();
    ~ProjectConfiguration();
    void closeEvent(QCloseEvent* e);
    QByteArray save();
    void load(const QByteArray& data);
    void clearGraphView();

signals:
    void handleDock(QWidget* component);
    void componentWidgetCreated(QWidget* component);
    void stopSimulation();
    void startSimulation();

private slots:
    void nodeCreatedCallback(QtNodes::Node& node);
    void nodeDeletedCallback(QtNodes::Node& node);
    void nodeDoubleClickedCallback(QtNodes::Node& node);

private:
    QScopedPointer<ProjectConfigurationPrivate> d_ptr;
};

#endif // PROJECTCONFIGURATION_H
