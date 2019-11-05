#ifndef CANSIGNALENCODER_H
#define CANSIGNALENCODER_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class CanSignalEncoderPrivate;
class QWidget;
struct CanSignalEncoderGuiInt;
typedef Context<> CanSignalEncoderCtx;

class CanSignalEncoder : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalEncoder)

public:
    CanSignalEncoder();
    explicit CanSignalEncoder(CanSignalEncoderCtx&& ctx);
    ~CanSignalEncoder();

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
    void simBcastSnd(const QJsonObject& msg, const QVariant& param = QVariant()) override;
    void requestRedraw();

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void simBcastRcv(const QJsonObject& msg, const QVariant& param) override;

private:
    QScopedPointer<CanSignalEncoderPrivate> d_ptr;
};

#endif // CANSIGNALENCODER_H
