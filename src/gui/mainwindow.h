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
    int logoIntervalMs{ 5000 };
    std::vector<QString> logosFiles{ "genivi_1.png", "genivi_2.png" };
    QTimer logosTimer;
    std::unique_ptr<CanDevice> canDevice;
    std::unique_ptr<CanSignalCoder> canSignalCoder;
    std::unique_ptr<CanScripter> canScripter;
};

#endif // MAINWINDOW_H
