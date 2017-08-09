#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    CanFactoryQt factory;
    auto modelRegistry = std::make_shared<QtNodes::DataModelRegistry>();

    canDevice = std::make_shared<CanDevice>(factory);
    graphScene = std::make_shared<QtNodes::FlowScene>(modelRegistry);
    canRawView = new CanRawView();
    canRawSender = new CanRawSender();
    graphView = new QtNodes::FlowView(graphScene.get());

    setupMdiArea();
    connectCanComponentsSignals();
    connectToolbarSignals();
    connectMenuSignals();
    connectDockingSignals();

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow()
{
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

void MainWindow::handleExitAction()
{
    QMessageBox::StandardButton userReply;
    userReply = QMessageBox::question(this, "Exit"
                                      , "Are you shure you want to quit CANdevStudio?"
                                      , QMessageBox::Yes | QMessageBox::No);
    if(userReply == QMessageBox::Yes)
        QApplication::quit();

}

void MainWindow::connectCanComponentsSignals()
{
    connect(canDevice.get(), &CanDevice::frameReceived, canRawView, &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView, &CanRawView::frameSent);
    connect(ui->actionstart, &QAction::triggered, canRawView, &CanRawView::startSimulation);
    connect(ui->actionstop, &QAction::triggered, canRawView, &CanRawView::stopSimulation);
    connect(canRawSender, &CanRawSender::sendFrame, canDevice.get(), &CanDevice::sendFrame);
}

void MainWindow::connectToolbarSignals()
{
    connect(ui->actionstart, &QAction::triggered, ui->actionstop, &QAction::setDisabled);
    connect(ui->actionstart, &QAction::triggered, ui->actionstart, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstop, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstart, &QAction::setDisabled);
}

void MainWindow::connectMenuSignals()
{
    QActionGroup *ViewModes = new QActionGroup(this);
    ViewModes->addAction(ui->actionTabView);
    ViewModes->addAction(ui->actionSubWindowView);
    connect(ui->actionAbout,&QAction::triggered,this,[this] { QMessageBox::about(this,"About","<about body>"); });
    connect(ui->actionExit,&QAction::triggered,this,&MainWindow::handleExitAction);
    connect(ui->actionTile,&QAction::triggered,ui->mdiArea,&QMdiArea::tileSubWindows);
    connect(ui->actionCascade,&QAction::triggered,ui->mdiArea,&QMdiArea::cascadeSubWindows);
    connect(ui->actionTabView,&QAction::triggered,this,[this]{ ui->mdiArea->setViewMode(QMdiArea::TabbedView); });
    connect(ui->actionTabView,&QAction::toggled,ui->actionTile,&QAction::setDisabled);
    connect(ui->actionTabView,&QAction::toggled,ui->actionCascade,&QAction::setDisabled);
    connect(ui->actionSubWindowView,&QAction::triggered,this,[this]{ ui->mdiArea->setViewMode(QMdiArea::SubWindowView); });
}

void MainWindow::connectDockingSignals()
{
    connect(canRawView, &CanRawView::dockUndock, this, [this] {
		                        handleDock(canRawView, ui->mdiArea);
					                    });

    connect(canRawSender, &CanRawSender::dockUndock, this, [this] {
		                        handleDock(canRawSender, ui->mdiArea);
					                    });
}

void MainWindow::setupMdiArea()
{
    canRawView->setWindowTitle("Can Raw View");
    ui->mdiArea->addSubWindow(canRawView);

    canRawSender->setWindowTitle("Can Raw Sender");
    ui->mdiArea->addSubWindow(canRawSender);

    graphView->setWindowTitle("Graph");
    ui->mdiArea->addSubWindow(graphView);

    ui->mdiArea->tileSubWindows();
}
