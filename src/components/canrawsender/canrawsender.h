#ifndef CANRAWSENDER_H
#define CANRAWSENDER_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>

class QCanBusFrame;
class CanRawSenderPrivate;
class QWidget;

class CanRawSender : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawSender)

public:
    CanRawSender();
    explicit CanRawSender(CanRawSenderCtx&& ctx);
    ~CanRawSender();

    // TODO this should go as config parameter!
    int getLineCount() const;

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
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

signals:
    void sendFrame(const QCanBusFrame& frame);

    /**
    *   @see ComponentInterface
    */
    void mainWidgetDockToggled(QWidget* widget) override;

    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    /**
    *   @see ComponentInterface
    */
    void stopSimulation() override;
    /**
    *   @see ComponentInterface
    */
    void startSimulation() override;

    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    QScopedPointer<CanRawSenderPrivate> d_ptr;
};

#endif // CANRAWSENDER_H
