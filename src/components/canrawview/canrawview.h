
#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include "uibackend.h" // UsesUIBackend

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

class CanRawViewPrivate;
class QCanBusFrame;
class QCloseEvent;

/** -- this can go through the ctor if needed
[](CanRawView& v)
{
    assert(nullptr != v.d_ptr);
    auto widget = v.d_ptr->backend().getMainWidget();
    assert(nullptr != widget);

    widget->setLayout(widget->layout);
}
*/


class CanRawView
  : public UsesUIBackend<
                CanRawView
              , CanRawViewPrivate
              , CanRawView
              >
  , public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

 public:

    using UsesUIBackend::UsesUIBackend;

    virtual ~CANrawView() = default;  // if delete goes through QObject

    void closeEvent(QCloseEvent* e);

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

