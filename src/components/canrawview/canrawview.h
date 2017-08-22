#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QtCore/QScopedPointer>
#include <QtCore/QObject>
#include <memory>  // unique_ptr

#include "uibackend.hpp"

class CanRawViewPrivate;
class QCanBusFrame;
class QWidget;

class CanRawView : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

public:
    explicit CanRawView();
    explicit CanRawView(std::unique_ptr<UIBackend<CanRawView>> backend);

    ~CanRawView() = default;

    QWidget* getMainWidget();

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
