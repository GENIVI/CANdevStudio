#ifndef CANSIGNALSENDER_H
#define CANSIGNALSENDER_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class CanSignalSenderPrivate;
class QWidget;
struct CanSignalSenderGuiInt;
typedef Context<CanSignalSenderGuiInt> CanSignalSenderCtx;

class CanSignalSender : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalSender)

public:
    CanSignalSender();
    explicit CanSignalSender(CanSignalSenderCtx&& ctx);
    ~CanSignalSender();

    QWidget* mainWidget() override;
    void setConfig(const QJsonObject& json) override;
    void setConfig(const QWidget& qobject) override;
    QJsonObject getConfig() const override;
    std::shared_ptr<QWidget> getQConfig() const override;
    void configChanged() override;
    bool mainWidgetDocked() const override;
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

signals:
    void mainWidgetDockToggled(QWidget* widget) override;
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;
    void requestRedraw();
    void sendSignal(const QString& name, const QVariant& val);

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    QScopedPointer<CanSignalSenderPrivate> d_ptr;
};

#endif //CANSIGNALSENDER_H
