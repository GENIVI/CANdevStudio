#ifndef COMPONENTMODEL_H
#define COMPONENTMODEL_H

#include <QThread>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <bcastmsgs.h>
#include <functional>
#include <log.h>
#include <nodes/Node>
#include <nodes/NodeDataModel>

struct ComponentInterface;

struct ComponentModelInterface : public QtNodes::NodeDataModel {

    Q_OBJECT

public:
    virtual ~ComponentModelInterface() = default;
    virtual ComponentInterface& getComponent() = 0;
    virtual void setCaption(const QString& caption) = 0;
    virtual bool restored() = 0;
    virtual void setColorMode(bool darkMode) = 0;
    virtual bool hasSeparateThread() const = 0;
    virtual void initModel(QtNodes::Node& node, int nodeCnt, bool darkMode) = 0;
    virtual void simBcastRcv(const QJsonObject& msg, const QVariant& param) = 0;
    virtual void simBcastSndSlot(const QJsonObject& msg, const QVariant& param) = 0;

signals:
    void startSimulation();
    void stopSimulation();
    void handleDock(QWidget* widget);
    void simBcastSnd(const QJsonObject& msg, const QVariant& param);
};

template <typename C, typename Derived> class ComponentModel : public ComponentModelInterface {

public:
    ComponentModel() = default;

    ComponentModel(const QString& name)
        : _caption(name)
        , _name(name)
    {
    }

    virtual ~ComponentModel()
    {
        if (_thread) {
            _thread->exit();
            _thread->wait();
        }
    }

    virtual void initModel(QtNodes::Node& node, int nodeCnt, bool darkMode) override
    {
        if (!restored()) {
            setCaption(node.nodeDataModel()->caption() + " #" + QString::number(nodeCnt));
        }

        // For some reason QWidget title is being set to name instead of caption.
        // TODO: investigate why
        setCaption(node.nodeDataModel()->caption());
        setColorMode(darkMode);

        connect((Derived*)this, &Derived::requestRedraw, [&node] { node.nodeGraphicsObject().update(); });
        connect(this, &ComponentModelInterface::startSimulation, &_component, &C::startSimulation);
        connect(this, &ComponentModelInterface::stopSimulation, &_component, &C::stopSimulation);
        connect(&_component, &C::mainWidgetDockToggled, this, &ComponentModelInterface::handleDock);
        connect(&_component, &C::simBcastSnd, this, &ComponentModelInterface::simBcastSndSlot);

        _id = node.id();

        if (hasSeparateThread()) {
            _thread = std::make_unique<QThread>();
            if (_thread) {
                cds_info("Setting separate event loop for component {}", _caption.toStdString());

                this->moveToThread(_thread.get());
                _component.moveToThread(_thread.get());

                _thread->start();
            }
        }
    }

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

    virtual bool restored() override
    {
        return _restored;
    }

    virtual void setColorMode(bool darkMode) override
    {
        _darkMode = darkMode;

        if (darkMode) {
            QColor bgColor = QColor(94, 94, 94);
            _nodeStyle.GradientColor0 = bgColor;
            _nodeStyle.GradientColor1 = bgColor;
            _nodeStyle.GradientColor2 = bgColor;
            _nodeStyle.GradientColor3 = bgColor;
            _nodeStyle.NormalBoundaryColor = bgColor;
            _nodeStyle.FontColor = QColor(206, 206, 206);
            _nodeStyle.FontColorFaded = QColor(125, 125, 125);
            _nodeStyle.ShadowColor = QColor(20, 20, 20);
            _nodeStyle.ConnectionPointColor = QColor(125, 125, 125);
            _nodeStyle.FilledConnectionPointColor = QColor(206, 206, 206);
        } else {
            QColor bgColor = QColor(255, 255, 255);
            _nodeStyle.GradientColor0 = bgColor;
            _nodeStyle.GradientColor1 = bgColor;
            _nodeStyle.GradientColor2 = bgColor;
            _nodeStyle.GradientColor3 = bgColor;
            _nodeStyle.NormalBoundaryColor = bgColor;
            _nodeStyle.FontColor = QColor(110, 110, 110);
            _nodeStyle.FontColorFaded = QColor(189, 189, 189);
            _nodeStyle.ShadowColor = QColor(170, 170, 170);
            _nodeStyle.ConnectionPointColor = QColor(170, 170, 170);
            _nodeStyle.FilledConnectionPointColor = QColor(110, 110, 110);
        }

        _nodeStyle.SelectedBoundaryColor = QColor(20, 146, 202);
        _nodeStyle.Opacity = 1.0;
        _nodeStyle.PenWidth = 1.5;
        _nodeStyle.HoveredPenWidth = 2.0;
        _nodeStyle.ConnectionPointDiameter = 3.5;

        setNodeStyle(_nodeStyle);
    }

    virtual bool hasSeparateThread() const override
    {
        // Override if you want model and underlying component to be run in separate thread
        return false;
    }

    void simBcastRcv(const QJsonObject& msg, const QVariant& param) override
    {
        if (msg["id"].toString() != _id.toString()) {
            _component.simBcastRcv(msg, param);
        } else if (msg["msg"].toString() == BcastMsg::NodeCreated) {
            QJsonObject msg2 = msg;
            msg2["msg"] = BcastMsg::InitDone;
            _component.simBcastRcv(msg2, param);
        }
    }

    void simBcastSndSlot(const QJsonObject& msg, const QVariant& param) override
    {
        emit simBcastSnd(fillBcastMsg(msg), param);
    }

private:
    QJsonObject fillBcastMsg(const QJsonObject& msg)
    {
        QJsonObject ret(msg);

        ret["id"] = _id.toString();
        ret["name"] = name();
        ret["caption"] = caption();

        return ret;
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
    std::unique_ptr<QThread> _thread;
    QUuid _id;
};

#endif // COMPONENTMODEL_H
