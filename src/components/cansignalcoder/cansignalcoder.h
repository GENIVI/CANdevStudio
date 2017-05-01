#ifndef CANSIGNALCODER_H
#define CANSIGNALCODER_H

#include <QScopedPointer>
#include <QWidget>

class QCanBusFrame;
class CanSignalCoderPrivate;

class CanSignalCoder : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalCoder)

public:
    explicit CanSignalCoder(QObject* parent = 0);
    ~CanSignalCoder();

public Q_SLOTS:
    void frameReceived(const QCanBusFrame& frame);
    void signalReceived(const QString& name, const QByteArray& value);
    void clearFrameCache();

Q_SIGNALS:
    void sendFrame(const QCanBusFrame& frame, const QVariant& context);
    void sendSignal(const QString& name, const QByteArray& value);
    void signalEncoded(const QString& name, const QByteArray& value);

private:
    QScopedPointer<CanSignalCoderPrivate> d_ptr;
};

#endif // CANSIGNALCODER_H
