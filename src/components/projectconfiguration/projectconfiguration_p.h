#ifndef PROJECTCONFIGURATION_P_H
#define PROJECTCONFIGURATION_P_H
#include "modelvisitor.h" // apply_model_visitor
#include "ui_projectconfiguration.h"
#include <cassert> // assert

#include <QAction>

#include <QLabel>

#include <candevice/candevicemodel.h>
#include <canrawsender/canrawsendermodel.h>
#include <canrawview/canrawviewmodel.h>

namespace Ui {
class ProjectConfigurationPrivate;
}

class ProjectConfigurationPrivate : public QWidget {
    Q_OBJECT
    Q_DECLARE_PUBLIC(ProjectConfiguration)

public:
    ProjectConfigurationPrivate(ProjectConfiguration* q)
        : ui(std::make_unique<Ui::ProjectConfigurationPrivate>())
        , q_ptr(q)
    {
        auto modelRegistry = std::make_shared<QtNodes::DataModelRegistry>();
        modelRegistry->registerModel<CanDeviceModel>();
        modelRegistry->registerModel<CanRawSenderModel>();
        modelRegistry->registerModel<CanRawViewModel>();

        graphScene = std::make_shared<QtNodes::FlowScene>(modelRegistry);
        graphView = new QtNodes::FlowView(graphScene.get());

        connect(graphScene.get(), &QtNodes::FlowScene::nodeCreated, this,
            &ProjectConfigurationPrivate::nodeCreatedCallback);
        connect(graphScene.get(), &QtNodes::FlowScene::nodeDeleted, this,
            &ProjectConfigurationPrivate::nodeDeletedCallback);
        connect(graphScene.get(), &QtNodes::FlowScene::nodeDoubleClicked, this,
            &ProjectConfigurationPrivate::nodeDoubleClickedCallback);

        ui->setupUi(this);
        ui->layout->addWidget(graphView);
    }
    ~ProjectConfigurationPrivate() {}
    std::unique_ptr<Ui::ProjectConfigurationPrivate> ui;

private:
    std::shared_ptr<QtNodes::FlowScene> graphScene;
    QtNodes::FlowView* graphView; // FIXME

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

public:
    QByteArray save() const { return graphScene->saveToMemory(); }

    void load(const QByteArray& data) { return graphScene->loadFromMemory(data); }

    void clearGraphView() { return graphScene->clearScene(); };

    void nodeCreatedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        Q_Q(ProjectConfiguration);

        apply_model_visitor(*dataModel,
            [this, dataModel, q](CanRawViewModel& m) {
                auto rawView = &m.canRawView;
                emit q->componentWidgetCreated(rawView);
                connect(q->_start, &QAction::triggered, rawView, &CanRawView::startSimulation);
                connect(q->_stop, &QAction::triggered, rawView, &CanRawView::stopSimulation);
                connect(rawView, &CanRawView::dockUndock, this, [this, rawView, q] { emit q->handleDock(rawView); });
            },
            [this, dataModel, q](CanRawSenderModel& m) {
                QWidget* crsWidget = m.canRawSender.getMainWidget();
                auto& rawSender = m.canRawSender;
                emit q->componentWidgetCreated(crsWidget);
                connect(q->_start, &QAction::triggered, &rawSender, &CanRawSender::startSimulation);
                connect(q->_stop, &QAction::triggered, &rawSender, &CanRawSender::stopSimulation);
                connect(&rawSender, &CanRawSender::dockUndock, this,
                    [this, crsWidget, q] { emit q->handleDock(crsWidget); });
            },
            [this](CanDeviceModel&) {});
    }

    void nodeDeletedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        apply_model_visitor(*dataModel, [this, dataModel](CanRawViewModel& m) { handleWidgetDeletion(&m.canRawView); },
            [this, dataModel](CanRawSenderModel& m) { handleWidgetDeletion(m.canRawSender.getMainWidget()); },
            [this](CanDeviceModel&) {});
    }

    void nodeDoubleClickedCallback(QtNodes::Node& node)
    {
        auto dataModel = node.nodeDataModel();

        assert(nullptr != dataModel);

        apply_model_visitor(*dataModel, [this, dataModel](CanRawViewModel& m) { handleWidgetShowing(&m.canRawView); },
            [this, dataModel](CanRawSenderModel& m) { handleWidgetShowing(m.canRawSender.getMainWidget()); },
            [this](CanDeviceModel&) {});
    }

private:
    ProjectConfiguration* q_ptr;
};
#endif // PROJECTCONFIGURATION_P_H
