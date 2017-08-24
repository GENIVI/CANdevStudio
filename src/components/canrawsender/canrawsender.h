#ifndef CANRAWSENDER_H
#define CANRAWSENDER_H

#include <QCloseEvent>
#include <QtCore/QScopedPointer>
#include <QtCore/QObject>

class QCanBusFrame;
class CanRawSenderPrivate;
class CRSFactoryInterface;
class QWidget;

class CanRawSender : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawSender)

public:
    explicit CanRawSender();
    explicit CanRawSender(CRSFactoryInterface& factory);
    ~CanRawSender();
    void closeEvent(QCloseEvent* e);

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
