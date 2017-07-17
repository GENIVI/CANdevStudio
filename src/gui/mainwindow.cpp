#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>

#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "cansignalsender/cansignalsender.cpp"
#include "cansignalview/cansignalview.h"
#include "mainwindow.h"
#include "toolbar/cdstoolbar.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , MainWidget(std::make_unique<QWidget>())
    , VertLayout(std::make_unique<QVBoxLayout>())
    , Toolbar(std::make_unique<CdsToolbar>())
    , mdi(std::make_unique<QMdiArea>())
    , canDevice(std::make_unique<CanDevice>(factory))
    , canRawView(std::make_unique<CanRawView>())
    , canSignalView(std::make_unique<CanSignalView>())
    , canRawSender(std::make_unique<CanRawSender>())
    , canSignalSender(std::make_unique<CanSignalSender>())
{
    mdi->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdi->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    VertLayout->addWidget(Toolbar.get());
    VertLayout->addWidget(mdi.get());
    MainWidget->setLayout(VertLayout.get());
    setCentralWidget(MainWidget.get());

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
    connect(canDevice.get(), &CanDevice::frameSent, canRawView.get(), &CanRawView::frameSent);

    connect(canRawSender.get(), &CanRawSender::sendFrame, canDevice.get(), &CanDevice::sendFrame);

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow()
{
}
