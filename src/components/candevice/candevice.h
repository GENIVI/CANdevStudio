#ifndef __CANDEVICE_H
#define __CANDEVICE_H

#include <QScopedPointer>
#include <QWidget>
#include <componentinterface.h>
#include <context.h>

class CanDevicePrivate;
class QCanBusFrame;

/**
*   @brief The class provides abstraction layer for CAN BUS hardware
*/
class CanDevice : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanDevice)

public:
    explicit CanDevice();
    explicit CanDevice(CanDeviceCtx&& ctx);
    ~CanDevice();

    /**
     * @brief  Configures CAN BUS backend and interface
     *
     * This function configures QtCanBus class with backend and interface
     * values stored in properties.
     * @return true on success, false on failure
     */
    bool init();

    /**
    *   @see ComponentInterface
    */
    QWidget* mainWidget() override;

    /**
    *   @see ComponentInterface
    */
    void setConfig(const QJsonObject& json) override;

    /**
     *  @see ComponentInterface
     */
    void setConfig(const QWidget& qobject) override;

    /**
    *   @see ComponentInterface
    */
    QJsonObject getConfig() const override;

    /**
    *   @see ComponentInterface
    */
    std::shared_ptr<QWidget> getQConfig() const override;

    /**
    *   @see ComponentInterface
    */
    virtual void configChanged() override;

    /**
     *   @see ComponentInterface
     */
    bool mainWidgetDocked() const override;

    /**
     *   @see ComponentInterface
     */
    ComponentProperties getSupportedProperties() const override;


signals:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);

    /**
    *   @see ComponentInterface
    */
    void mainWidgetDockToggled(QWidget* widget) override;
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    void sendFrame(const QCanBusFrame& frame);

    /**
    *   @see ComponentInterface
    */
    void startSimulation() override;

    /**
    *   @see ComponentInterface
    */
    void stopSimulation() override;

    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private slots:
    void errorOccurred(int error);
    void framesWritten(qint64 cnt);
    void framesReceived();

private:
    QScopedPointer<CanDevicePrivate> d_ptr;
};

#endif /* !__CANDEVICE_H */
