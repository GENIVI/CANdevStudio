#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

#include "candevice/canfactoryqt.hpp"

class QAction;
class QMenu;
class QMdiArea;
class CanDevice;
class CanRawView;
class CanSignalView;
class CanRawSender;
class CanSignalSender;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    // functions:
    void createMenu();

    // data:
    std::unique_ptr<QMdiArea> mdi;
    // Project menu items:
    std::unique_ptr<QMenu> projectMenu;
    std::unique_ptr<QAction> loadAct;
    std::unique_ptr<QAction> exitAct;

    // Window menu items:
    std::unique_ptr<QMenu> windowMenu;
    std::unique_ptr<QAction> cascadeAct;
    std::unique_ptr<QAction> tileAct;
    std::unique_ptr<QAction> tabViewAct;

    // Help menu items:
    std::unique_ptr<QMenu> helpMenu;
    std::unique_ptr<QAction> aboutAct;

    std::unique_ptr<CanDevice> canDevice;
    std::unique_ptr<CanRawView> canRawView;
    std::unique_ptr<CanSignalView> canSignalView;
    std::unique_ptr<CanRawSender> canRawSender;
    std::unique_ptr<CanSignalSender> canSignalSender;
    CanFactoryQt factory;

private slots:
    void loadProject();
    void cascadeWindows();
    void tileWindows();
    void tabView();
    void about();
};

#endif // MAINWINDOW_H
