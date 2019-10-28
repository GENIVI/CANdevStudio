#ifndef CANSIGNALDATA_H
#define CANSIGNALDATA_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <cantypes.hpp>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class CanSignalDataPrivate;
class QWidget;
struct CanSignalDataGuiInt;
typedef Context<CanSignalDataGuiInt> CanSignalDataCtx;

Q_DECLARE_METATYPE(CANmessages_t);

class CanSignalData : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalData)

public:
    CanSignalData();
    explicit CanSignalData(CanSignalDataCtx&& ctx);
    ~CanSignalData();

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
    QScopedPointer<CanSignalDataPrivate> d_ptr;
};

#endif // CANSIGNALDATA_H
