#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <memory>

class CanDevice;
class CanSignalCoder;
class CanScripter;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    uint32_t logoNdx{ 0 };
    int logoIntervalMs{ 1000 };
    std::vector<QString> logosFiles{ "test.png", "test2.png" };
    QTimer logosTimer;
    std::unique_ptr<CanDevice> canDevice;
    std::unique_ptr<CanSignalCoder> canSignalCoder;
    std::unique_ptr<CanScripter> canScripter;
};

#endif // MAINWINDOW_H
