#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "cansignalsender/cansignalsender.h"
#include "cansignalview/cansignalview.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mdi(std::make_unique<QMdiArea>())
    , canDevice(std::make_unique<CanDevice>(factory))
    , canRawView(std::make_unique<CanRawView>())
    , canSignalView(std::make_unique<CanSignalView>())
    , canRawSender(std::make_unique<CanRawSender>())
    , canSignalSender(std::make_unique<CanSignalSender>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0,0,0,0);

    canRawView->setWindowTitle("Can Raw View");
    ui->mdiArea->addSubWindow(canRawView.get());

    canSignalView->setWindowTitle("Can Signal View");
    ui->mdiArea->addSubWindow(canSignalView.get());

    canSignalSender->setWindowTitle("Can Signal Sender");
    ui->mdiArea->addSubWindow(canSignalSender.get());

    canRawSender->setWindowTitle("Can Raw Sender");
    ui->mdiArea->addSubWindow(canRawSender.get());

    ui->mdiArea->tileSubWindows();

    connect(canDevice.get(), &CanDevice::frameReceived, canRawView.get(), &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView.get(), &CanRawView::frameSent);

    connect(canRawSender.get(), &CanRawSender::sendFrame, canDevice.get(), &CanDevice::sendFrame);

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
