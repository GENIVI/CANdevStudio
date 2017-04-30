#ifndef CANSRIPTER_H
#define CANSRIPTER_H

#include <QScopedPointer>
#include <QObject>

class QCanBusFrame;
class CanScripterPrivate;

class CanScripter : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanScripter)

public:
    explicit CanScripter(const QString& scriptFile, QObject* parent = 0);
    ~CanScripter();

Q_SIGNALS:
    void sendFrame(const QCanBusFrame& frame, const QVariant& context);
    void sendSignal(const QString& name, const QByteArray& value);

public Q_SLOTS:
    void start();
    void stop();

private:
    QScopedPointer<CanScripterPrivate> d_ptr;
};

#endif // CANSRIPTER_H
