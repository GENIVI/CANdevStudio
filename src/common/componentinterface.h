#ifndef __COMPONENTINTERFACE_H
#define __COMPONENTINTERFACE_H

#include <QtCore/QJsonObject>
#include <functional>
#include <memory>

class QWidget;

/**
*   @brief  Interface to be implemented by every component
*/
struct ComponentInterface {
    virtual ~ComponentInterface()
    {
    }

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
    *   @param  json configuratio to be aplied
    */
    virtual void setConfig(const QJsonObject& json) = 0;

    /**
    *   @brief  Sets configuration for component
    *   @param  json configuratio to be aplied
    */
    virtual void setConfig(const QObject& json) = 0;

    /**
    *   @brief  Gets current component configuation
    *   @return current config
    */
    virtual QJsonObject getConfig() const = 0;

    /**
    *   @brief  Gets current component configuation
    *   @return current config
    */
    virtual std::shared_ptr<QObject> getQConfig() const = 0;

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
};

#endif /* !__COMPONENTINTERFACE_H */
