#ifndef CANRAWFILTER_H
#define CANRAWFILTER_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class CanRawFilterPrivate;
class QWidget;
class QCanBusFrame;
struct CanRawFilterGuiInt;
typedef Context<CanRawFilterGuiInt> CanRawFilterCtx;

class CanRawFilter : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawFilter)

public:
    CanRawFilter();
    explicit CanRawFilter(CanRawFilterCtx&& ctx);
    ~CanRawFilter();

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
    void txFrameOut(const QCanBusFrame& frame);
    void rxFrameOut(const QCanBusFrame& frame);

public slots:
    void txFrameIn(const QCanBusFrame& frame);
    void rxFrameIn(const QCanBusFrame& frame);
    void stopSimulation() override;
    void startSimulation() override;

private:
    QScopedPointer<CanRawFilterPrivate> d_ptr;
};

#endif //CANRAWFILTER_H
