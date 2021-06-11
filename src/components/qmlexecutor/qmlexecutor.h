#ifndef QMLEXECUTOR_H
#define QMLEXECUTOR_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class QMLExecutorPrivate;
class QWidget;
class QMLExecutorGuiInt;
class CANBusModel;

/**
 * @brief single instance ioc/iod container
 */
typedef Context<QMLExecutorGuiInt> QMLExecutorCtx;

/**
 * @see ComponentInterface
 */
class QMLExecutor : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMLExecutor)

public:
    /**
     * @brief constructor
     */
    QMLExecutor();

    /**
     * @brief explicit constructor with context
     * @param ctx a context with something inside, most likely an ui
     */
    explicit QMLExecutor(QMLExecutorCtx&& ctx);

    /**
     * @brief virtual dtor
     */
    virtual ~QMLExecutor();

    /**
     * @see ComponentInterface::mainWidget
     */
    QWidget* mainWidget() override;


    /**
     * @see ComponentInterface::setConfig
     */
    void setConfig(const QJsonObject& json) override;

    /**
     * @see ComponentInterface::
     */
    void setConfig(const QWidget& qobject) override;

    /**
     * @see ComponentInterface::
     */
    QJsonObject getConfig() const override;

    /**
     * @see ComponentInterface::getQConfig
     */
    std::shared_ptr<QWidget> getQConfig() const override;

    /**
     * @see ComponentInterface::configChanged
     */
    void configChanged() override;

    /**
     * @see ComponentInterface::mainWidgetDocked
     */
    bool mainWidgetDocked() const override;

    /**
     * @see ComponentInterface::getSupportedProperties
     */
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

public:

    /**
     * @brief sets can bus model used from QML
     * @param model can bus model used from qml
     */
    void setCANBusModel(CANBusModel* model);

signals:
    /**
     * @see ComponentInterface::mainWidgetDockToggled
     */
    void mainWidgetDockToggled(QWidget* widget) override;

    /**
     * @see ComponentInterface::simBcastSnd
     */
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    /**
     * @see ComponentInterface::stopSimulation
     */
    void stopSimulation() override;

    /**
     * @see ComponentInterface::startSimulation
     */
    void startSimulation() override;

    /**
     * @see ComponentInterface::simBcastRcv
     */
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    /**
     * @brief qt weird stuff
     */
    QScopedPointer<QMLExecutorPrivate> d_ptr;
};

#endif //QMLEXECUTOR_H
