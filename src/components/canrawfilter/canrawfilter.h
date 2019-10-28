#ifndef CANRAWFILTER_H
#define CANRAWFILTER_H

#include <QWidget>
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
    void setConfig(const QWidget& qobject) override;
    QJsonObject getConfig() const override;
    std::shared_ptr<QWidget> getQConfig() const override;
    void configChanged() override;
    bool mainWidgetDocked() const override;
    ComponentInterface::ComponentProperties getSupportedProperties() const override;

signals:
    void mainWidgetDockToggled(QWidget* widget) override;
    void txFrameOut(const QCanBusFrame& frame);
    void rxFrameOut(const QCanBusFrame& frame);
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    void txFrameIn(const QCanBusFrame& frame);
    void rxFrameIn(const QCanBusFrame& frame);
    void stopSimulation() override;
    void startSimulation() override;
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    QScopedPointer<CanRawFilterPrivate> d_ptr;
};

#endif //CANRAWFILTER_H
