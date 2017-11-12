
#include "mainwindow.h"
#include "log.h"
#include "modelvisitor.h" // apply_model_visitor
#include "subwindow.h"
#include "ui_mainwindow.h"
#include "ui_toolbar.h"
#include "projectconfigvalidator.h"
#include <nodes/FlowViewStyle>

#include <QCloseEvent>
#include <QtCore/QFile>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QToolBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _ui(std::make_unique<Ui::MainWindow>())
    , _toolBar(std::make_unique<Ui::ToolBar>())
{
    _ui->setupUi(this);
    _ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    QWidget *toolBarWidget = new QWidget;
    _toolBar->setupUi(toolBarWidget);

    QToolBar *bar = new QToolBar();
    bar->addWidget(toolBarWidget);
    bar->setMovable(false);
    addToolBar(bar);

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
        && _ui->mdiArea->subWindowList().contains(static_cast<QMdiSubWindow*>(component->parentWidget()))) {
        cds_debug("Undock action");
        auto parent = component->parentWidget();
        _ui->mdiArea->removeSubWindow(component); // removeSubwWndow only removes widget, not window

        component->show();
        parent->close();
    } else {
        cds_debug("Dock action");
        addToMdi(component);
    }
}

void MainWindow::connectToolbarSignals()
{
    connect(_ui->actionStart, &QAction::triggered, _ui->actionStop, &QAction::setDisabled);
    connect(_ui->actionStart, &QAction::triggered, _ui->actionStart, &QAction::setEnabled);
    connect(_ui->actionStart, &QAction::triggered, _toolBar->toolStop, &QWidget::setDisabled);
    connect(_ui->actionStart, &QAction::triggered, _toolBar->toolStart, &QWidget::setEnabled);
    connect(_ui->actionStop, &QAction::triggered, _ui->actionStop, &QAction::setEnabled);
    connect(_ui->actionStop, &QAction::triggered, _ui->actionStart, &QAction::setDisabled);
    connect(_ui->actionStop, &QAction::triggered, _toolBar->toolStop, &QWidget::setEnabled);
    connect(_ui->actionStop, &QAction::triggered, _toolBar->toolStart, &QWidget::setDisabled);

    connect(_toolBar->toolNew, &QToolButton::clicked, _ui->actionNew, &QAction::trigger);
    connect(_toolBar->toolOpen, &QToolButton::clicked, _ui->actionLoad, &QAction::trigger);
    connect(_toolBar->toolSave, &QToolButton::clicked, _ui->actionSave, &QAction::trigger);
    connect(_toolBar->toolSaveAs, &QToolButton::clicked, _ui->actionSaveAs, &QAction::trigger);
    connect(_toolBar->toolSimulation, &QToolButton::clicked, _ui->actionSimulation, &QAction::trigger);
    connect(_toolBar->toolClose, &QToolButton::clicked, _ui->actionClose, &QAction::trigger);
    connect(_toolBar->toolStart, &QToolButton::clicked, _ui->actionStart, &QAction::trigger);
    connect(_toolBar->toolStop, &QToolButton::clicked, _ui->actionStop, &QAction::trigger);
    connect(_toolBar->toolSwitch, &QToolButton::clicked, _ui->actionSwitchStyle, &QAction::trigger);
    connect(_toolBar->toolTabView, &QToolButton::clicked, _ui->actionTabView, &QAction::trigger);
    connect(_toolBar->toolWindowView, &QToolButton::clicked, _ui->actionSubWindowView, &QAction::trigger);
}

void MainWindow::handleSaveAction()
{
    if (!_projectConfig)
        return;

    if(_projectFile.isEmpty()) {
        handleSaveAsAction();
    } else {
        QFile file(_projectFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(_projectConfig->save()); // FIXME
        } else {
            cds_error("Failed to open file for writing: {}", _projectFile.toStdString());
        }
    }
}

void MainWindow::handleSaveAsAction()
{
    if (!_projectConfig)
        return;

    QString fileName = QFileDialog::getSaveFileName(
        nullptr, "Save project as...", QDir::homePath(), "CANdevStudio Files (*.cds)");

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".cds", Qt::CaseInsensitive))
            fileName += ".cds";

        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(_projectConfig->save()); // FIXME

            QFileInfo fInfo(fileName);
            _projectFile = fileName;
            _projectName = fInfo.completeBaseName();
            _projectConfig->setWindowTitle(_projectName);
        } else {
            cds_error("Failed to open file for writing: {}", _projectFile.toStdString());
        }
    } else {
        cds_debug("Save action cancelled by the user");
    }
}

void MainWindow::handleLoadAction()
{
    QString fileName
        = QFileDialog::getOpenFileName(nullptr, "Open project", QDir::homePath(), "CANdevStudio (*.cds)");

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

    // TODO nodeeditor is not validating data. Improve schema file (using required fields) to be safe.
    if (!ProjectConfigValidator::validateConfiguration(wholeFile))
        return;

    QFileInfo fInfo(fileName);

    if (createProjectConfig(fInfo.completeBaseName())) {
        if (_projectConfig) {
            _projectConfig->load(wholeFile); // FIXME
            _projectFile = fileName;
        } else {
            cds_error("Project config does not exist");
        }
    }
}

bool MainWindow::closeProjectConfig()
{
    if (_projectConfig) {
        // Ask the question only if projet is currently open
        QMessageBox::StandardButton userReply;
        userReply = QMessageBox::question(
            this, "Close Project", "Do you want to close current project?", QMessageBox::Yes | QMessageBox::No);
        if (userReply == QMessageBox::No) {
            return false;
        }

        _projectConfig->clearGraphView();
        _ui->mdiArea->removeSubWindow(_projectConfig.get()->parentWidget());
        _projectConfig.reset();

        _ui->actionClose->setDisabled(true);
        _ui->actionStart->setDisabled(true);
        _ui->actionSave->setDisabled(true);
        _ui->actionSaveAs->setDisabled(true);
        _ui->actionStop->setDisabled(true);
        _ui->actionSimulation->setDisabled(true);
        _toolBar->toolClose->setDisabled(true);
        _toolBar->toolStart->setDisabled(true);
        _toolBar->toolSave->setDisabled(true);
        _toolBar->toolSaveAs->setDisabled(true);
        _toolBar->toolSimulation->setDisabled(true);
        _toolBar->toolStop->setDisabled(true);
    }

    return true;
}

bool MainWindow::createProjectConfig(const QString& name)
{
    if (!closeProjectConfig())
        return false;

    _projectConfig = std::make_unique<ProjectConfig>(this);
    _projectName = name;

    if (_projectConfig) {
        _projectConfig->setWindowTitle(_projectName);
        addToMdi(_projectConfig.get());

        connect(_ui->actionStop, &QAction::triggered, _projectConfig.get(), &ProjectConfig::stopSimulation);
        connect(_ui->actionStart, &QAction::triggered, _projectConfig.get(), &ProjectConfig::startSimulation);
        connect(_projectConfig.get(), &ProjectConfig::handleDock, this, &MainWindow::handleDock);
        connect(_projectConfig.get(), &ProjectConfig::handleWidgetDeletion, this, &MainWindow::handleWidgetDeletion);
        connect(_projectConfig.get(), &ProjectConfig::handleWidgetShowing, this, &MainWindow::handleWidgetShowing);
        connect(_projectConfig.get(), &ProjectConfig::closeProject, this, &MainWindow::closeProjectConfig);

        _ui->actionClose->setDisabled(false);
        _ui->actionStart->setDisabled(false);
        _ui->actionSave->setDisabled(false);
        _ui->actionSaveAs->setDisabled(false);
        _ui->actionSimulation->setDisabled(false);
        _ui->actionStop->setDisabled(true);
        _toolBar->toolClose->setDisabled(false);
        _toolBar->toolStart->setDisabled(false);
        _toolBar->toolSave->setDisabled(false);
        _toolBar->toolSaveAs->setDisabled(false);
        _toolBar->toolSimulation->setDisabled(false);
        _toolBar->toolStop->setDisabled(true);

        return true;
    } else {
        cds_error("Failed to create project config");
    }

    return false;
}

void MainWindow::connectMenuSignals()
{
    QActionGroup* ViewModes = new QActionGroup(this);
    ViewModes->addAction(_ui->actionTabView);
    ViewModes->addAction(_ui->actionSubWindowView);
    connect(_ui->actionAbout, &QAction::triggered, this, [this] { QMessageBox::about(this, "About", "<about body>"); });
    connect(_ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(_ui->actionLoad, &QAction::triggered, this, &MainWindow::handleLoadAction);
    connect(_ui->actionSave, &QAction::triggered, this, &MainWindow::handleSaveAction);
    connect(_ui->actionSaveAs, &QAction::triggered, this, &MainWindow::handleSaveAsAction);
    connect(_ui->actionTile, &QAction::triggered, _ui->mdiArea, &QMdiArea::tileSubWindows);
    connect(_ui->actionCascade, &QAction::triggered, _ui->mdiArea, &QMdiArea::cascadeSubWindows);
    connect(_ui->actionTabView, &QAction::triggered, this, [this] { _ui->mdiArea->setViewMode(QMdiArea::TabbedView); });
    connect(_ui->actionTabView, &QAction::triggered, _toolBar->toolTabView, &QToolButton::setDisabled);
    connect(_ui->actionTabView, &QAction::triggered, _toolBar->toolWindowView, &QToolButton::setEnabled);
    connect(_ui->actionTabView, &QAction::toggled, _ui->actionTile, &QAction::setDisabled);
    connect(_ui->actionTabView, &QAction::toggled, _ui->actionCascade, &QAction::setDisabled);
    connect(_ui->actionSubWindowView, &QAction::triggered, this,
        [this] { _ui->mdiArea->setViewMode(QMdiArea::SubWindowView); });
    connect(_ui->actionSubWindowView, &QAction::triggered, _toolBar->toolWindowView, &QToolButton::setDisabled);
    connect(_ui->actionSubWindowView, &QAction::triggered, _toolBar->toolTabView, &QToolButton::setEnabled);
    connect(_ui->actionClose, &QAction::triggered, this, &MainWindow::closeProjectConfig);
    connect(_ui->actionNew, &QAction::triggered, [this] { createProjectConfig("New Project"); });
    connect(_ui->actionSimulation, &QAction::triggered, [this] { handleWidgetShowing(_projectConfig.get(), true); });
    connect(_ui->actionSwitchStyle, & QAction::triggered, this, &MainWindow::switchStyle);
}

void MainWindow::addToMdi(QWidget* component)
{
    auto wnd = new SubWindow(_ui->mdiArea);
    wnd->setWidget(component);
    component->show();
}

void MainWindow::setupMdiArea()
{
    _ui->mdiArea->setViewMode(QMdiArea::TabbedView);
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

void MainWindow::switchStyle()
{
    if (currentStyle == Styles::darkStyle)
        setStyle(Styles::lightStyle);
    else
        setStyle(Styles::darkStyle);
}

void MainWindow::setStyle(Styles style)
{
    QString stylefile;
    QString flowStyle;

    switch (style)
    {
    case Styles::darkStyle :
    {
        stylefile = ":/files/css/darkStyle.css";
        flowStyle = R"(
        {
          "FlowViewStyle": {
              "BackgroundColor": [38, 38, 38],
              "FineGridColor": [30, 30, 30],
              "CoarseGridColor": [20, 20, 20]
          }
        }
        )";
    } break;
    case Styles::lightStyle :
    {
        stylefile = ":/files/css/lightStyle.css";
        flowStyle = R"(
        {
          "FlowViewStyle": {
              "BackgroundColor": [229, 229, 229],
              "FineGridColor": [220, 220, 220],
              "CoarseGridColor": [210, 210, 210]
          }
        }
        )";
    } break;
    }

    QFile f(stylefile);
    f.open(QFile::ReadOnly);
    QString css = QLatin1String(f.readAll());
    qApp->setStyleSheet(css);

    QtNodes::FlowViewStyle::setStyle(flowStyle);

    currentStyle = style;
}
