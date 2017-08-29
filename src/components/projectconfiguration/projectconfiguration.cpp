#include "projectconfiguration.h"
#include "modelvisitor.h" // apply_model_visitor
#include <cassert> // assert

#include <QAction>

#include <candevice/candevicemodel.h>
#include <canrawsender/canrawsendermodel.h>
#include <canrawview/canrawviewmodel.h>

ProjectConfiguration::ProjectConfiguration(QAction* start, QAction* stop)
    : _start(start)
    , _stop(stop)
{
    auto modelRegistry = std::make_shared<QtNodes::DataModelRegistry>();
    modelRegistry->registerModel<CanDeviceModel>();
    modelRegistry->registerModel<CanRawSenderModel>();
    modelRegistry->registerModel<CanRawViewModel>();

    graphScene = std::make_shared<QtNodes::FlowScene>(modelRegistry);
    graphView = new QtNodes::FlowView(graphScene.get());
    graphView->setWindowTitle("Project Configuration");

    connect(graphScene.get(), &QtNodes::FlowScene::nodeCreated, this, &ProjectConfiguration::nodeCreatedCallback);
    connect(graphScene.get(), &QtNodes::FlowScene::nodeDeleted, this, &ProjectConfiguration::nodeDeletedCallback);
    connect(graphScene.get(), &QtNodes::FlowScene::nodeDoubleClicked, this,
        &ProjectConfiguration::nodeDoubleClickedCallback);
}

void handleWidgetDeletion(QWidget* widget)
{
    assert(nullptr != widget);
    if (widget->parentWidget()) {

        widget->parentWidget()->close();
    } // else path not needed
}

void handleWidgetShowing(QWidget* widget)
{
    assert(nullptr != widget);
    if (widget->parentWidget()) {

        widget->parentWidget()->show();
    } else {
        widget->show();
    }
}

QWidget* ProjectConfiguration::getGraphView() const { return graphView; };

void ProjectConfiguration::nodeCreatedCallback(QtNodes::Node& node)
{
    auto dataModel = node.nodeDataModel();

    assert(nullptr != dataModel);

    apply_model_visitor(*dataModel,
        [this, dataModel](CanRawViewModel& m) {
            auto rawView = &m.canRawView;
            emit componentWidgetCreated(rawView);
            connect(_start, &QAction::triggered, rawView, &CanRawView::startSimulation);
            connect(_stop, &QAction::triggered, rawView, &CanRawView::stopSimulation);
            connect(rawView, &CanRawView::dockUndock, this, [this, rawView] { emit handleDock(rawView); });
        },
        [this, dataModel](CanRawSenderModel& m) {
            QWidget* crsWidget = m.canRawSender.getMainWidget();
            auto& rawSender = m.canRawSender;
            emit componentWidgetCreated(crsWidget);
            connect(&rawSender, &CanRawSender::dockUndock, this, [this, crsWidget] { emit handleDock(crsWidget); });
            connect(_start, &QAction::triggered, &rawSender, &CanRawSender::startSimulation);
            connect(_stop, &QAction::triggered, &rawSender, &CanRawSender::stopSimulation);
        },
        [this](CanDeviceModel&) {});
}

void ProjectConfiguration::nodeDeletedCallback(QtNodes::Node& node)
{
    auto dataModel = node.nodeDataModel();

    assert(nullptr != dataModel);

    apply_model_visitor(*dataModel, [this, dataModel](CanRawViewModel& m) { handleWidgetDeletion(&m.canRawView); },
        [this, dataModel](CanRawSenderModel& m) { handleWidgetDeletion(m.canRawSender.getMainWidget()); },
        [this](CanDeviceModel&) {});
}

void ProjectConfiguration::nodeDoubleClickedCallback(QtNodes::Node& node)
{
    auto dataModel = node.nodeDataModel();

    assert(nullptr != dataModel);

    apply_model_visitor(*dataModel, [this, dataModel](CanRawViewModel& m) { handleWidgetShowing(&m.canRawView); },
        [this, dataModel](CanRawSenderModel& m) { handleWidgetShowing(m.canRawSender.getMainWidget()); },
        [this](CanDeviceModel&) {});
}
