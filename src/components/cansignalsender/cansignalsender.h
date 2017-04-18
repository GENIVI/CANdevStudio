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

public slots:

private:
    QScopedPointer<CanSignalSenderPrivate> d_ptr;
};

#endif // CANSIGNALSENDER_H
