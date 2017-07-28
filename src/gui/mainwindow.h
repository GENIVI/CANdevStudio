#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

#include "candevice/canfactoryqt.hpp"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"

class CanDevice;
class QMdiArea;
class QShowEvent;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void requestAvailableDevices(QString backend);
    void selectCANDevice(QString backend, QString name);

public slots:
    void availableDevices(QString backend, QList<QCanBusDeviceInfo> devices);
    void attachToViews(CanDevice* device);

protected:
    void showEvent(QShowEvent *event);

private:
    void handleDock(QWidget* component, QMdiArea* mdi);

    std::unique_ptr<Ui::MainWindow> ui;

    CanRawView* canRawView = nullptr;
    CanRawSender* canRawSender = nullptr;

};

#endif // MAINWINDOW_H
