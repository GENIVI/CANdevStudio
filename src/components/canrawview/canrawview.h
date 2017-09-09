#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <context.h>
#include <memory>

class QCanBusFrame;
class CanRawViewPrivate;
class QWidget;

class CanRawView : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

public:
    CanRawView();
    explicit CanRawView(CanRawViewCtx&& ctx);
    ~CanRawView();

    void saveSettings(QJsonObject& json) const;
    QWidget* getMainWidget();

signals:
    void dockUndock();

public slots:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);
    void stopSimulation(void);
    void startSimulation(void);

private:
    QScopedPointer<CanRawViewPrivate> d_ptr;
};

#endif // CANRAWVIEW_H
