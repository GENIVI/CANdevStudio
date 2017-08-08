#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "mainwindow.h"
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>
#include <qfiledialog.h>
#include <qfile.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);
    ui->menuBar->setNativeMenuBar(false);
    CanFactoryQt factory;
    CanDevice* canDevice = new CanDevice(factory);
    CanRawView* canRawView = new CanRawView();
    CanRawSender* canRawSender = new CanRawSender();

    canRawView->setWindowTitle("Can Raw View");
    ui->mdiArea->addSubWindow(canRawView);

    canRawSender->setWindowTitle("Can Raw Sender");
    ui->mdiArea->addSubWindow(canRawSender);

    ui->mdiArea->tileSubWindows();

    connect(canDevice, &CanDevice::frameReceived, canRawView, &CanRawView::frameReceived);
    connect(canDevice, &CanDevice::frameSent, canRawView, &CanRawView::frameSent);
    connect(ui->actionstart, &QAction::triggered, canRawView, &CanRawView::startSimulation);
    connect(ui->actionstop, &QAction::triggered, canRawView, &CanRawView::stopSimulation);
    connect(canRawSender, &CanRawSender::sendFrame, canDevice, &CanDevice::sendFrame);

    connect(ui->actionstart, &QAction::triggered, ui->actionstop, &QAction::setDisabled);
    connect(ui->actionstart, &QAction::triggered, ui->actionstart, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstop, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstart, &QAction::setDisabled);

    QActionGroup *ViewModes = new QActionGroup(this);
    ViewModes->addAction(ui->actionTabView);
    ViewModes->addAction(ui->actionSubWindowView);
    connect(ui->actionAbout,&QAction::triggered,this,[this] { QMessageBox::about(this,"About","<about body>"); });
    connect(ui->actionExit,&QAction::triggered,this,&MainWindow::handleExitAction);
    connect(ui->actionLoad,&QAction::triggered,this,&MainWindow::handleLoadAction);
    connect(ui->actionTile,&QAction::triggered,ui->mdiArea,&QMdiArea::tileSubWindows);
    connect(ui->actionCascade,&QAction::triggered,ui->mdiArea,&QMdiArea::cascadeSubWindows);
    connect(ui->actionTabView,&QAction::triggered,this,[this]{ ui->mdiArea->setViewMode(QMdiArea::TabbedView); });
    connect(ui->actionTabView,&QAction::toggled,ui->actionTile,&QAction::setDisabled);
    connect(ui->actionTabView,&QAction::toggled,ui->actionCascade,&QAction::setDisabled);
    connect(ui->actionSubWindowView,&QAction::triggered,this,[this]{ ui->mdiArea->setViewMode(QMdiArea::SubWindowView); });

    //docking signals connection
    connect(canRawView, &CanRawView::dockUndock, this, [this, canRawView] {
                        handleDock(canRawView, ui->mdiArea);
		    });
    connect(canRawSender, &CanRawSender::dockUndock, this, [this, canRawSender] {
			handleDock(canRawSender, ui->mdiArea);
		    });
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

void MainWindow::handleLoadAction()
{
    try
    {
        QString fileData;
        QString fileName = QFileDialog::getOpenFileName(this,tr("Open CDS file"), "", tr("CDS file (*.cds)"));
        QFile file(fileName);

        if( !file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            throw "Could not open file";
        }

        while(!file.atEnd())
        {
            fileData += file.readLine();
        }

        /*to do
         *
        */
    }
    catch (QString str)
    {
        QMessageBox::question(this, "Error", str);
    }
    catch(...)
    {
        QMessageBox::question(this, "Unknown Error", QString::fromStdString("<error body>"));
    }

}
