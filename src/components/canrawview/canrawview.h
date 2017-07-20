#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>
#include <memory>

class QCanBusFrame;
class CanRawViewPrivate;
class QElapsedTimer;

class CanRawView : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

public:
    explicit CanRawView(QWidget* parent = nullptr);
    ~CanRawView();

public Q_SLOTS:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame, const QVariant& context);
    void simulationStarted(void);
    void simulationStopped(void);

private:
    QScopedPointer<CanRawViewPrivate> d_ptr;
    std::unique_ptr<QElapsedTimer> timer;
    bool simStarted;
};

#endif // CANRAWVIEW_H
