#include "mainwindow.h"
#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "canscripter/canscripter.h"
#include "cansignalcoder/cansignalcoder.h"
#include "cansignalsender/cansignalsender.cpp"
#include "cansignalview/cansignalview.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMdiArea>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mdi(std::make_unique<QMdiArea>())
    , canDevice(std::make_unique<CanDevice>())
    , canRawView(std::make_unique<CanRawView>())
    , canSignalCoder(std::make_unique<CanSignalCoder>())
    , canSignalView(std::make_unique<CanSignalView>())
    , canRawSender(std::make_unique<CanRawSender>())
    , canSignalSender(std::make_unique<CanSignalSender>())
    , canScripter(std::make_unique<CanScripter>("src/components/canscripter/genivi-script.json"))
{
    mdi->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdi->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(mdi.get());

    canRawView->setWindowTitle("Can Raw View");
    mdi->addSubWindow(canRawView.get());
    canSignalView->setWindowTitle("Can Signal View");
    mdi->addSubWindow(canSignalView.get());
    canSignalSender->setWindowTitle("Can Signal Sender");
    mdi->addSubWindow(canSignalSender.get());
    canRawSender->setWindowTitle("Can Raw Sender");
    mdi->addSubWindow(canRawSender.get());
    mdi->tileSubWindows();

    connect(canDevice.get(), &CanDevice::frameReceived, canRawView.get(), &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameReceived, canSignalCoder.get(), &CanSignalCoder::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView.get(), &CanRawView::frameSent);

    connect(canSignalCoder.get(), &CanSignalCoder::sendFrame, canDevice.get(), &CanDevice::sendFrame);
    connect(canSignalCoder.get(), &CanSignalCoder::sendSignal, canSignalView.get(), &CanSignalView::signalReceived);

    connect(canSignalSender.get(), &CanSignalSender::sendSignal, canSignalCoder.get(), &CanSignalCoder::signalReceived);
    connect(canRawSender.get(), &CanRawSender::sendFrame, canDevice.get(), &CanDevice::sendFrame);

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow()
{
}
