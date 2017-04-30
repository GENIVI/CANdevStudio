#include "mainwindow.h"
#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "canscripter/canscripter.h"
#include "cansignalcoder/cansignalcoder.h"
#include "cansignalsender/cansignalsender.cpp"
#include "cansignalview/cansignalview.h"
#include <QPushButton>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , canDevice(std::make_unique<CanDevice>())
    , canSignalCoder(std::make_unique<CanSignalCoder>())
    , canScripter(std::make_unique<CanScripter>("src/components/canscripter/genivi-script.json"))
{
    CanRawView *canRawView = new CanRawView();
    CanSignalView *canSignalView = new CanSignalView();
    CanRawSender *canRawSender = new CanRawSender();
    CanSignalSender *canSignalSender = new CanSignalSender();
    QVBoxLayout *rowLayout = new QVBoxLayout();
    QHBoxLayout *colLayout;
    QToolBar *tb = new QToolBar();
    QPushButton *pbStart = new QPushButton("Start");
    QPushButton *pbStop = new QPushButton("Stop");
    QPushButton *pbClear = new QPushButton("Clear");

    tb->addWidget(pbStart);
    tb->addWidget(pbStop);
    tb->addWidget(pbClear);
    rowLayout->addWidget(tb);

    colLayout = new QHBoxLayout();
    colLayout->QLayout::addWidget(canRawView);
    colLayout->QLayout::addWidget(canSignalView);
    rowLayout->addLayout(colLayout);

    colLayout = new QHBoxLayout();
    colLayout->QLayout::addWidget(canRawSender);
    colLayout->QLayout::addWidget(canSignalSender);
    rowLayout->addLayout(colLayout);

    QWidget *window = new QWidget();
    window->setLayout(rowLayout);
    setCentralWidget(window);

    connect(canDevice.get(), &CanDevice::frameReceived, canRawView, &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameReceived, canSignalCoder.get(), &CanSignalCoder::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView, &CanRawView::frameSent);

    connect(canSignalCoder.get(), &CanSignalCoder::sendFrame, canDevice.get(), &CanDevice::sendFrame);
    connect(canSignalCoder.get(), &CanSignalCoder::sendSignal, canSignalView, &CanSignalView::signalReceived);

    connect(canSignalSender, &CanSignalSender::sendSignal, canSignalCoder.get(), &CanSignalCoder::signalReceived);
    connect(canScripter.get(), &CanScripter::sendSignal, canSignalCoder.get(), &CanSignalCoder::signalReceived);
    connect(canRawSender, &CanRawSender::sendFrame, canDevice.get(), &CanDevice::sendFrame);

    connect(pbStart, &QPushButton::pressed, canScripter.get(), &CanScripter::start);
    connect(pbStart, &QPushButton::pressed, canSignalCoder.get(), &CanSignalCoder::clearFrameCache);
    connect(pbStop, &QPushButton::pressed, canScripter.get(), &CanScripter::stop);

    canDevice.get()->init("socketcan", "can0");
    canDevice.get()->start();
}

MainWindow::~MainWindow()
{
}
