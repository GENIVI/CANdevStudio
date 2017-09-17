#ifndef CANRAWSENDER_H
#define CANRAWSENDER_H

#include <QtCore/QObject>
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
    void sendFrame(const QCanBusFrame& frame);

    /**
    *   @see ComponentInterface
    */
    void mainWidgetDockToggled(QWidget* widget) override;

public slots:
    /**
    *   @see ComponentInterface
    */
    void stopSimulation() override;
    /**
    *   @see ComponentInterface
    */
    void startSimulation() override;

private:
    QScopedPointer<CanRawSenderPrivate> d_ptr;
};

#endif // CANRAWSENDER_H
