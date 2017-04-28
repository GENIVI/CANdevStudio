#ifndef CANSRIPTER_H
#define CANSRIPTER_H

#include <QWidget>
#include <QScopedPointer>

class QCanBusFrame;
class CanScripterPrivate;

class CanScripter : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanScripter)

public:
    explicit CanScripter(QWidget *parent = 0);
    ~CanScripter();

Q_SIGNALS:
    void sendFrame(const QCanBusFrame &frame, const QVariant &context);
    void sendSignal(const QString &name, const QByteArray &value);

private:
    QScopedPointer<CanScripterPrivate> d_ptr;
};

#endif // CANSRIPTER_H

