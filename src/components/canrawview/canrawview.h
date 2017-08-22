#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QtCore/QScopedPointer>
#include <QtCore/QObject>
#include <memory>

class QCanBusFrame;
class CanRawViewPrivate;
class CRVFactoryInterface;

class CanRawView : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

public:
    explicit CanRawView();
    explicit CanRawView(CRVFactoryInterface& factory);
    ~CanRawView();

signals:
    void dockUndock();

public Q_SLOTS:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);
    void stopSimulation(void);
    void startSimulation(void);

private:
    QScopedPointer<CanRawViewPrivate> d_ptr;
};

#endif // CANRAWVIEW_H
