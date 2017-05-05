#include "mainwindow.h"
#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "canscripter/canscripter.h"
#include "cansignalcoder/cansignalcoder.h"
#include "cansignalsender/cansignalsender.cpp"
#include "cansignalview/cansignalview.h"
#include <QCheckBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , canDevice(std::make_unique<CanDevice>())
    , canSignalCoder(std::make_unique<CanSignalCoder>())
    , canScripter(std::make_unique<CanScripter>())
{
    CanRawView* canRawView = new CanRawView();
    CanSignalView* canSignalView = new CanSignalView();
    CanRawSender* canRawSender = new CanRawSender();
    CanSignalSender* canSignalSender = new CanSignalSender();
    QVBoxLayout* rowLayout = new QVBoxLayout();
    QVBoxLayout* colLayout;
    QToolBar* tb = new QToolBar();
    QPushButton* pbStart = new QPushButton("Start");
    QPushButton* pbStop = new QPushButton("Stop");
    QPushButton* pbClear = new QPushButton("Clear");
    QPushButton* pbOpen = new QPushButton("Open");
    QLineEdit* canIf = new QLineEdit();
    QLineEdit* scriptPath = new QLineEdit();
    QCheckBox* scriptCB = new QCheckBox(" Script: ");
    QCheckBox* scriptRepeat = new QCheckBox(" Repeat: ");

    canIf->setFixedWidth(50);
    pbStop->setEnabled(false);

    tb->addWidget(pbStart);
    tb->addWidget(pbStop);
    tb->addWidget(pbClear);
    tb->addSeparator();
    tb->addWidget(new QLabel(" CAN: "));
    tb->addWidget(canIf);
    tb->addSeparator();
    tb->addWidget(scriptCB);
    tb->addWidget(scriptRepeat);
    tb->addWidget(scriptPath);
    tb->addWidget(pbOpen);
    rowLayout->addWidget(tb);

    QHBoxLayout* mainScr = new QHBoxLayout();

    colLayout = new QVBoxLayout();
    colLayout->QLayout::addWidget(canSignalView);
    colLayout->QLayout::addWidget(canSignalSender);
    mainScr->addLayout(colLayout);

    QImage image("test.png");
    QLabel* imgLabel = new QLabel("");
    imgLabel->setMinimumSize({ 800, 1000 });
    imgLabel->setPixmap(QPixmap::fromImage(image));
    mainScr->addWidget(imgLabel);

    colLayout = new QVBoxLayout();
    colLayout->QLayout::addWidget(canRawView);
    colLayout->QLayout::addWidget(canRawSender);
    mainScr->addLayout(colLayout);

    QWidget* window = new QWidget();
    rowLayout->addLayout(mainScr);
    window->setLayout(rowLayout);
    setCentralWidget(window);

    connect(canDevice.get(), &CanDevice::frameReceived, canRawView, &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameReceived, canSignalCoder.get(), &CanSignalCoder::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView, &CanRawView::frameSent);

    connect(canSignalCoder.get(), &CanSignalCoder::sendFrame, canDevice.get(), &CanDevice::sendFrame);
    connect(canSignalCoder.get(), &CanSignalCoder::sendSignal, canSignalView, &CanSignalView::signalReceived);
    connect(canSignalCoder.get(), &CanSignalCoder::signalEncoded, canSignalView, &CanSignalView::signalSent);

    connect(canSignalSender, &CanSignalSender::sendSignal, canSignalCoder.get(), &CanSignalCoder::signalReceived);
    connect(canScripter.get(), &CanScripter::sendSignal, canSignalCoder.get(), &CanSignalCoder::signalReceived);
    connect(canRawSender, &CanRawSender::sendFrame, canDevice.get(), &CanDevice::sendFrame);

    connect(pbStart, &QPushButton::pressed, [this, scriptCB]() {
        if (scriptCB->isChecked()) {
            canScripter->start();
        }
    });
    connect(pbStart, &QPushButton::pressed, canSignalCoder.get(), &CanSignalCoder::clearFrameCache);
    connect(pbStart, &QPushButton::pressed, canDevice.get(), &CanDevice::start);
    connect(pbStart, &QPushButton::pressed, canRawView, &CanRawView::start);
    connect(pbStart, &QPushButton::pressed, canSignalView, &CanSignalView::start);
    connect(pbStart, &QPushButton::pressed, canSignalSender, &CanSignalSender::start);
    connect(pbStart, &QPushButton::pressed, canRawSender, &CanRawSender::start);
    connect(pbStop, &QPushButton::pressed, canScripter.get(), &CanScripter::stop);
    connect(pbStop, &QPushButton::pressed, canDevice.get(), &CanDevice::stop);
    connect(pbStop, &QPushButton::pressed, canSignalSender, &CanSignalSender::stop);
    connect(pbStop, &QPushButton::pressed, canRawSender, &CanRawSender::stop);
    connect(pbClear, &QPushButton::pressed, canSignalView, &CanSignalView::clear);
    connect(pbClear, &QPushButton::pressed, canRawView, &CanRawView::clear);

    connect(canIf, &QLineEdit::textChanged, [this](const QString& str) {
        canDevice->init("socketcan", str);
    });
    canIf->setText("can0");

    connect(pbOpen, &QPushButton::pressed, [this, scriptPath]() {
        scriptPath->setText(QFileDialog::getOpenFileName(this, tr("Open Script"), ".", tr("JSON file (*.json)")));
    });
    connect(scriptPath, &QLineEdit::textChanged, canScripter.get(), &CanScripter::setScriptFilename);

    // Disabling, enabling
    connect(pbStart, &QPushButton::clicked, pbStop, &QPushButton::setDisabled);
    connect(pbStart, &QPushButton::clicked, pbStart, &QPushButton::setEnabled);
    connect(pbStart, &QPushButton::clicked, canIf, &QLineEdit::setEnabled);
    connect(pbStart, &QPushButton::clicked, scriptCB, &QCheckBox::setEnabled);
    connect(pbStart, &QPushButton::clicked, scriptPath, &QWidget::setEnabled);
    connect(pbStart, &QPushButton::clicked, scriptRepeat, &QWidget::setEnabled);
    connect(pbStart, &QPushButton::clicked, pbOpen, &QWidget::setEnabled);

    connect(pbStop, &QPushButton::clicked, pbStart, &QPushButton::setDisabled);
    connect(pbStop, &QPushButton::clicked, pbStop, &QPushButton::setEnabled);
    connect(pbStop, &QPushButton::clicked, canIf, &QLineEdit::setDisabled);
    connect(pbStop, &QPushButton::clicked, scriptCB, &QCheckBox::setDisabled);
    connect(pbStop, &QPushButton::pressed, [scriptCB, pbOpen, scriptPath, scriptRepeat]() {
        if (scriptCB->isChecked()) {
            pbOpen->setEnabled(true);
            scriptPath->setEnabled(true);
            scriptRepeat->setEnabled(true);
        }
    });

    connect(scriptCB, &QCheckBox::clicked, pbOpen, &QPushButton::setEnabled);
    connect(scriptCB, &QCheckBox::clicked, scriptPath, &QLineEdit::setEnabled);
    connect(scriptCB, &QCheckBox::clicked, scriptRepeat, &QCheckBox::setEnabled);
    connect(scriptRepeat, &QCheckBox::clicked, canScripter.get(), &CanScripter::setScriptRepeat);

    scriptCB->setChecked(true);
    scriptRepeat->setChecked(true);
    scriptPath->setText("src/components/canscripter/genivi-script.json");

    logosTimer.setInterval(logoIntervalMs);
    connect(&logosTimer, &QTimer::timeout, [imgLabel, this]() {
        QImage image(logosFiles[logoNdx++ % logosFiles.size()]);
        imgLabel->setPixmap(QPixmap::fromImage(image));
    });
    logosTimer.start(logoIntervalMs);
}

MainWindow::~MainWindow()
{
}
