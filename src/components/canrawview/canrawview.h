
#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include "canrawview_p.h" // CanRawViewPrivate
#include "canrawviewbackend.hpp" // UIBackendDefault used in Uses...
#include "uibackend.h" // UsesUIBackend
#include "withexplicitinit.h" // EXPLICIT_INIT

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>


class CanRawViewPrivate;
class QCanBusFrame;
class QCloseEvent;
class QWidget;


class CanRawView
  : public QObject  // moc assumes first inherited is a subclass of it
  , public UsesUIBackend<
                CanRawView
              , CanRawViewPrivate
              , CanRawView
              >
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(UsesUIBackend::d_ptr.data(), CanRawView)
//                   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

 public:

    using UsesUIBackend::UsesUIBackend;

    virtual ~CanRawView() = default;  // if delete goes through QObject

    void closeEvent(QCloseEvent* e);
    QWidget* getMainWidget();

 signals:

    void dockUndock();

 public slots:

    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);
    void stopSimulation();
    void startSimulation();

 private:

    EXPLICIT_INIT(CanRawView)  // MUST be at the very end of the class!
//  ^^^^^^^^^^^^^^^^^^^^^^^^^
};

#endif // CANRAWVIEW_H

