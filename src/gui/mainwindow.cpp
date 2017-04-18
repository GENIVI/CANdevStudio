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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mdi(std::make_unique<QMdiArea>()),
    canDevice(std::make_unique<CanDevice>()),
    canRawView(std::make_unique<CanRawView>()),
    canSignalCoder(std::make_unique<CanSignalCoder>())
{
    mdi->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdi->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setCentralWidget(mdi.get());

    mdi->addSubWindow(canRawView.get());
    mdi->tileSubWindows();

    connect(canDevice.get(), &CanDevice::frameReceived, canRawView.get(), &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView.get(), &CanRawView::frameSent);

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow()
{
}
