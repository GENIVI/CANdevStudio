#include "projectconfig.h"
#include "projectconfig_p.h"
#include <QCloseEvent>

ProjectConfig::ProjectConfig(QWidget* parent_)
    : QWidget(parent_), d_ptr(new ProjectConfigPrivate(this, this))
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

void ProjectConfig::setGraphViewBackground(const QColor &color)
{
    Q_D(ProjectConfig);
    d->_graphView->setBackgroundBrush(QBrush(color, Qt::SolidPattern));
}
