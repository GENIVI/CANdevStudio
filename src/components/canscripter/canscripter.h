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
    explicit CanScripter(QObject* parent = 0);
    ~CanScripter();

Q_SIGNALS:
    void sendSignal(const QString& name, const QByteArray& value);

public Q_SLOTS:
    void setScriptFilename(const QString& scriptFile);
    void setScriptRepeat(bool repeat);
    void start();
    void stop();

private:
    QScopedPointer<CanScripterPrivate> d_ptr;
};

#endif // CANSRIPTER_H
