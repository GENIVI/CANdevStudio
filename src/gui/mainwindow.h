#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class CanDevice;
class CanSignalCoder;
class CanScripter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    std::unique_ptr<CanDevice> canDevice;
    std::unique_ptr<CanSignalCoder> canSignalCoder;
    std::unique_ptr<CanScripter> canScripter;
};

#endif // MAINWINDOW_H
