#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

#include "candevice/canfactoryqt.hpp"

class QMdiArea;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    MainWindow(QWidget* parent,std::shared_ptr<Ui::MainWindow> ui);
    ~MainWindow();
signals:
    void fileLoadedOk();

private:
    std::shared_ptr<Ui::MainWindow> ui;
    void handleDock(QWidget* component, QMdiArea* mdi);
private slots:
    void handleExitAction(void);
};

#endif // MAINWINDOW_H
