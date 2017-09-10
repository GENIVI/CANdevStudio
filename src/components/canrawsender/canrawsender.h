#ifndef CANRAWSENDER_H
#define CANRAWSENDER_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <context.h>

class QCanBusFrame;
class CanRawSenderPrivate;
class QWidget;

class CanRawSender : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawSender)

public:
    CanRawSender();
    explicit CanRawSender(CanRawSenderCtx&& ctx);
    ~CanRawSender();
    int getLineCount() const;
    void saveSettings(QJsonObject& json) const;

    QWidget* getMainWidget();

signals:
    void sendFrame(const QCanBusFrame& frame);
    void dockUndock();

public slots:
    void stopSimulation(void);
    void startSimulation(void);

private:
    QScopedPointer<CanRawSenderPrivate> d_ptr;
};

#endif // CANRAWSENDER_H
