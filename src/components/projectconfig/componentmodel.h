#ifndef COMPONENTMODEL_H
#define COMPONENTMODEL_H

#include "projectconfig.h"
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <functional>
#include <nodes/NodeDataModel>

struct ComponentInterface;

struct ComponentModelInterface {
    virtual ~ComponentModelInterface() = default;
    virtual ComponentInterface& getComponent() = 0;
    virtual void handleModelCreation(ProjectConfig* config) = 0;
};

template <typename C, typename Derived>
class ComponentModel : public QtNodes::NodeDataModel, public ComponentModelInterface {

public:
    ComponentModel() = default;
    virtual ~ComponentModel() = default;

    /**
    *   @brief  Used to get node caption
    *   @return Node caption
    */
    virtual QString caption() const override
    {
        return _caption;
    }

    /**
    *   @brief  Used to identify model by data model name
    *   @return Node model name
    */
    virtual QString name() const override
    {
        return _name;
    }

    /**
    *   @brief Creates new node of the same type
    *   @return cloned node
    */
    virtual std::unique_ptr<QtNodes::NodeDataModel> clone() const override
    {
        return std::make_unique<Derived>();
    }

    /**
     * @brief Possibility to save node properties
     * @return json object
     */
    virtual QJsonObject save() const override
    {
        QJsonObject json = _component.getConfig();
        json["name"] = name();
        return json;
    }

    /**
    *   @brief  Used to get model name
    *   @return Model name
    */
    virtual QString modelName() const
    {
        return _modelName;
    }

    /**
    *   @brief  Used to get widget embedded in Node
    *   @return QLabel
    */
    virtual QWidget* embeddedWidget() override
    {
        return _label;
    }

    /**
    *   @brief  Used to get information if node is resizable
    *   @return false
    */
    virtual bool resizable() const override
    {
        return _resizable;
    }

    /**
    *   @brief Component getter
    *   @return Component managed by model
    */
    virtual ComponentInterface& getComponent() override
    {
        return _component;
    }

    virtual void handleModelCreation(ProjectConfig* config) override
    {
        connect(config, &ProjectConfig::startSimulation, &_component, &C::startSimulation);
        connect(config, &ProjectConfig::stopSimulation, &_component, &C::stopSimulation);
        connect(&_component, &C::mainWidgetDockToggled, config, &ProjectConfig::handleDock);
    }

protected:
    C _component;
    QLabel* _label{ new QLabel };
    QString _caption;
    QString _name;
    QString _modelName;
    bool _resizable{ false };
};

#endif // COMPONENTMODEL_H
