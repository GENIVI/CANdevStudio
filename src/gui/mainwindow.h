#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

#include "candevice/candevice.h"
#include "candevice/canfactoryqt.hpp"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"

#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/Node>
#include <nodes/NodeData>

class QMdiArea;
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
    std::shared_ptr<CanRawSender> canRawSender;
    std::shared_ptr<QtNodes::FlowScene> graphScene;
    QtNodes::FlowView* graphView;

    void handleDock(QWidget* component, QMdiArea* mdi);
    void connectToolbarSignals();
    void connectMenuSignals();
    void setupMdiArea();
    void closeEvent(QCloseEvent* event);
    void nodeCreatedCallback(QtNodes::Node& node);
    void nodeDeletedCallback(QtNodes::Node& node);
    void nodeDoubleClickedCallback(QtNodes::Node& node);
    void handleLoadAction();
    void handleSaveAction();

private slots:
    void handleExitAction(void);
};

#endif // MAINWINDOW_H
