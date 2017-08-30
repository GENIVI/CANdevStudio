
#include "mainwindow.h"
#include "log.hpp"
#include "modelvisitor.h" // apply_model_visitor
#include "subwindow.hpp"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QtCore/QFile>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>

#include <cassert> // assert
#include <iostream>

#include <candevicemodel.h>
#include <canrawsendermodel.h>
#include <canrawviewmodel.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    auto modelRegistry = std::make_shared<QtNodes::DataModelRegistry>();
    modelRegistry->registerModel<CanDeviceModel>();
    modelRegistry->registerModel<CanRawSenderModel>();
    modelRegistry->registerModel<CanRawViewModel>();

    graphScene = std::make_shared<QtNodes::FlowScene>(modelRegistry);

    connect(graphScene.get(), &QtNodes::FlowScene::nodeCreated, this, &MainWindow::nodeCreatedCallback);
    connect(graphScene.get(), &QtNodes::FlowScene::nodeDeleted, this, &MainWindow::nodeDeletedCallback);
    connect(graphScene.get(), &QtNodes::FlowScene::nodeDoubleClicked, this, &MainWindow::nodeDoubleClickedCallback);

    setupMdiArea();
    connectToolbarSignals();
    connectMenuSignals();
}

MainWindow::~MainWindow() {}

void MainWindow::closeEvent(QCloseEvent* e)
{
    QMessageBox::StandardButton userReply;
    userReply = QMessageBox::question(
        this, "Exit", "Are you sure you want to quit CANdevStudio?", QMessageBox::Yes | QMessageBox::No);
    if (userReply == QMessageBox::Yes) {
        QApplication::quit();
    } else {
        e->ignore();
    }
}

void MainWindow::nodeCreatedCallback(QtNodes::Node& node)
{
    auto dataModel = node.nodeDataModel();

    assert(nullptr != dataModel);

    apply_model_visitor(*dataModel,
        [this, dataModel](CanRawViewModel& m) {
            auto rawView = &m.canRawView;
            connect(ui->actionstart, &QAction::triggered, rawView, &CanRawView::startSimulation);
            connect(ui->actionstop, &QAction::triggered, rawView, &CanRawView::stopSimulation);
            connect(rawView, &CanRawView::dockUndock, this, [this, rawView] { handleDock(rawView, ui->mdiArea); });
        },
        [this, dataModel](CanRawSenderModel& m) {
            QWidget* crsWidget = m.canRawSender.getMainWidget();
            auto& rawSender = m.canRawSender;
            connect(
                &rawSender, &CanRawSender::dockUndock, this, [this, crsWidget] { handleDock(crsWidget, ui->mdiArea); });
            connect(ui->actionstart, &QAction::triggered, &rawSender, &CanRawSender::startSimulation);
            connect(ui->actionstop, &QAction::triggered, &rawSender, &CanRawSender::stopSimulation);
        },
        [this](CanDeviceModel&) {});
}

void handleWidgetDeletion(QWidget* widget)
{
    assert(nullptr != widget);
    if (widget->parentWidget()) {

        widget->parentWidget()->close();
    } // else path not needed
}

void MainWindow::nodeDeletedCallback(QtNodes::Node& node)
{
    auto dataModel = node.nodeDataModel();

    assert(nullptr != dataModel);

    apply_model_visitor(*dataModel, [this, dataModel](CanRawViewModel& m) { handleWidgetDeletion(&m.canRawView); },
        [this, dataModel](CanRawSenderModel& m) { handleWidgetDeletion(m.canRawSender.getMainWidget()); },
        [this](CanDeviceModel&) {});
}

void handleWidgetShowing(QWidget* widget, QMdiArea* mdi)
{
    assert(nullptr != widget);
    cds_debug("Subwindows cnt: {}", mdi->subWindowList().size());
    bool docked = false;

    // TODO: Temporary solution. To be changed once MainWindow is refactored
    QPushButton *undockButton = widget->findChild<QPushButton*>("pbDockUndock");
    if(undockButton) {
        docked = !undockButton->isChecked();
    }
    else {
        cds_debug("Undock button for '{}' widget not found", widget->windowTitle().toStdString());
    }

    // Add widget to MDI area when showing for the first time
    // Widget will be also added to MDI area after closing it in undocked state
    if (!widget->isVisible() && docked) {
        cds_debug("Adding '{}' widget to MDI", widget->windowTitle().toStdString());
        auto wnd = new SubWindow(widget);
        // We need to delete the window to remove it from tabView when closed
        wnd->setAttribute(Qt::WA_DeleteOnClose);
        mdi->addSubWindow(wnd);
    }

    if (widget->parentWidget()) {
        cds_debug("Widget is a part of MDI");
        widget->hide();
        widget->show();
    } else {
        cds_debug("Widget not a part of MDI");
        widget->show();
        widget->activateWindow();
    }
}

void MainWindow::nodeDoubleClickedCallback(QtNodes::Node& node)
{
    auto dataModel = node.nodeDataModel();

    assert(nullptr != dataModel);

    apply_model_visitor(*dataModel,
        [this, dataModel](CanRawViewModel& m) { handleWidgetShowing(&m.canRawView, ui->mdiArea); },
        [this, dataModel](CanRawSenderModel& m) { handleWidgetShowing(m.canRawSender.getMainWidget(), ui->mdiArea); },
        [this](CanDeviceModel&) {});
}

void MainWindow::handleDock(QWidget* component, QMdiArea* mdi)
{
    // check if component is already displayed by mdi area
    if (mdi->subWindowList().contains(static_cast<QMdiSubWindow*>(component->parentWidget()))) {
        // undock
        auto parent = component->parentWidget();
        mdi->removeSubWindow(component); // removeSubwWndow only removes widget, not window

        component->show();
        parent->close();
    } else {
        // dock
        mdi->addSubWindow(component)->show();
    }
}

void MainWindow::connectToolbarSignals()
{
    connect(ui->actionstart, &QAction::triggered, ui->actionstop, &QAction::setDisabled);
    connect(ui->actionstart, &QAction::triggered, ui->actionstart, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstop, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstart, &QAction::setDisabled);
}

void MainWindow::handleSaveAction()
{
    QString fileName = QFileDialog::getSaveFileName(
        nullptr, "Project Configuration", QDir::homePath(), "CANdevStudio Files (*.cds)");

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".cds", Qt::CaseInsensitive))
            fileName += ".cds";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(graphScene->saveToMemory()); // FIXME
        }
    } else {
        cds_error("File name empty");
    }
}

void MainWindow::handleLoadAction()
{
    graphScene->clearScene();

    QString fileName
        = QFileDialog::getOpenFileName(nullptr, "Project Configuration", QDir::homePath(), "CANdevStudio (*.cds)");

    if (!QFileInfo::exists(fileName)) {
        cds_error("File does not exist");
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        cds_error("Could not open file");
        return;
    }

    QByteArray wholeFile = file.readAll();

    // TODO check if file is correct, nodeeditor library does not provide it and will crash if incorrect file is
    // supplied

    graphScene->loadFromMemory(wholeFile); // FIXME
}

void MainWindow::connectMenuSignals()
{
    QActionGroup* ViewModes = new QActionGroup(this);
    ViewModes->addAction(ui->actionTabView);
    ViewModes->addAction(ui->actionSubWindowView);
    connect(ui->actionAbout, &QAction::triggered, this, [this] { QMessageBox::about(this, "About", "<about body>"); });
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::handleLoadAction);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::handleSaveAction);
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
    graphView = new QtNodes::FlowView(graphScene.get());
    graphView->setWindowTitle("Project Configuration");
    ui->mdiArea->addSubWindow(graphView);
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
}
