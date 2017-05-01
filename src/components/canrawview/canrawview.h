#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QScopedPointer>
#include <QWidget>

class QCanBusFrame;
class CanRawViewPrivate;

class CanRawView : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

public:
    explicit CanRawView(QWidget* parent = 0);
    ~CanRawView();

public Q_SLOTS:
    void start();
    void clear();
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame, const QVariant& context);

private:
    QScopedPointer<CanRawViewPrivate> d_ptr;
};

#endif // CANRAWVIEW_H
