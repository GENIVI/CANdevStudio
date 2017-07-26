#ifndef CANRAWSENDER_H
#define CANRAWSENDER_H

#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

class QCanBusFrame;
class CanRawSenderPrivate;

class CanRawSender : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawSender)

public:
    explicit CanRawSender(QWidget* parent = nullptr);
    ~CanRawSender();

signals:
    void sendFrame(const QCanBusFrame& frame, const QVariant& context);
    void dockUndock();

public slots:

private:
    QScopedPointer<CanRawSenderPrivate> d_ptr;
};

#endif // CANRAWSENDER_H
