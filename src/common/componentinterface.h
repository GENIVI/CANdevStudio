#ifndef __COMPONENTINTERFACE_H
#define __COMPONENTINTERFACE_H

#include <QVariant>
#include <QtCore/QJsonObject>
#include <functional>
#include <map>
#include <memory>

class QWidget;

/**
 *   @brief  Interface to be implemented by every component
 */
struct ComponentInterface {
    virtual ~ComponentInterface() {}

    /**
     *   @brief  Signal to be implemented by Component. Indicates when dock/undock action was invoked.
     *   @param  widget Widget subjected to dock/undock action
     */
    virtual void mainWidgetDockToggled(QWidget* widget) = 0;

    /**
     *   @brief  Slot to be implemented by Component to execute simulation stop action
     */
    virtual void stopSimulation() = 0;

    /**
     *   @brief  Slot to be implemented by Component to execute simulation start action
     */
    virtual void startSimulation() = 0;

    /**
     *   @brief  Sets configuration for component
     *   @param  json configuration to be aplied
     */
    virtual void setConfig(const QJsonObject& json) = 0;

    /**
     *   @brief  Sets configuration for component
     *   @param  QWidget configuration to be aplied
     */
    virtual void setConfig(const QWidget& qobject) = 0;

    /**
     *  @brief  Reconfigure component if necessary
     */
    virtual void configChanged() = 0;

    /**
     *   @brief  Gets current component configuation
     *   @return current config
     */
    virtual QJsonObject getConfig() const = 0;

    /**
     *   @brief  Gets current component configuation
     *   @return current config
     */
    virtual std::shared_ptr<QWidget> getQConfig() const = 0;

    using PropertyEditable = bool;
    using CustomEditFieldCbk = std::function<QWidget*(void)>;
    using ComponentProperty = std::tuple<QString, QVariant::Type, PropertyEditable, CustomEditFieldCbk>;
    using ComponentProperties = std::vector<ComponentProperty>;
    using PropertyContainer = std::map<QString, QVariant>;

    static constexpr const QString& propertyName(const ComponentProperty& p)
    {
        return std::get<0>(p);
    }

    static constexpr const QVariant::Type& propertyType(const ComponentProperty& p)
    {
        return std::get<1>(p);
    }

    static constexpr const PropertyEditable& propertyEditability(const ComponentProperty& p)
    {
        return std::get<2>(p);
    }

    static constexpr const CustomEditFieldCbk& propertyField(const ComponentProperty& p)
    {
        return std::get<3>(p);
    }

    /**
     * Gets list of properties supported by this component
     * @return list of supported properties
     */
    virtual ComponentProperties getSupportedProperties() const = 0;

    /**
     *   @brief  Gets components's main widget
     *   @return Main widget or nullptr if component doesn't have it
     */
    virtual QWidget* mainWidget() = 0;

    /**
     *   @brief  Main widget docking status
     *   @return returns true if widget is docked (part of MDI) or undocked (separate window)
     */
    virtual bool mainWidgetDocked() const = 0;

    virtual void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) = 0;
    virtual void simBcastRcv(const QJsonObject &msg, const QVariant &param) = 0;
};

#endif /* !__COMPONENTINTERFACE_H */
