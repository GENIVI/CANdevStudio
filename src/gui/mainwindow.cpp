#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "candevice/candevice.h"
#include "canrawsender/canrawsender.h"
#include "canrawview/canrawview.h"
#include "cansignalsender/cansignalsender.h"
#include "cansignalview/cansignalview.h"
#include "mainwindow.h"
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

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

    connect(canRawSender, &CanRawSender::sendFrame, canDevice, &CanDevice::sendFrame);

    connect(ui->actionstart, SIGNAL(triggered(bool)), ui->actionstop, SLOT(setDisabled(bool)));
    connect(ui->actionstart, SIGNAL(triggered(bool)), ui->actionstart, SLOT(setEnabled(bool)));
    connect(ui->actionstop, SIGNAL(triggered(bool)), ui->actionstop, SLOT(setEnabled(bool)));
    connect(ui->actionstop, SIGNAL(triggered(bool)), ui->actionstart, SLOT(setDisabled(bool)));

    //docking signals connection
    connect(canRawView, &CanRawView::dockUndock, this, [this, canRawView] {
                        handleDock(canRawView, ui->mdiArea);
		    });
    connect(canSignalView, &CanSignalView::dockUndock, this, [this, canSignalView] {
		        handleDock(canSignalView, ui->mdiArea);
		    });
    connect(canSignalSender, &CanSignalSender::dockUndock, this, [this, canSignalSender] {
		        handleDock(canSignalSender, ui->mdiArea);
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
