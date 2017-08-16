#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QCloseEvent>
#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>
#include <memory>

class QCanBusFrame;
class CanRawViewPrivate;

class CanRawView : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

public:
    explicit CanRawView(QWidget* parent = nullptr);
    ~CanRawView();
    void closeEvent(QCloseEvent* e);

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
