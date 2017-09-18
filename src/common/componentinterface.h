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
    *   @brief  Action to be taken on simulation stop
    */
    virtual void stopSimulation() = 0;

    /**
    *   @brief  Action to be taken on simulation start
    */
    virtual void startSimulation() = 0;

    /**
    *   @brief  Sets configuration for component
    *   @param  json configuratio to be aplied
    */
    virtual void setConfig(QJsonObject& json) = 0;

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
    virtual QWidget* getMainWidget()
    {
        return nullptr;
    }

    /**
    *   @brief  Callback, called when component requests dock/undock action
    */
    virtual void setDockUndockClbk(const std::function<void()>&)
    {
    }

    /**
    *   @brief  Main widget docking status
    *   @return returns true if widget is docked (part of MDI) or undocked (separate window)
    */
    virtual bool mainWidgetDocked() const
    {
        return true;
    }
};

#endif /* !__COMPONENTINTERFACE_H */
