#ifndef CANSIGNALSENDER_H
#define CANSIGNALSENDER_H

#include <QtWidgets/QWidget>
#include <QtCore/QScopedPointer>

class QCanBusFrame;
class CanSignalSenderPrivate;

class CanSignalSender : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalSender)

public:
    explicit CanSignalSender(QWidget *parent = nullptr);
    ~CanSignalSender();

signals:
    void sendSignal(const QString &name, const QByteArray &value);

public slots:

private:
    QScopedPointer<CanSignalSenderPrivate> d_ptr;
};

#endif // CANSIGNALSENDER_H
