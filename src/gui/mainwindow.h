#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

#include "candevice/canfactoryqt.hpp"
#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"

#include <nodes/NodeData>
#include <nodes/FlowScene>
#include <nodes/FlowView>

class QMdiArea;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::shared_ptr<CanDevice> canDevice;
    std::shared_ptr<QtNodes::FlowScene> graphScene;
    CanRawView* canRawView;
    CanRawSender* canRawSender;
    QtNodes::FlowView* graphView;

    void handleDock(QWidget* component, QMdiArea* mdi);
    void connectCanComponentsSignals();
    void connectToolbarSignals();
    void connectMenuSignals();
    void connectDockingSignals();
    void setupMdiArea();

private slots:
    void handleExitAction(void);
};

#endif // MAINWINDOW_H
