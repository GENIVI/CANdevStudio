#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QtWidgets/QMainWindow>
#include <memory>

#include "projectconfig/projectconfig.h"

class QCloseEvent;
class QPushButton;

namespace Ui {
class ToolBar;
class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    enum class Styles { darkStyle, lightStyle };

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> _ui;
    std::unique_ptr<Ui::ToolBar> _toolBar;
    std::unique_ptr<ProjectConfig> _projectConfig;
    QString _projectFile;
    QString _projectName;
    Styles _currentStyle;
    QSettings _settings;
    QVector<std::pair<QString, QString>> _recentProjects;
    QVector<std::pair<QPushButton *, QPushButton *>> _recentProjectsButtons;

    void connectToolbarSignals();
    void connectMenuSignals();
    void closeEvent(QCloseEvent* event);
    void handleLoadAction();
    void handleAboutAction();
    void handleSaveAction();
    void handleSaveAsAction();
    void addToMdi(QWidget* component);
    void loadSettings();
    void saveSettings();
    void addRecentProject(const QString &name, const QString &path);
    void refreshRecentProjects();
    void handleRecentProject(int ndx);

public slots:
    void handleDock(QWidget* component);
    void handleWidgetDeletion(QWidget* widget);
    void handleWidgetShowing(QWidget* widget, bool docked);
    bool createProjectConfig(const QString& name);
    bool closeProjectConfig();
    void switchStyle();
    void setStyle(Styles style);
};

#endif // MAINWINDOW_H
