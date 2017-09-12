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
    int getLineCount() const;

    /**
    *   @see ComponentInterface
    */
    QWidget* getMainWidget() override;

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
    void setDockUndockClbk(const std::function<void()>& cb) override;

    /**
    *   @see ComponentInterface
    */
    bool mainWidgetDocked() const override;

signals:
    void sendFrame(const QCanBusFrame& frame);

public slots:
    void stopSimulation(void);
    void startSimulation(void);

private:
    QScopedPointer<CanRawSenderPrivate> d_ptr;
};

#endif // CANRAWSENDER_H
