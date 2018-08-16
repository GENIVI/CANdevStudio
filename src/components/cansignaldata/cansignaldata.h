#ifndef CANSIGNALDATA_H
#define CANSIGNALDATA_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>
#include <cantypes.hpp>

class CanSignalDataPrivate;
class QWidget;
struct CanSignalDataGuiInt;
typedef Context<CanSignalDataGuiInt> CanSignalDataCtx;


class CanSignalData : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalData)

public:
    CanSignalData();
    explicit CanSignalData(CanSignalDataCtx&& ctx);
    ~CanSignalData();

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
    void canDbUpdated(const CANmessages_t& messages);

public slots:
    void stopSimulation() override;
    void startSimulation() override;

private:
    QScopedPointer<CanSignalDataPrivate> d_ptr;
};

#endif //CANSIGNALDATA_H
