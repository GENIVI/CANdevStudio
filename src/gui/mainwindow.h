#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

#include "projectconfig/projectconfig.h"

class QCloseEvent;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    enum class Styles {
        darkStyle,
        lightStyle
    };

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> _ui;
    std::unique_ptr<ProjectConfig> _projectConfig;
    QString _projectFile;
    QString _projectName;
    Styles currentStyle;

    void connectToolbarSignals();
    void connectMenuSignals();
    void setupMdiArea();
    void closeEvent(QCloseEvent* event);
    void handleLoadAction();
    void handleSaveAction();
    void handleSaveAsAction();
    void addToMdi(QWidget* component);

public slots:
    void handleDock(QWidget* component);
    void handleWidgetDeletion(QWidget* widget);
    void handleWidgetShowing(QWidget* widget, bool docked);
    bool createProjectConfig(const QString &name);
    bool closeProjectConfig();
    void switchStyle();
    void setStyle(Styles style);
};

#endif // MAINWINDOW_H
