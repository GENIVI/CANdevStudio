#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "candevice/candevice.h"
#include "mainwindow.h"
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QInputDialog>

#include <iostream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    canRawView = new CanRawView();
    canRawSender = new CanRawSender();

    canRawView->setWindowTitle("Can Raw View");
    ui->mdiArea->addSubWindow(canRawView);

    canRawSender->setWindowTitle("Can Raw Sender");
    ui->mdiArea->addSubWindow(canRawSender);

    ui->mdiArea->tileSubWindows();

    connect(ui->actionstart, &QAction::triggered, canRawView, &CanRawView::startSimulation);
    connect(ui->actionstop, &QAction::triggered, canRawView, &CanRawView::stopSimulation);

    connect(ui->actionstart, &QAction::triggered, ui->actionstop, &QAction::setDisabled);
    connect(ui->actionstart, &QAction::triggered, ui->actionstart, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstop, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstart, &QAction::setDisabled);

    //docking signals connection
    connect(canRawView, &CanRawView::dockUndock, this, [this] {
        handleDock(this->canRawView, ui->mdiArea);
    });
    connect(canRawSender, &CanRawSender::dockUndock, this, [this] {
        handleDock(this->canRawSender, ui->mdiArea);
    });

}

MainWindow::~MainWindow()
{
    delete canRawView;
    delete canRawSender;
}

void MainWindow::handleDock(QWidget* component, QMdiArea* mdi)
{
    //check if component is already displayed by mdi area
    if(mdi->subWindowList().contains(static_cast<QMdiSubWindow*>(component->parentWidget())))
    {
        //undock
        auto parent = component->parentWidget();
        mdi->removeSubWindow(component);    //removeSubwWndow only removes widget, not window
        component->show();
        parent->close();
    }
    else
    {
        //dock
        mdi->addSubWindow(component)->show();
    }
}

void MainWindow::showEvent(QShowEvent* event)
{
    emit requestAvailableDevices("socketcan"); // FIXME: plugin name should be configurable
}

void MainWindow::availableDevices(QString backend,
        QList<QCanBusDeviceInfo> devices)
{
    QStringList items;

    if (devices.empty())
    {
        return; //FIXME: show dialog
    }

    for (const auto& d: devices)
        items.push_back(d.name());

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Choose CAN device"),
                                         tr("Available CAN devices on %1 backend").arg(backend),
                                         items, 0, false, &ok);

    emit selectCANDevice(backend, item);
}

void MainWindow::attachToViews(CanDevice* device)
{
    QObject::connect(device, &CanDevice::frameReceived, canRawView, &CanRawView::frameReceived);
    QObject::connect(device, &CanDevice::frameSent, canRawView, &CanRawView::frameSent);
    QObject::connect(canRawSender, &CanRawSender::sendFrame, device, &CanDevice::sendFrame);

    ui->actionstart->setEnabled(true);
}
