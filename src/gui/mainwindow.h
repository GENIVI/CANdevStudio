#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

class QMdiArea;
class CanDevice;
class CanRawView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    std::unique_ptr<QMdiArea> mdi;
    std::unique_ptr<CanDevice> canDevice;
    std::unique_ptr<CanRawView> canRawView;
};

#endif // MAINWINDOW_H
