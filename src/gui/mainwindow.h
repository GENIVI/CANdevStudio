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
    std::shared_ptr<CanDevice> canDevice;
    std::shared_ptr<QtNodes::FlowScene> graphScene;
    QVector<QWidget*> undockWindows;

    void handleDock(QWidget* component, QMdiArea* mdi);
    void connectToolbarSignals();
    void connectMenuSignals();
    void setupMdiArea();
    void closeEvent(QCloseEvent* event);

private slots:
    void handleExitAction(void);
};

#endif // MAINWINDOW_H
