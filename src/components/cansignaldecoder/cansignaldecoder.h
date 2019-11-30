#ifndef CANSIGNALDECODER_H
#define CANSIGNALDECODER_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class CanSignalDecoderPrivate;
class QWidget;
typedef Context<> CanSignalDecoderCtx;

class CanSignalDecoder : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalDecoder)

public:
    CanSignalDecoder();
    explicit CanSignalDecoder(CanSignalDecoderCtx&& ctx);
    ~CanSignalDecoder();

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

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    QScopedPointer<CanSignalDecoderPrivate> d_ptr;
};

#endif //CANSIGNALDECODER_H
