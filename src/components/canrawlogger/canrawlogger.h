#ifndef CANRAWLOGGER_H
#define CANRAWLOGGER_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class QCanBusFrame;
class CanRawLoggerPrivate;
class QWidget;
typedef Context<> CanRawLoggerCtx;

class CanRawLogger : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawLogger)

public:
    CanRawLogger();
    explicit CanRawLogger(CanRawLoggerCtx&& ctx);
    ~CanRawLogger();

    QWidget* mainWidget() override;
    void setConfig(const QJsonObject& json) override;
    void setConfig(const QObject& qobject) override;
    QJsonObject getConfig() const override;
    std::shared_ptr<QObject> getQConfig() const override;
    void configChanged() override;
    bool mainWidgetDocked() const override;
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

signals:
    void mainWidgetDockToggled(QWidget* widget) override;

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);

private:
    QScopedPointer<CanRawLoggerPrivate> d_ptr;
};

#endif //CANRAWLOGGER_H
