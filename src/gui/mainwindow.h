#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

#include "projectconfig/projectconfig.h"
#include "projectconfig/configmgr.h"

class QCloseEvent;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<ProjectConfig> projectConfig;
    std::unique_ptr<ConfigMgr> configMgr;

    void connectToolbarSignals();
    void connectMenuSignals();
    void setupMdiArea();
    void closeEvent(QCloseEvent* event);
    void handleLoadAction();
    void handleSaveAction();
    void addToMdi(QWidget* component);

public slots:
    void handleDock(QWidget* component);
    void handleWidgetDeletion(QWidget* widget);
    void handleWidgetShowing(QWidget* widget, bool docked);
    bool createProjectConfig(const QString &name);
    bool closeProjectConfig();
};

#endif // MAINWINDOW_H
