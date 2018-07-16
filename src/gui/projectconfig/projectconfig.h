#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H

#include <QtWidgets/QWidget>
#include <context.h>

struct PCInterface;
typedef Context<PCInterface> ProjectConfigCtx;

namespace QtNodes {
class Node;
}

class ProjectConfigPrivate;

class ProjectConfig : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(ProjectConfig)

public:
    explicit ProjectConfig(QWidget* parent_);
    explicit ProjectConfig(QWidget* parent_, ProjectConfigCtx&& ctx);
    ~ProjectConfig();
    void closeEvent(QCloseEvent* e);
    QByteArray save();
    void load(const QByteArray& data);
    void clearGraphView();
    void setColorMode(bool darkMode);

public slots:
    void simulationStarted();
    void simulationStopped();

signals:
    void handleDock(QWidget* component);
    void handleWidgetDeletion(QWidget* widget);
    void handleWidgetShowing(QWidget* widget, bool docked);
    void stopSimulation();
    void startSimulation();
    void componentWidgetCreated(QWidget* component);
    void closeProject();

private:
    QScopedPointer<ProjectConfigPrivate> d_ptr;
};

#endif // PROJECTCONFIG_H
