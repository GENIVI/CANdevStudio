#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QtCore/QObject>
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
    void setConfig(QJsonObject& json) override;

    /**
    *   @see ComponentInterface
    */
    QJsonObject getConfig() const override;

    /**
    *   @see ComponentInterface
    */
    bool mainWidgetDocked() const override;

signals:
    /**
    *   @see ComponentInterface
    */
    void mainWidgetDockToggled(QWidget* widget) override;

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

private:
    QScopedPointer<CanRawViewPrivate> d_ptr;
};

#endif // CANRAWVIEW_H
