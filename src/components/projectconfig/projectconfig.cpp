#include "projectconfig.h"
#include "projectconfig_p.h"
#include <QCloseEvent>

ProjectConfig::ProjectConfig(QWidget* parent_)
    : QWidget(parent_), d_ptr(new ProjectConfigPrivate(this, this))
{
    Q_D(ProjectConfig);

    setLayout(d->_ui->layout);
}

ProjectConfig::ProjectConfig(QWidget* parent_, ProjectConfigCtx&& ctx)
    : QWidget(parent_), d_ptr(new ProjectConfigPrivate(this, this, std::move(ctx)))
{
    Q_D(ProjectConfig);

    setLayout(d->_ui->layout);
}

ProjectConfig::~ProjectConfig()
{
}

void ProjectConfig::closeEvent(QCloseEvent* e)
{
    emit closeProject();
    e->ignore();
}

QByteArray ProjectConfig::save()
{
    Q_D(ProjectConfig);
    return d->save();
}

void ProjectConfig::load(const QByteArray& data)
{
    Q_D(ProjectConfig);
    return d->load(data);
}

void ProjectConfig::clearGraphView()
{
    Q_D(ProjectConfig);
    return d->clearGraphView();
}

void ProjectConfig::setColorMode(bool darkMode)
{
    Q_D(ProjectConfig);
    QColor bgViewColor;

    if(darkMode) {
        bgViewColor = QColor(38, 38, 38);
    } else {
        bgViewColor = QColor(205, 205, 205);
    }

    d->_graphView->setBackgroundBrush(QBrush(bgViewColor, Qt::SolidPattern));
    d->updateNodeStyle(darkMode);
    d->addModelIcons();
}

void ProjectConfig::simulationStarted()
{
    emit startSimulation();

    d_ptr->_simStarted = true;
}

void ProjectConfig::simulationStopped()
{
    emit stopSimulation();

    d_ptr->_simStarted = false;
}

