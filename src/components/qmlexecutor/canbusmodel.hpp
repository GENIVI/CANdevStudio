#ifndef CANBUSMODEL_HPP
#define CANBUSMODEL_HPP

#include <QObject>
#include <QCanBusFrame>

/**
 * @brief The CANBusModel class is used to interface CAN operations with QML stack
 */
class CANBusModel : public QObject
{
    Q_OBJECT

public:
    CANBusModel() : QObject(nullptr) {}

signals:
    /**
     * @brief Sends a CAN frame
     * @param frameId frame identifier
     * @param frameData frame payload
     */
    void sendFrame(const quint32& frameId, const QByteArray& frameData);

    /**
     * @brief Send a CAN named signal
     * @param name signal name
     * @param value signal value
     */
    void sendSignal(const QString& name, const QVariant& value);

    /**
     * @brief A CAN frame was received
     * @param frameId frame identifier
     * @param frameData frame payload
     */
    void frameReceived(const quint32& frameId, const QByteArray& frameData);

    /**
     * @brief A CAN signal was received
     * @param signal name
     * @param signal value
     */
    void signalReceived(const QString& name, const QVariant& value);

    /**
    * @brief The simulation was started
    */
    void simulationStarted();

    /**
     * @brief The simulation was stopped
     */
    void simulationStopped();
};

#endif // CANBUSMODEL_HPP
