#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H

#include <QtWidgets/QWidget>

namespace QtNodes {
class Node;
}

class ProjectConfigPrivate;

class ProjectConfig : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProjectConfig)

public:
    explicit ProjectConfig(QWidget* parent_);
    ~ProjectConfig();
    void closeEvent(QCloseEvent* e);
    QByteArray save();
    void load(const QByteArray& data);
    void clearGraphView();

signals:
    void handleDock(QWidget* component);
    void componentWidgetCreated(QWidget* component);
    void stopSimulation();
    void startSimulation();

private:
    QScopedPointer<ProjectConfigPrivate> d_ptr;
};

#endif // PROJECTCONFIG_H
