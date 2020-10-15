#ifndef QMLEXECUTOR_P_H
#define QMLEXECUTOR_P_H

#include <memory>

#include <QObject>

#include "gui/qmlexecutorguiimpl.h"
#include "qmlexecutor.h"
#include "propertyfields.h"

class QMLExecutor;
class CANBusModel;

/**
 * @brief QMLExecutor plugin private implementation
 */
class QMLExecutorPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(QMLExecutor)

public:
    /**
     * @brief constructor carring interface class and ui
     * @param q qt stuff, interface class
     * @param ioc/iod container with gui, created when not supplied
     */
    QMLExecutorPrivate(QMLExecutor* q, QMLExecutorCtx&& ctx = QMLExecutorCtx(new QMLExecutorGuiImpl(nullptr)));
    ~QMLExecutorPrivate();

public: // ComponentInterface inheritance
    /**
     * @see ComponentInterface::getSupportedProperties
     */
    ComponentInterface::ComponentProperties getSupportedProperties() const;

    /**
     * @see ComponentInterface::getSettings
     */
    QJsonObject getSettings();

    /**
     * @see ComponentInterface::setSettings
     */
    void setSettings(const QJsonObject& json);

    /**
     * @see ComponentInterface::configChanged
     */
    void configChanged();

    /**
     * @brief start simulation handler
     */
    void startSimulation();

    /**
     * @brief stop simulation handler
     */
    void stopSimulation();

    /**
     * @brief used to align empty QQuickWidget color with current color scheme
     */
    void updateUIColor();

public slots:
    /**
     * @brief Sets a can bus model used in QML
     * @param model can bus interface model
     */
    void setCANBusModel(CANBusModel* model);

    /**
     * @brief QMLLoaded slot used for notification of QML load
     * @param url url of a qml file
     */
    void QMLLoaded(const QUrl& url);

private:
    /**
     * @brief populate properties of this plugin
     * @see ComponentInterface
     */
    void initProps();

public:
    //! Simulation started flag
    bool _simStarted{ false };
    //! ioc/iod container for a gui
    QMLExecutorCtx _ctx;
    //! gui
    QMLExecutorGuiInt& _ui;

    //! not used
    bool _docked{ true };

    //! plugin properties @see ComponentInterface
    std::map<QString, QVariant> _props;

private:
    QMLExecutor* q_ptr;
    const QString _nameProperty = "name";
    const QString _fileProperty = "QML file";
    CANBusModel* _model;


    // workaround for clang 3.5
    using cf = ComponentInterface::CustomEditFieldCbk;

    // clang-format off
    ComponentInterface::ComponentProperties _supportedProps = {
            std::make_tuple(_nameProperty, QVariant::String, true, cf(nullptr)),
            std::make_tuple(_fileProperty, QVariant::String, true, cf([] { return new PropertyFieldPath; } ))
    };
    // clang-format on
};

#endif // QMLEXECUTOR_P_H
