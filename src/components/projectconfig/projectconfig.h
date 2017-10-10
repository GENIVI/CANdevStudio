#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H

#include <QtWidgets/QWidget>
#include "configuration.h"
namespace QtNodes {
class Node;
}

class ProjectConfigPrivate;

class ProjectConfig : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProjectConfig)

public:
    explicit ProjectConfig(QWidget* parent_, Config::IConfig* configMgr);
    ~ProjectConfig();
    void closeEvent(QCloseEvent* e);
    QByteArray save();
    void load(const QByteArray& data);
    void clearGraphView();

signals:
    void handleDock(QWidget* component);
    void handleWidgetDeletion(QWidget* widget);
    void handleWidgetShowing(QWidget* widget, bool docked);
    void componentWidgetCreated(QWidget* component);
    void stopSimulation();
    void startSimulation();
    void closeProject();

private:
    QScopedPointer<ProjectConfigPrivate> d_ptr;
    //Config::IConfig* _configMgr;
};

#endif // PROJECTCONFIG_H
