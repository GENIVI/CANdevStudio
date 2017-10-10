
#include "mainwindow.h"
#include "log.h"
#include "modelvisitor.h" // apply_model_visitor
#include "subwindow.h"
#include "ui_mainwindow.h"
#include "projectconfigvalidator.h"

#include <QCloseEvent>
#include <QtCore/QFile>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);
    ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    setupMdiArea();
    connectToolbarSignals();
    connectMenuSignals();
}

MainWindow::~MainWindow()
{
} // NOTE: Qt MOC requires this code

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

void MainWindow::handleDock(QWidget* component)
{
    // check if component is already displayed by mdi area
    if (component->parentWidget()
        && ui->mdiArea->subWindowList().contains(static_cast<QMdiSubWindow*>(component->parentWidget()))) {
        cds_debug("Undock action");
        auto parent = component->parentWidget();
        ui->mdiArea->removeSubWindow(component); // removeSubwWndow only removes widget, not window

        component->show();
        parent->close();
    } else {
        cds_debug("Dock action");
        addToMdi(component);
    }
}

void MainWindow::connectToolbarSignals()
{
    connect(ui->actionStart, &QAction::triggered, ui->actionStop, &QAction::setDisabled);
    connect(ui->actionStart, &QAction::triggered, ui->actionStart, &QAction::setEnabled);
    connect(ui->actionStop, &QAction::triggered, ui->actionStop, &QAction::setEnabled);
    connect(ui->actionStop, &QAction::triggered, ui->actionStart, &QAction::setDisabled);
}

void MainWindow::handleSaveAction()
{
    if (!projectConfig)
        return;

    QString fileName = QFileDialog::getSaveFileName(
        nullptr, "Project Configuration", QDir::homePath(), "CANdevStudio Files (*.cds)");

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".cds", Qt::CaseInsensitive))
            fileName += ".cds";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(projectConfig->save()); // FIXME
        }
    } else {
        cds_debug("Save action cancelled by the user");
    }
}

void MainWindow::handleLoadAction()
{
    QString fileName
        = QFileDialog::getOpenFileName(nullptr, "Project Configuration", QDir::homePath(), "CANdevStudio (*.cds)");

    if (fileName.isEmpty()) {
        cds_debug("Load action cancelled by the user");
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        cds_error("Could not open file");
        return;
    }

    QByteArray wholeFile = file.readAll();

    if (!ProjectConfigValidator::validateConfiguration(wholeFile))
        return;

    // TODO check if file is correct, nodeeditor library does not provide it and will crash if incorrect file is
    // supplied

    // TODO: Customize project name
    if (createProjectConfig("Project Config")) {
        if (projectConfig) {
            projectConfig->load(wholeFile); // FIXME
        } else {
            cds_error("Project config does not exist");
        }
    }
}

bool MainWindow::closeProjectConfig()
{
    if (projectConfig) {
        // Ask the question only if projet is currently open
        QMessageBox::StandardButton userReply;
        userReply = QMessageBox::question(
            this, "Close Project", "Do you want to close current project?", QMessageBox::Yes | QMessageBox::No);
        if (userReply == QMessageBox::No) {
            return false;
        }

        projectConfig->clearGraphView();
        ui->mdiArea->removeSubWindow(projectConfig.get()->parentWidget());
        projectConfig.reset();

        ui->actionClose->setDisabled(true);
        ui->actionStart->setDisabled(true);
        ui->actionSave->setDisabled(true);
        ui->actionStop->setDisabled(true);
        ui->actionSimulation->setDisabled(true);
    }

    return true;
}

bool MainWindow::createProjectConfig(const QString& name)
{
    if (!closeProjectConfig())
        return false;

    projectConfig = std::make_unique<ProjectConfig>(this);

    if (projectConfig) {
        projectConfig->setWindowTitle(name);
        addToMdi(projectConfig.get());

        connect(ui->actionStop, &QAction::triggered, projectConfig.get(), &ProjectConfig::stopSimulation);
        connect(ui->actionStart, &QAction::triggered, projectConfig.get(), &ProjectConfig::startSimulation);
        connect(projectConfig.get(), &ProjectConfig::handleDock, this, &MainWindow::handleDock);
        connect(projectConfig.get(), &ProjectConfig::handleWidgetDeletion, this, &MainWindow::handleWidgetDeletion);
        connect(projectConfig.get(), &ProjectConfig::handleWidgetShowing, this, &MainWindow::handleWidgetShowing);
        connect(projectConfig.get(), &ProjectConfig::closeProject, this, &MainWindow::closeProjectConfig);

        ui->actionClose->setDisabled(false);
        ui->actionStart->setDisabled(false);
        ui->actionSave->setDisabled(false);
        ui->actionSimulation->setDisabled(false);
        ui->actionStop->setDisabled(true);

        return true;
    } else {
        cds_error("Failed to create project config");
    }

    return false;
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
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeProjectConfig);
    connect(ui->actionNew, &QAction::triggered, [this] { createProjectConfig("Project Config"); });
    connect(ui->actionSimulation, &QAction::triggered, [this] { handleWidgetShowing(projectConfig.get(), true); });
}

void MainWindow::addToMdi(QWidget* component)
{
    auto wnd = new SubWindow(ui->mdiArea);
    wnd->setWidget(component);
    component->show();
}

void MainWindow::setupMdiArea()
{
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
}

void MainWindow::handleWidgetDeletion(QWidget* widget)
{
    if (!widget)
        return;

    if (widget->parentWidget()) {
        widget->parentWidget()->close();
    } else {
        widget->close();
    }
}

void MainWindow::handleWidgetShowing(QWidget* widget, bool docked)
{
    if (!widget)
        return;

    // Add widget to MDI area when showing for the first time
    // Widget will be also added to MDI area after closing it in undocked state
    if (!widget->isVisible() && docked) {
        cds_debug("Adding '{}' widget to MDI", widget->windowTitle().toStdString());
        addToMdi(widget);
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

