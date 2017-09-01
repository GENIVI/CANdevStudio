#include "projectconfiguration.h"
#include "modelvisitor.h" // apply_model_visitor
#include "projectconfiguration_p.h"
#include <cassert> // assert

#include <QAction>

#include <candevice/candevicemodel.h>
#include <canrawsender/canrawsendermodel.h>
#include <canrawview/canrawviewmodel.h>

ProjectConfiguration::ProjectConfiguration(QAction* start, QAction* stop) //FIXME actions
    : _start(start)
    , _stop(stop)
    , d_ptr(new ProjectConfigurationPrivate(this))
{
    Q_D(ProjectConfiguration);

    setLayout(d->ui->layout);
}

ProjectConfiguration::~ProjectConfiguration()
{
}

void ProjectConfiguration::nodeCreatedCallback(QtNodes::Node& node)
{
    Q_D(ProjectConfiguration);
    d->nodeCreatedCallback(node);
}

void ProjectConfiguration::nodeDeletedCallback(QtNodes::Node& node)
{
    Q_D(ProjectConfiguration);
    d->nodeDeletedCallback(node);
}

void ProjectConfiguration::nodeDoubleClickedCallback(QtNodes::Node& node)
{
    Q_D(ProjectConfiguration);
    d->nodeDoubleClickedCallback(node);
}

void ProjectConfiguration::closeEvent(QCloseEvent* e)
{
    e->ignore();
}

QByteArray ProjectConfiguration::save()
{
    Q_D(ProjectConfiguration);
    return d->save();
}

void ProjectConfiguration::load(const QByteArray& data)
{
    Q_D(ProjectConfiguration);
    return d->load(data);
}

void ProjectConfiguration::clearGraphView()
{
    Q_D(ProjectConfiguration);
    return d->clearGraphView();
}
