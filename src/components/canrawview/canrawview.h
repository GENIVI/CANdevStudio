#ifndef CANRAWVIEW_H
#define CANRAWVIEW_H

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <componentinterface.h>
#include <context.h>
#include <memory>

class QCanBusFrame;
class CanRawViewPrivate;
class QWidget;

class CanRawView : public QObject, public ComponentInterface {
    Q_OBJECT
    Q_DECLARE_PRIVATE(CanRawView)

public:
    CanRawView();
    explicit CanRawView(CanRawViewCtx&& ctx);
    ~CanRawView();

    /**
    *   @see ComponentInterface
    */
    QWidget* getMainWidget() override;

    /**
    *   @see ComponentInterface
    */
    void setConfig(QJsonObject& json) override;

    /**
     *  @see ComponentInterface
     */
    void setConfig(const QObject& qobject) override;

    /**
    *   @see ComponentInterface
    */
    QJsonObject getConfig() const override;

    /**
    *   @see ComponentInterface
    */
    std::shared_ptr<QObject> getQConfig() const override;

    /**
    *   @see ComponentInterface
    */
    void setDockUndockClbk(const std::function<void()>& cb) override;

    /**
    *   @see ComponentInterface
    */
    bool mainWidgetDocked() const override;

public slots:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);
    void stopSimulation(void);
    void startSimulation(void);

private:
    QScopedPointer<CanRawViewPrivate> d_ptr;
};

#endif // CANRAWVIEW_H
