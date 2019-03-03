#include "mainwindow.h"
#include "log.h"
#include "modelvisitor.h" // apply_model_visitor
#include "projectconfigvalidator.h"
#include "subwindow.h"
#include "ui_mainwindow.h"
#include "ui_toolbar.h"
#include "ui_aboutdialog.h"
#include <nodes/FlowViewStyle>

#include <QCloseEvent>
#include <QMenu>
#include <QDesktopServices>
#include <QtCore/QFile>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolButton>
#include <QGraphicsDropShadowEffect>
#include <version.h>
#include <nodes/ConnectionStyle>

namespace {
const QString SETTINGS_STYLE_TAG = "style";
const QString SETTINGS_RECENT_TAG = "recentProjects";
const QString SETTINGS_NAME_TAG = "name";
const QString SETTINGS_FILENAME_TAG = "filename";
const int SETTINGS_RECENT_SIZE = 20;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _ui(std::make_unique<Ui::MainWindow>())
    , _toolBar(std::make_unique<Ui::ToolBar>())
    , _settings("CANdevStudio")
{
    _ui->setupUi(this);
    _ui->centralWidget->layout()->setContentsMargins(0, 0, 0, 0);

    QWidget* toolBarWidget = new QWidget;
    _toolBar->setupUi(toolBarWidget);

    QToolBar* bar = new QToolBar();
    bar->addWidget(toolBarWidget);
    bar->setMovable(false);
    addToolBar(bar);

    _ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    _ui->mdiArea->hide();
    _ui->pbStartNew->setCursor(Qt::PointingHandCursor);
    _ui->pbStartOpen->setCursor(Qt::PointingHandCursor);

    QGraphicsDropShadowEffect* ef = new QGraphicsDropShadowEffect;
    ef->setBlurRadius(50);
    ef->setOffset(0);
    _ui->startScreen->setGraphicsEffect(ef);

    connectToolbarSignals();
    connectMenuSignals();

    _recentProjectsButtons.push_back({ _ui->pbRecent1Name, _ui->pbRecent1File });
    _recentProjectsButtons.push_back({ _ui->pbRecent2Name, _ui->pbRecent2File });
    _recentProjectsButtons.push_back({ _ui->pbRecent3Name, _ui->pbRecent3File });
    _recentProjectsButtons.push_back({ _ui->pbRecent4Name, _ui->pbRecent4File });
    _recentProjectsButtons.push_back({ _ui->pbRecent5Name, _ui->pbRecent5File });
    _recentProjectsButtons.push_back({ _ui->pbRecent6Name, _ui->pbRecent6File });


    for(int i = 0; i < _recentProjectsButtons.size(); ++i) {
        connect(_recentProjectsButtons[i].first, &QPushButton::clicked, [this, i] {
           handleRecentProject(i);
        });
        connect(_recentProjectsButtons[i].second, &QPushButton::clicked, [this, i] {
           handleRecentProject(i);
        });

        _recentProjectsButtons[i].first->setCursor(Qt::PointingHandCursor);
        _recentProjectsButtons[i].second->setCursor(Qt::PointingHandCursor);
    }

    loadSettings();

    // loadSettings will update _recentProjects;
    refreshRecentProjects();
    // loadSettings will update _currentStyle;
    setStyle(_currentStyle);
}

MainWindow::~MainWindow() {} // NOTE: Qt MOC requires this code

void MainWindow::refreshRecentProjects()
{
    for(int i = 0; i < _recentProjectsButtons.size(); ++i)
    {
        if(i < _recentProjects.size()) {
            _recentProjectsButtons[i].first->setText(_recentProjects[i].first);
            _recentProjectsButtons[i].first->setToolTip(_recentProjects[i].first);
            _recentProjectsButtons[i].second->setText(_recentProjects[i].second);
            _recentProjectsButtons[i].second->setToolTip(_recentProjects[i].second);
        } else {
            _recentProjectsButtons[i].first->setText("");
            _recentProjectsButtons[i].first->setToolTip("");
            _recentProjectsButtons[i].second->setText("");
            _recentProjectsButtons[i].second->setToolTip("");
        }
    }

    _ui->menuRecent_Projects->clear();
    for(int i = 0; i < _recentProjects.size(); ++i) {
        _ui->menuRecent_Projects->addAction(_recentProjects[i].second, [this, i] {handleRecentProject(i);});
    }
    _ui->menuRecent_Projects->addSeparator();
    _ui->menuRecent_Projects->addAction(_ui->actionClear);
}

void MainWindow::handleRecentProject(int ndx)
{
    if(ndx >= _recentProjects.size()) {
        return;
    }

    QString projectName = _recentProjects[ndx].first;
    QString projectFile = _recentProjects[ndx].second;

    QFile file(projectFile);

    if (!file.open(QIODevice::ReadOnly)) {
        cds_error("Could not open file: {}", projectFile.toStdString());

        auto userReply = QMessageBox::warning(
                    this, "Project not found", "File " + projectFile + " not found. Do you want to remove it from the recent list?",
                    QMessageBox::Yes | QMessageBox::No);

        if (userReply == QMessageBox::Yes) {
            _recentProjects.remove(ndx);
            refreshRecentProjects();
        }

        return;
    }

    QByteArray wholeFile = file.readAll();


    if (createProjectConfig(projectName)) {
        if (_projectConfig) {
            _projectConfig->load(wholeFile); // FIXME
            _projectFile = projectFile;

            addRecentProject(projectName, projectFile);
        } else {
            cds_error("Project config does not exist");
        }
    }

    file.close();

    refreshRecentProjects();
 }

void MainWindow::loadSettings()
{
    _currentStyle
        = static_cast<Styles>(_settings.value(SETTINGS_STYLE_TAG, static_cast<int>(Styles::lightStyle)).toInt());

    _recentProjects.clear();

    int recentSize = _settings.beginReadArray(SETTINGS_RECENT_TAG);
    if(recentSize > SETTINGS_RECENT_SIZE) {
        recentSize = SETTINGS_RECENT_SIZE;
    }

    for(int i = 0; i < recentSize; ++i) {
        _settings.setArrayIndex(i);
        QString name = _settings.value(SETTINGS_NAME_TAG).toString();
        QString filename = _settings.value(SETTINGS_FILENAME_TAG).toString();
        _recentProjects.push_back( {name, filename} );
    }
    _settings.endArray();

}

void MainWindow::saveSettings()
{
    _settings.clear();

    _settings.setValue(SETTINGS_STYLE_TAG, static_cast<int>(_currentStyle));

    int recentSize = _recentProjects.size();
    if(recentSize > SETTINGS_RECENT_SIZE) {
        recentSize = SETTINGS_RECENT_SIZE;
    }

    _settings.beginWriteArray(SETTINGS_RECENT_TAG);
    for(int i = 0; i < recentSize; ++i) {
        _settings.setArrayIndex(i);
        _settings.setValue(SETTINGS_NAME_TAG, _recentProjects[i].first);
        _settings.setValue(SETTINGS_FILENAME_TAG, _recentProjects[i].second);
    }
    _settings.endArray();

    _settings.sync();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    QMessageBox::StandardButton userReply;
    userReply = QMessageBox::question(
        this, "Exit", "Are you sure you want to quit CANdevStudio?", QMessageBox::Yes | QMessageBox::No);
    if (userReply == QMessageBox::Yes) {
        saveSettings();
        QApplication::quit();
    } else {
        e->ignore();
    }
}

void MainWindow::handleDock(QWidget* component)
{ // check if component is already displayed by mdi area
    if (component->parentWidget()
        && _ui->mdiArea->subWindowList().contains(static_cast<QMdiSubWindow*>(component->parentWidget()))) {
        cds_debug("Undock action");
        auto parent = component->parentWidget();
        _ui->mdiArea->removeSubWindow(component); // removeSubWindow only removes widget, not window

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

    connect(_ui->pbStartNew, &QPushButton::clicked, _ui->actionNew, &QAction::trigger);
    connect(_ui->pbStartOpen, &QPushButton::clicked, _ui->actionLoad, &QAction::trigger);
}

void MainWindow::addRecentProject(const QString &name, const QString &path)
{
    // Do not allow duplicates. Most recent project must be first on the list
    for(auto &row : _recentProjects) {
        if(row.second == path) {
            cds_debug("Project '{}' ({}) already found in recent list", row.first.toStdString(), row.second.toStdString());
            _recentProjects.removeOne(row);
        }
    }

    _recentProjects.push_front({ name, path });

    int ndx = 0;
    cds_debug("Recent project list:");
    for(auto &row : _recentProjects) {
        cds_debug("{}. '{}', {}", ++ndx, row.first.toStdString(), row.second.toStdString());
    }
}

void MainWindow::handleSaveAction()
{
    if (!_projectConfig)
        return;

    if (_projectFile.isEmpty()) {
        handleSaveAsAction();
    } else {
        QFile file(_projectFile);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(_projectConfig->save()); // FIXME

            addRecentProject(_projectName, _projectFile);
        } else {
            cds_error("Failed to open file for writing: {}", _projectFile.toStdString());
        }
    }

    refreshRecentProjects();
}

void MainWindow::handleSaveAsAction()
{
    if (!_projectConfig)
        return;

    QString fileName
        = QFileDialog::getSaveFileName(nullptr, "Save project as...", QDir::homePath(), "CANdevStudio Files (*.cds)");

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

            addRecentProject(_projectName, _projectFile);
        } else {
            cds_error("Failed to open file for writing: {}", _projectFile.toStdString());
        }
    } else {
        cds_debug("Save action cancelled by the user");
    }

    refreshRecentProjects();
}

void MainWindow::handleLoadAction()
{
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open project", QDir::homePath(), "CANdevStudio (*.cds)");

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
    if (!ProjectConfigValidator::validateConfiguration(wholeFile)) {
        QMessageBox::warning(nullptr, "File corrupted", "Invalid file. Project loading aborted");
        return;
    }

    QFileInfo fInfo(fileName);

    if (createProjectConfig(fInfo.completeBaseName())) {
        if (_projectConfig) {
            _projectConfig->load(wholeFile); // FIXME
            _projectFile = fileName;

            addRecentProject(_projectName, _projectFile);
        } else {
            cds_error("Project config does not exist");
        }
    }

    refreshRecentProjects();
}

void MainWindow::handleAboutAction()
{
    Ui::aboutDialog dialog;
    QDialog *d = new QDialog();
    dialog.setupUi(d);
    QString version = CDS_VERSION;

    if(QString(GIT_COMMIT_HASH).length() > 0) {
        version += QString(" (") + GIT_BRANCH + ", " + GIT_COMMIT_HASH + ")";
    }

    dialog.version->setText(QString("v") + CDS_VERSION);
    dialog.info->setText(dialog.info->text() + " " + version);
    dialog.repoLink->setTextFormat(Qt::RichText);
    dialog.repoLink->setOpenExternalLinks(true);

    dialog.info->setTextInteractionFlags(Qt::TextSelectableByMouse);
    dialog.legal->setTextInteractionFlags(Qt::TextSelectableByMouse);
    dialog.copyright->setTextInteractionFlags(Qt::TextSelectableByMouse);
    dialog.repoLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    dialog.repoLink->setTextInteractionFlags(Qt::TextSelectableByMouse);
    dialog.close->setCursor(Qt::PointingHandCursor);
    dialog.mobica->setCursor(Qt::PointingHandCursor);
    dialog.genivi->setCursor(Qt::PointingHandCursor);

    connect(dialog.close, &QPushButton::clicked, d, &QDialog::close);
    connect(dialog.mobica, &QPushButton::clicked, []{
        QDesktopServices::openUrl({"http://www.mobica.com"});
    });
    connect(dialog.genivi, &QPushButton::clicked, []{
        QDesktopServices::openUrl({"http://www.genivi.org"});
    });

    d->setWindowFlags(d->windowFlags() | Qt::FramelessWindowHint);
    d->exec();
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

        _ui->actionStop->trigger();

        _projectConfig->clearGraphView();
        if (_projectConfig.get()->isVisible()) {
            _projectConfig.get()->parentWidget()->close();
        }
        _projectConfig.reset();
        _projectFile.clear();
        _projectName.clear();

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

        _ui->mdiArea->hide();
        refreshRecentProjects();
        _ui->startScreenWidget->show();
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
        _ui->startScreenWidget->hide();
        _ui->mdiArea->show();

        setStyle(_currentStyle);

        _projectConfig->setWindowTitle(_projectName);
        addToMdi(_projectConfig.get());

        connect(_ui->actionStop, &QAction::triggered, _projectConfig.get(), &ProjectConfig::simulationStopped);
        connect(_ui->actionStart, &QAction::triggered, _projectConfig.get(), &ProjectConfig::simulationStarted);
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
    connect(_ui->actionAbout, &QAction::triggered, this, &MainWindow::handleAboutAction);
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
    connect(_ui->actionSwitchStyle, &QAction::triggered, this, &MainWindow::switchStyle);
    connect(_ui->actionClear, &QAction::triggered, [this] {
        _recentProjects.clear();
        refreshRecentProjects();
    });
}

void MainWindow::addToMdi(QWidget* component)
{
    auto wnd = new SubWindow(nullptr);
    wnd->setWidget(component);
    _ui->mdiArea->addSubWindow(wnd);
    component->show();
    wnd->show();
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
    if (_currentStyle == Styles::darkStyle)
        setStyle(Styles::lightStyle);
    else
        setStyle(Styles::darkStyle);
}

void MainWindow::setStyle(Styles style)
{
    QString stylefile;
    QString flowStyle;
    QString styletoolbar;
    QString neStyleFile;
    QColor bgMdiColor(0x1d, 0x1d, 0x1d);
    bool darkMode = false;

    switch (style) {
    case Styles::darkStyle: {
        darkMode = true;
        stylefile = ":/files/css/darkStyle.css";
        styletoolbar = ":/files/css/toolBar_dark.css";
        neStyleFile = ":/files/json/NodeEditorStyleDark.json"; 

        // Setting icons for QAction in CSS does not work
        _ui->actionNew->setIcon(QIcon(":/images/files/images/light/CANbus_icon_NewProject.svg"));
        _ui->actionLoad->setIcon(QIcon(":/images/files/images/light/CANbus_icon_OpenProject.svg"));
        _ui->actionSave->setIcon(QIcon(":/images/files/images/light/CANbus_icon_SaveProject.svg"));
        _ui->actionSaveAs->setIcon(QIcon(":/images/files/images/light/CANbus_icon_SaveAsProject.svg"));
        _ui->actionSimulation->setIcon(QIcon(":/images/files/images/light/CANbus_icon_OpenSim.svg"));
        _ui->actionClose->setIcon(QIcon(":/images/files/images/light/CANbus_icon_CloseSim.svg"));
        _ui->actionExit->setIcon(QIcon(":/images/files/images/light/CANbus_icon_Close.svg"));
    } break;
    case Styles::lightStyle: {
        stylefile = ":/files/css/lightStyle.css";
        styletoolbar = ":/files/css/toolBar_light.css";
        neStyleFile = ":/files/json/NodeEditorStyleLight.json"; 
        bgMdiColor = QColor(0xd3, 0xd3, 0xd3);

        // Setting icons for QAction in CSS does not work
        _ui->actionNew->setIcon(QIcon(":/images/files/images/dark/CANbus_icon_NewProject.svg"));
        _ui->actionLoad->setIcon(QIcon(":/images/files/images/dark/CANbus_icon_OpenProject.svg"));
        _ui->actionSave->setIcon(QIcon(":/images/files/images/dark/CANbus_icon_SaveProject.svg"));
        _ui->actionSaveAs->setIcon(QIcon(":/images/files/images/dark/CANbus_icon_SaveAsProject.svg"));
        _ui->actionSimulation->setIcon(QIcon(":/images/files/images/dark/CANbus_icon_OpenSim.svg"));
        _ui->actionClose->setIcon(QIcon(":/images/files/images/dark/CANbus_icon_CloseSim.svg"));
        _ui->actionExit->setIcon(QIcon(":/images/files/images/dark/CANbus_icon_Close.svg"));
    } break;
    }

    // Application style
    QFile f(stylefile);
    f.open(QFile::ReadOnly);
    QString css = QLatin1String(f.readAll());
    f.close();

    // Toolbar style
    QFile f2(styletoolbar);
    f2.open(QFile::ReadOnly);
    css.append(QLatin1String(f2.readAll()));
    f2.close();

    qApp->setStyleSheet(css);

    QFile neFile(neStyleFile);
    QString neStyle;
    if (neFile.open(QIODevice::ReadOnly)) {
        neStyle = QString::fromUtf8(neFile.readAll());
    } else {
        cds_error("Failed to open style file '{}'", neStyleFile.toStdString());
    }

    QtNodes::FlowViewStyle::setStyle(neStyle);
    QtNodes::ConnectionStyle::setConnectionStyle(neStyle);

    // Workaround. Background is not updated via style sheet.
    if (_projectConfig) {
        _projectConfig->setColorMode(darkMode);
    }
    _ui->mdiArea->setBackground(QBrush(bgMdiColor, Qt::SolidPattern));

    _currentStyle = style;
}
