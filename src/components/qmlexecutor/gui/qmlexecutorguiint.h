#ifndef QMLEXECUTORGUIINT_H
#define QMLEXECUTORGUIINT_H

#include <Qt>
#include <QObject>
#include <QUrl>

class QWidget;
class CANBusModel;

/**
 * @brief Interface of gui for QMLExecutor plugin
 * @todo rename
 */
class QMLExecutorGuiInt : public QObject
{

    Q_OBJECT

public:
    /**
     * @brief constructor
     * @param parent qt stuff
     */
    QMLExecutorGuiInt(QObject* parent)
        : QObject(parent){};

    /**
     * @brief virt dtor to get things working
     */
    virtual ~QMLExecutorGuiInt()
    {
    }

public:
    /**
     * @brief not used, return main widget for ui
     */
    virtual QWidget* mainWidget() = 0;

public slots:
    /**
     * Loads qml file.
     *
     * @param url url to a qml file
     */
    virtual void loadQML(const QUrl& url) = 0;

    /**
     * @brief sets can bus model pointer
     * @param model can bus model
     */
    virtual void setModel(CANBusModel* model) = 0;

    /**
     * @brief used to align empty QQuickWidget color with current color scheme
     */
    virtual void updateUIColor() = 0;

signals:
    /**
     * @brief this signal is emited when qml was loaded
     * @param url url of qml file
     */
    void QMLLoaded(const QUrl& url);

    void dockUndock();
};


#endif // QMLEXECUTORGUIINT_H
