#ifndef CANSIGNALSENDER_H
#define CANSIGNALSENDER_H

#include <QWidget>
#include <QScopedPointer>

class QCanBusFrame;
class CanSignalSenderPrivate;

class CanSignalSender : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalSender)

public:
    explicit CanSignalSender(QWidget *parent = 0);
    ~CanSignalSender();

signals:
    void sendSignal(const QString &name, const QByteArray &value);

public slots:
    void start();
    void stop();

private:
    QScopedPointer<CanSignalSenderPrivate> d_ptr;
};

#endif // CANSIGNALSENDER_H
