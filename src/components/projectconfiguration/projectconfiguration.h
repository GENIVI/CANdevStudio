#ifndef PROJECTCONFIGURATION_H
#define PROJECTCONFIGURATION_H

#include <QtCore/QObject>
#include <memory>

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>
#include <nodes/NodeData>

class ProjectConfigurationPrivate;

class ProjectConfiguration : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProjectConfiguration)

public:
    QAction* _start;
    QAction* _stop;

private:
    QScopedPointer<ProjectConfigurationPrivate> d_ptr;

public:
    explicit ProjectConfiguration(QAction* start, QAction* stop);
    ~ProjectConfiguration();
    void closeEvent(QCloseEvent* e);
    QByteArray save();
    void load(const QByteArray& data);
    void clearGraphView();

signals:
    void handleDock(QWidget* component);
    void componentWidgetCreated(QWidget* component);

private slots:
    void nodeCreatedCallback(QtNodes::Node& node);
    void nodeDeletedCallback(QtNodes::Node& node);
    void nodeDoubleClickedCallback(QtNodes::Node& node);
};

#endif // PROJECTCONFIGURATION_H
