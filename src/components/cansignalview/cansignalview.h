#ifndef CANSIGNALVIEW_H
#define CANSIGNALVIEW_H

#include <QtCore/QScopedPointer>
#include <QtWidgets/QWidget>

class QCanBusFrame;
class CanSignalViewPrivate;

class CanSignalView : public QWidget {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanSignalView)

public:
    explicit CanSignalView(QWidget* parent = 0);
    ~CanSignalView();

public Q_SLOTS:
    void signalReceived(const QString& name, const QByteArray& val);

private:
    QScopedPointer<CanSignalViewPrivate> d_ptr;
};

#endif // CANSIGNALVIEW_H
