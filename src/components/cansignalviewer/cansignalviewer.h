#ifndef CANSIGNALVIEWER_H
#define CANSIGNALVIEWER_H

#include <QWidget>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class CanSignalViewerPrivate;
class QWidget;
struct CanSignalViewerGuiInt;
typedef Context<CanSignalViewerGuiInt> CanSignalViewerCtx;
enum class Direction;

class CanSignalViewer : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalViewer)

public:
    CanSignalViewer();
    explicit CanSignalViewer(CanSignalViewerCtx&& ctx);
    ~CanSignalViewer();

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
    void rcvSignal(const QString& name, const QVariant& val, const Direction& dir);

private:
    QScopedPointer<CanSignalViewerPrivate> d_ptr;
};

#endif //CANSIGNALVIEWER_H
