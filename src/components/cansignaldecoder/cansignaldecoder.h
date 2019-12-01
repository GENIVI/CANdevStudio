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
class QCanBusFrame;
enum class Direction;

class CanSignalDecoder : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalDecoder)

public:
    CanSignalDecoder();
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
    void requestRedraw();
    void sndSignal(const QString& name, const QVariant& val, const Direction& dir);

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;
    void rcvFrame(const QCanBusFrame& frame, Direction const direction, bool status);

private:
    QScopedPointer<CanSignalDecoderPrivate> d_ptr;
};

#endif //CANSIGNALDECODER_H
