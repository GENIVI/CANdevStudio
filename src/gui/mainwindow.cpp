#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "candevice/candevice.h"
#include "mainwindow.h"
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>
#include <QInputDialog>

#include <log.hpp>


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    canRawView = new CanRawView();
    canRawSender = new CanRawSender();

    canRawView->setWindowTitle("Can Raw View");
    ui->mdiArea->addSubWindow(canRawView);

    canRawSender->setWindowTitle("Can Raw Sender");
    ui->mdiArea->addSubWindow(canRawSender);

    ui->mdiArea->tileSubWindows();

    connect(ui->actionstart, &QAction::triggered, canRawView, &CanRawView::startSimulation);
    connect(ui->actionstop, &QAction::triggered, canRawView, &CanRawView::stopSimulation);

    connect(ui->actionstart, &QAction::triggered, ui->actionstop, &QAction::setDisabled);
    connect(ui->actionstart, &QAction::triggered, ui->actionstart, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstop, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstart, &QAction::setDisabled);

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

    //docking signals connection
    connect(canRawView, &CanRawView::dockUndock, this, [this] {
        handleDock(this->canRawView, ui->mdiArea);
    });
    connect(canRawSender, &CanRawSender::dockUndock, this, [this] {
        handleDock(this->canRawSender, ui->mdiArea);
    });

}

MainWindow::~MainWindow()
{
    delete canRawView;
    delete canRawSender;
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

void MainWindow::showEvent(QShowEvent* event [[gnu::unused]])
{
    emit requestAvailableDevices("socketcan"); // FIXME: plugin name should be configurable
}

void MainWindow::availableDevices(QString backend,
        QList<QCanBusDeviceInfo> devices)
{
    QStringList items;

    if (devices.empty())
    {
        cds_error("Received empty devices list");
        return;
    }

    for (const auto& d: devices)
        items.push_back(d.name());

    bool ok;
    QString item = QInputDialog::getItem(this, tr("Choose CAN device"),
                                         tr("Available CAN devices on %1 backend").arg(backend),
                                         items, 0, false, &ok);

    emit selectCANDevice(backend, item);
}

void MainWindow::attachToViews(CanDevice* device)
{
    QObject::connect(device, &CanDevice::frameReceived, canRawView, &CanRawView::frameReceived);
    QObject::connect(device, &CanDevice::frameSent, canRawView, &CanRawView::frameSent);
    QObject::connect(canRawSender, &CanRawSender::sendFrame, device, &CanDevice::sendFrame);

    ui->actionstart->setEnabled(true);
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
