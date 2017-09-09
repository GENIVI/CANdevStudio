#include "projectconfig.h"
#include "modelvisitor.h" // apply_model_visitor
#include "projectconfig_p.h"
#include <cassert> // assert
#include <QCloseEvent>
#include <QAction>

#include <candevice/candevicemodel.h>
#include <canrawsender/canrawsendermodel.h>
#include <canrawview/canrawviewmodel.h>

ProjectConfig::ProjectConfig()
    : d_ptr(new ProjectConfigPrivate(this))
{
    Q_D(ProjectConfig);

    setLayout(d->ui->layout);
}

ProjectConfig::~ProjectConfig()
{
}

void ProjectConfig::nodeCreatedCallback(QtNodes::Node& node)
{
    Q_D(ProjectConfig);
    d->nodeCreatedCallback(node);
}

void ProjectConfig::nodeDeletedCallback(QtNodes::Node& node)
{
    Q_D(ProjectConfig);
    d->nodeDeletedCallback(node);
}

void ProjectConfig::nodeDoubleClickedCallback(QtNodes::Node& node)
{
    Q_D(ProjectConfig);
    d->nodeDoubleClickedCallback(node);
}

void ProjectConfig::closeEvent(QCloseEvent* e)
{
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
