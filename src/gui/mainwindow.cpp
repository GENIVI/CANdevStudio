#include "mainwindow.h"
#include "log.hpp"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    auto modelRegistry = std::make_shared<QtNodes::DataModelRegistry>();

    canDevice = std::make_shared<CanDevice>();
    graphScene = std::make_shared<QtNodes::FlowScene>(modelRegistry);

    setupMdiArea();
    connectToolbarSignals();
    connectMenuSignals();

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

MainWindow::~MainWindow() {}

void MainWindow::closeEvent(QCloseEvent*)
{
    // Close all undocked windows
    for (QWidget* w : undockWindows) {
        cds_debug("Deleting 0x{:x} widget", reinterpret_cast<uintptr_t>(w));
        w->close();
        delete w;
    }
}

void MainWindow::handleDock(QWidget* component, QMdiArea* mdi)
{
    // check if component is already displayed by mdi area
    if (mdi->subWindowList().contains(static_cast<QMdiSubWindow*>(component->parentWidget()))) {
        // undock
        auto parent = component->parentWidget();
        mdi->removeSubWindow(component); // removeSubwWndow only removes widget, not window

        // component is loosing parent. Add it to list to clear it later
        if (!undockWindows.contains(component)) {
            undockWindows.push_back(component);
            cds_debug("Removing parent from 0x{:x} widget", reinterpret_cast<uintptr_t>(component));
        } else {
            cds_warn("Undocked window already on list!");
        }

        component->show();
        parent->close();
    } else {
        // dock
        mdi->addSubWindow(component)->show();

        // component have parent again. No need to clear it manually.
        if (undockWindows.contains(component)) {
            undockWindows.removeOne(component);
            cds_debug("Adding parent to 0x{:x} widget", reinterpret_cast<uintptr_t>(component));
        } else {
            cds_warn("Docked window is not on the list!");
        }
    }
}

void MainWindow::handleExitAction()
{
    QMessageBox::StandardButton userReply;
    userReply = QMessageBox::question(
        this, "Exit", "Are you shure you want to quit CANdevStudio?", QMessageBox::Yes | QMessageBox::No);
    if (userReply == QMessageBox::Yes)
        QApplication::quit();
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
    QActionGroup* ViewModes = new QActionGroup(this);
    ViewModes->addAction(ui->actionTabView);
    ViewModes->addAction(ui->actionSubWindowView);
    connect(ui->actionAbout, &QAction::triggered, this, [this] { QMessageBox::about(this, "About", "<about body>"); });
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::handleExitAction);
    connect(ui->actionTile, &QAction::triggered, ui->mdiArea, &QMdiArea::tileSubWindows);
    connect(ui->actionCascade, &QAction::triggered, ui->mdiArea, &QMdiArea::cascadeSubWindows);
    connect(ui->actionTabView, &QAction::triggered, this, [this] { ui->mdiArea->setViewMode(QMdiArea::TabbedView); });
    connect(ui->actionTabView, &QAction::toggled, ui->actionTile, &QAction::setDisabled);
    connect(ui->actionTabView, &QAction::toggled, ui->actionCascade, &QAction::setDisabled);
    connect(ui->actionSubWindowView, &QAction::triggered, this,
        [this] { ui->mdiArea->setViewMode(QMdiArea::SubWindowView); });
}

void MainWindow::setupMdiArea()
{
    CanRawView* canRawView = new CanRawView();
    connect(canDevice.get(), &CanDevice::frameReceived, canRawView, &CanRawView::frameReceived);
    connect(canDevice.get(), &CanDevice::frameSent, canRawView, &CanRawView::frameSent);
    connect(ui->actionstart, &QAction::triggered, canRawView, &CanRawView::startSimulation);
    connect(ui->actionstop, &QAction::triggered, canRawView, &CanRawView::stopSimulation);
    //canRawView->setWindowTitle("CANrawView");
    //connect(canRawView, &CanRawView::dockUndock, this, [this, canRawView] { handleDock(canRawView, ui->mdiArea); });
    //ui->mdiArea->addSubWindow(canRawView);

    CanRawSender* canRawSender = new CanRawSender();
    canRawSender->setWindowTitle("CANrawSender");
    connect(canRawSender, &CanRawSender::sendFrame, canDevice.get(), &CanDevice::sendFrame);
    connect(
        canRawSender, &CanRawSender::dockUndock, this, [this, canRawSender] { handleDock(canRawSender, ui->mdiArea); });
    ui->mdiArea->addSubWindow(canRawSender);

    QtNodes::FlowView* graphView = new QtNodes::FlowView(graphScene.get());
    graphView->setWindowTitle("Project Configuration");
    ui->mdiArea->addSubWindow(graphView);

    ui->mdiArea->tileSubWindows();
}
