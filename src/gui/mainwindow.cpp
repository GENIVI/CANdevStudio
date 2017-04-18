#include <QMdiArea>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QToolBar>
#include <QPushButton>
#include "mainwindow.h"
#include "candevice/candevice.h"
#include "canrawview/canrawview.h"
#include "cansignalcoder/cansignalcoder.h"
#include "cansignalview/cansignalview.h"
#include "canrawsender/canrawsender.h"
#include "cansignalsender/cansignalsender.cpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mdi(std::make_unique<QMdiArea>()),
    canDevice(std::make_unique<CanDevice>()),
    canRawView(std::make_unique<CanRawView>()),
    canSignalCoder(std::make_unique<CanSignalCoder>()),
    canSignalView(std::make_unique<CanSignalView>()),
    canRawSender(std::make_unique<CanRawSender>()),
    canSignalSender(std::make_unique<CanSignalSender>())
{
    mdi->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdi->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(mdi.get());

    mdi->addSubWindow(canRawView.get());
    mdi->addSubWindow(canSignalView.get());
    mdi->addSubWindow(canSignalSender.get());
    mdi->tileSubWindows();

    connect(canDevice.get(), &CanDevice::frameReceived, canRawView.get(), &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView.get(), &CanRawView::frameSent);

    connect(canDevice.get(), &CanDevice::frameReceived, canSignalCoder.get(), &CanSignalCoder::frameReceived);
    connect(canSignalCoder.get(), &CanSignalCoder::sendSignal, canSignalView.get(), &CanSignalView::signalReceived);

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow()
{
}
