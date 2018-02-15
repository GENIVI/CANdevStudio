#ifndef COMPONENTMODEL_H
#define COMPONENTMODEL_H

#include "projectconfig.h"
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <functional>
#include <log.h>
#include <nodes/NodeDataModel>
#include <QThread>

struct ComponentInterface;

struct ComponentModelInterface {
    virtual ~ComponentModelInterface() = default;
    virtual ComponentInterface& getComponent() = 0;
    virtual void handleModelCreation(ProjectConfig* config, QThread* th = nullptr) = 0;
    virtual void setCaption(const QString& caption) = 0;
    virtual bool restored() = 0;
    virtual void setColorMode(bool darkMode) = 0;
    virtual bool hasSeparateThread() const = 0;
};

template <typename C, typename Derived>
class ComponentModel : public QtNodes::NodeDataModel, public ComponentModelInterface {

public:
    ComponentModel() = default;

    ComponentModel(const QString& name)
        : _caption(name)
        , _name(name)
    {
    }

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
     * @brief Sets model caption and updates widget title.
     * @param caption new caption
     */
    virtual void setCaption(const QString& caption) override
    {
        _caption = caption;

        auto w = _component.mainWidget();
        if (w) {
            w->setWindowTitle(_caption);
        }
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
        json["caption"] = caption();
        return json;
    }

    /**
     * @brief Used to restore node configurations
     * @param json Constant reference to json object
     */
    virtual void restore(QJsonObject const& json) override
    {
        if (json.find("name") == json.end()) {
            cds_error("Problem with validation of restore configuration: component model name tag not exist.");
            return;
        }

        if (json.find("caption") == json.end()) {
            cds_error("Problem with validation of restore configuration: component model caption tag not exist.");
            return;
        }

        _restored = true;
        _name = json.find("name").value().toString();
        _caption = json.find("caption").value().toString();
        _component.setConfig(json);
        _component.configChanged();
        cds_info("Correct validation of restore configuration for {} modul", _name.toStdString());
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

    virtual void handleModelCreation(ProjectConfig* config, QThread *th = nullptr) override
    {
        connect(config, &ProjectConfig::startSimulation, &_component, &C::startSimulation);
        connect(config, &ProjectConfig::stopSimulation, &_component, &C::stopSimulation);
        connect(&_component, &C::mainWidgetDockToggled, config, &ProjectConfig::handleDock);

        if(th) {
            cds_info("Setting separate event loop for component {}", _caption.toStdString());

            this->moveToThread(th);
            _component.moveToThread(th);

            th->start();
        }
    }

    virtual bool restored() override
    {
        return _restored;
    }

    virtual void setColorMode(bool darkMode)
    {
        _darkMode = darkMode;

        if(darkMode) {
            QColor bgColor = QColor(94, 94, 94);
            _nodeStyle.GradientColor0 = bgColor;
            _nodeStyle.GradientColor1 = bgColor;
            _nodeStyle.GradientColor2 = bgColor;
            _nodeStyle.GradientColor3 = bgColor;
            _nodeStyle.NormalBoundaryColor = bgColor;
            _nodeStyle.FontColor = QColor(240, 240, 240);
            _nodeStyle.FontColorFaded = QColor(240, 240, 240);
            _nodeStyle.ShadowColor = QColor(20, 20, 20);
        } else {
            QColor bgColor = QColor(255, 255, 255);
            _nodeStyle.GradientColor0 = bgColor;
            _nodeStyle.GradientColor1 = bgColor;
            _nodeStyle.GradientColor2 = bgColor;
            _nodeStyle.GradientColor3 = bgColor;
            _nodeStyle.NormalBoundaryColor = bgColor;
            _nodeStyle.FontColor = QColor(20, 20, 20);
            _nodeStyle.FontColorFaded = QColor(20, 20, 20);
            _nodeStyle.ShadowColor = QColor(170, 170, 170);
        }

        _nodeStyle.Opacity = 1.0;

        setNodeStyle(_nodeStyle);
    }

    virtual bool hasSeparateThread() const override
    {
        // Override if you want model and underlying component to be run in separate thread
        return false;
    }

protected:
    C _component;
    QLabel* _label{ new QLabel };
    QString _caption;
    QString _name;
    bool _resizable{ false };
    bool _restored{ false };
    bool _darkMode{ true };
    QtNodes::NodeStyle _nodeStyle;
};

#endif // COMPONENTMODEL_H
