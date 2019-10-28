#ifndef CANRAWPLAYER_H
#define CANRAWPLAYER_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class CanRawPlayerPrivate;
class QCanBusFrame;
class QWidget;
typedef Context<> CanRawPlayerCtx;

class CanRawPlayer : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawPlayer)

public:
    CanRawPlayer();
    explicit CanRawPlayer(CanRawPlayerCtx&& ctx);
    ~CanRawPlayer();

    QWidget* mainWidget() override;
    void setConfig(const QJsonObject& json) override;
    void setConfig(const QWidget& qobject) override;
    QJsonObject getConfig() const override;
    std::shared_ptr<QWidget> getQConfig() const override;
    void configChanged() override;
    bool mainWidgetDocked() const override;
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

signals:
    void sendFrame(const QCanBusFrame& frame);
    void mainWidgetDockToggled(QWidget* widget) override;
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    QScopedPointer<CanRawPlayerPrivate> d_ptr;
};

#endif //CANRAWPLAYER_H
