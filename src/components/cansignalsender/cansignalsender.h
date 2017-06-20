#ifndef CANSIGNALSENDER_H
#define CANSIGNALSENDER_H

#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

class QCanBusFrame;
class CanSignalSenderPrivate;

class CanSignalSender : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalSender)

public:
    explicit CanSignalSender(QWidget* parent = nullptr);
    ~CanSignalSender();

signals:
    void sendSignal(const QString& name, const QByteArray& value);

public slots:

private:
    QScopedPointer<CanSignalSenderPrivate> d_ptr;
};

#endif // CANSIGNALSENDER_H
