#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

class QMdiArea;
class CanDevice;
class CanRawView;
class CanSignalView;
class CanRawSender;
class CanSignalSender;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<QMdiArea> mdi;
    std::unique_ptr<CanDevice> canDevice;
    std::unique_ptr<CanRawView> canRawView;
    std::unique_ptr<CanSignalView> canSignalView;
    std::unique_ptr<CanRawSender> canRawSender;
    std::unique_ptr<CanSignalSender> canSignalSender;
};

#endif // MAINWINDOW_H
