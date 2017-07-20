#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "cansignalsender/cansignalsender.h"
#include "cansignalview/cansignalview.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0,0,0,0);

    CanFactoryQt factory;
    CanDevice* canDevice = new CanDevice(factory);
    CanRawView* canRawView = new CanRawView();
    CanSignalView* canSignalView = new CanSignalView();
    CanRawSender* canRawSender = new CanRawSender();
    CanSignalSender* canSignalSender = new CanSignalSender;

    canRawView->setWindowTitle("Can Raw View");
    ui->mdiArea->addSubWindow(canRawView);

    canSignalView->setWindowTitle("Can Signal View");
    ui->mdiArea->addSubWindow(canSignalView);

    canSignalSender->setWindowTitle("Can Signal Sender");
    ui->mdiArea->addSubWindow(canSignalSender);

    canRawSender->setWindowTitle("Can Raw Sender");
    ui->mdiArea->addSubWindow(canRawSender);

    ui->mdiArea->tileSubWindows();

    connect(canDevice, &CanDevice::frameReceived, canRawView, &CanRawView::frameReceived);
    connect(canDevice, &CanDevice::frameSent, canRawView, &CanRawView::frameSent);
    connect(ui->actionstart, SIGNAL(triggered(bool)), canRawView, SLOT(simulationStarted(void)));
    connect(ui->actionstop, SIGNAL(triggered(bool)), canRawView, SLOT(simulationStopped(void)));
    connect(canRawSender, &CanRawSender::sendFrame, canDevice, &CanDevice::sendFrame);

    connect(ui->actionstart,SIGNAL(triggered(bool)),ui->actionstop,SLOT(setDisabled(bool)));
    connect(ui->actionstart,SIGNAL(triggered(bool)),ui->actionstart,SLOT(setEnabled(bool)));
    connect(ui->actionstop,SIGNAL(triggered(bool)),ui->actionstop,SLOT(setEnabled(bool)));
    connect(ui->actionstop,SIGNAL(triggered(bool)),ui->actionstart,SLOT(setDisabled(bool)));

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow()
{
}
