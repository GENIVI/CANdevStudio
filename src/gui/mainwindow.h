#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <rapidjson/schema.h>
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
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<ProjectConfig> projectConfig;

    static std::shared_ptr<rapidjson::SchemaDocument> configSchema;
    static bool schemaInitialized;

    void connectToolbarSignals();
    void connectMenuSignals();
    void setupMdiArea();
    void closeEvent(QCloseEvent* event);
    void handleLoadAction();
    void handleSaveAction();
    void addToMdi(QWidget* component);
    bool validateConfiguration(const QByteArray& wholeFile) const;
    static bool loadConfigSchema();

public slots:
    void handleDock(QWidget* component);
    void handleWidgetDeletion(QWidget* widget);
    void handleWidgetShowing(QWidget* widget, bool docked);
    bool createProjectConfig(const QString &name);
    bool closeProjectConfig();
};

#endif // MAINWINDOW_H
