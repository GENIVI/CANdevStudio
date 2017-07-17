#include <QtCore/QtDebug>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "cansignalsender/cansignalsender.cpp"
#include "cansignalview/cansignalview.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , mdi(std::make_unique<QMdiArea>())
    , projectMenu(nullptr)
    , loadAct(nullptr)
    , exitAct(nullptr)
    , windowMenu(nullptr)
    , cascadeAct(nullptr)
    , tileAct(nullptr)
    , tabViewAct(nullptr)
    , helpMenu(nullptr)
    , aboutAct(nullptr)
    , canDevice(std::make_unique<CanDevice>(factory))
    , canRawView(std::make_unique<CanRawView>())
    , canSignalView(std::make_unique<CanSignalView>())
    , canRawSender(std::make_unique<CanRawSender>())
    , canSignalSender(std::make_unique<CanSignalSender>())
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

    createMenu();

    connect(canDevice.get(), &CanDevice::frameReceived, canRawView.get(), &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView.get(), &CanRawView::frameSent);

    connect(canRawSender.get(), &CanRawSender::sendFrame, canDevice.get(), &CanDevice::sendFrame);

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenu()
{
    // Project menu:
    loadAct.reset(new QAction(tr("&Load"), this));
    loadAct->setShortcuts(QKeySequence::Open);
    loadAct->setStatusTip(tr("Load project"));
    connect(loadAct.get(), &QAction::triggered, this, &MainWindow::loadProject);

    exitAct.reset(new QAction(tr("E&xit"), this));
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct.get(), &QAction::triggered, this, &QWidget::close);

    projectMenu.reset(menuBar()->addMenu(tr("&Project")));
    projectMenu->addAction(loadAct.get());
    projectMenu->addSeparator();
    projectMenu->addAction(exitAct.get());

    // Window menu:
    cascadeAct.reset(new QAction(tr("&Cascade"), this));
    cascadeAct->setStatusTip(tr("Cascade windows"));
    connect(cascadeAct.get(), &QAction::triggered, this, &MainWindow::cascadeWindows);

    tileAct.reset(new QAction(tr("Tile"), this));
    tileAct->setStatusTip(tr("Tile windows"));
    connect(tileAct.get(), &QAction::triggered, this, &MainWindow::tileWindows);

    tabViewAct.reset(new QAction(tr("Tab &view"), this));
    tabViewAct->setStatusTip(tr("Enable tab view"));
    connect(tabViewAct.get(), &QAction::triggered, this, &MainWindow::tabView);

    windowMenu.reset(menuBar()->addMenu(tr("&Windows")));
    windowMenu->addAction(cascadeAct.get());
    windowMenu->addAction(tileAct.get());
    windowMenu->addAction(tabViewAct.get());

    // Help menu:
    aboutAct.reset(new QAction(tr("&About"), this));
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct.get(), &QAction::triggered, this, &MainWindow::about);

    helpMenu.reset(menuBar()->addMenu(tr("&Help")));
    helpMenu->addAction(aboutAct.get());
}

// Menu item slots:
void MainWindow::loadProject()
{
    qDebug() << "loadProject()";
}

void MainWindow::cascadeWindows()
{
    qDebug() << "cascadeWindows()";
    mdi->cascadeSubWindows();
}

void MainWindow::tileWindows()
{
    qDebug() << "tileWindows()";
    mdi->tileSubWindows();
}

void MainWindow::tabView()
{
    qDebug() << "tabView()";
}

void MainWindow::about()
{
    qDebug() << "about()";
}
