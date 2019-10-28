#ifndef CANLOAD_H
#define CANLOAD_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class CanLoadPrivate;
class QWidget;
class QCanBusFrame;
typedef Context<> CanLoadCtx;

class CanLoad : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanLoad)

public:
    CanLoad();
    explicit CanLoad(CanLoadCtx&& ctx);
    ~CanLoad();

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
    void currentLoad(uint8_t load);
    void simBcastSnd(const QJsonObject &msg, const QVariant &param = QVariant()) override;

public slots:
    void stopSimulation() override;
    void startSimulation() override;
    void frameIn(const QCanBusFrame& frame);
    void simBcastRcv(const QJsonObject &msg, const QVariant &param) override;

private:
    QScopedPointer<CanLoadPrivate> d_ptr;
};

#endif //CANLOAD_H
