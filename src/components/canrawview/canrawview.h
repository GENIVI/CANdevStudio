#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class QCanBusFrame;
class CanRawViewPrivate;
class QWidget;

class CanRawView : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

public:
    CanRawView();
    explicit CanRawView(CanRawViewCtx&& ctx);
    ~CanRawView();

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
    /**
    *   @see ComponentInterface
    */
    void mainWidgetDockToggled(QWidget* widget) override;
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);

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
    QScopedPointer<CanRawViewPrivate> d_ptr;
};

#endif // CANRAWVIEW_H
