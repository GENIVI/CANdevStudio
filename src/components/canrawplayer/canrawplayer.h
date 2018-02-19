#ifndef CANRAWPLAYER_H
#define CANRAWPLAYER_H

#include <QtCore/QObject>
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
    void setConfig(const QObject& qobject) override;
    QJsonObject getConfig() const override;
    std::shared_ptr<QObject> getQConfig() const override;
    void configChanged() override;
    bool mainWidgetDocked() const override;
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

signals:
    void sendFrame(const QCanBusFrame& frame);
    void mainWidgetDockToggled(QWidget* widget) override;

public slots:
    void stopSimulation() override;
    void startSimulation() override;

private:
    QScopedPointer<CanRawPlayerPrivate> d_ptr;
};

#endif //CANRAWPLAYER_H
