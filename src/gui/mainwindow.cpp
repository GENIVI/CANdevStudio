
#include "mainwindow.h"
#include "log.h"
#include "modelvisitor.h" // apply_model_visitor
#include "subwindow.h"
#include "ui_mainwindow.h"

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

    projectConfiguration = std::make_unique<ProjectConfiguration>();

    setupMdiArea();
    connectToolbarSignals();
    connectMenuSignals();
}

MainWindow::~MainWindow() {}  // NOTE: Qt MOC requires this code

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
        componentWidgetCreated(component);
    }
}

void MainWindow::connectToolbarSignals()
{
    connect(ui->actionstart, &QAction::triggered, ui->actionstop, &QAction::setDisabled);
    connect(ui->actionstart, &QAction::triggered, ui->actionstart, &QAction::setEnabled);
    connect(ui->actionstart, &QAction::triggered, projectConfiguration.get(), &ProjectConfiguration::startSimulation);
    connect(ui->actionstop, &QAction::triggered, ui->actionstop, &QAction::setEnabled);
    connect(ui->actionstop, &QAction::triggered, ui->actionstart, &QAction::setDisabled);
    connect(ui->actionstop, &QAction::triggered, projectConfiguration.get(), &ProjectConfiguration::stopSimulation);
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
            file.write(projectConfiguration->save()); // FIXME
        }
    } else {
        cds_error("File name empty");
    }
}

void MainWindow::handleLoadAction()
{
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

    projectConfiguration->clearGraphView();
    projectConfiguration->load(wholeFile); // FIXME
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

void MainWindow::componentWidgetCreated(QWidget* component)
{
    auto wnd = new SubWindow;
    // It seems we need to add Window first before setting the widget
    ui->mdiArea->addSubWindow(wnd);
    wnd->setWidget(component);
    // We need to delete the window to remove it from tabView when closed
    wnd->setAttribute(Qt::WA_DeleteOnClose);
}

void MainWindow::setupMdiArea()
{
    projectConfiguration->setWindowTitle("Project Configuration");
    ui->mdiArea->addSubWindow(projectConfiguration.get());
    ui->mdiArea->setAttribute(Qt::WA_DeleteOnClose, false);
    ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    connect(projectConfiguration.get(), &ProjectConfiguration::componentWidgetCreated, this,
        &MainWindow::componentWidgetCreated);
    connect(projectConfiguration.get(), &ProjectConfiguration::handleDock, this, &MainWindow::handleDock);
}
