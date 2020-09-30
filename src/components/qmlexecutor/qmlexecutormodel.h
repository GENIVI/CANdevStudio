#ifndef QMLEXECUTORMODEL_H
#define QMLEXECUTORMODEL_H

#include <deque>

#include <QtCore/QObject>
#include <QtSerialBus/QCanBusFrame>


#include <datamodeltypes/canrawdata.h>
#include <datamodeltypes/cansignalmodel.h>
#include <qmlexecutor.h>
#include "componentmodel.h"
#include "nodepainter.h"
#include "canbusmodel.hpp"



using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;


/**
 * @brief The QMLExecutorModel A model for QML executor component
 * @see ComponentModel
 */
class QMLExecutorModel : public ComponentModel<QMLExecutor, QMLExecutorModel> {
    Q_OBJECT

public:
    /**
     * @brief only constructor to satisfy ql
     */
    QMLExecutorModel();

public: // ComponentModelInterface
    /// @see NodeDataModel::nPorts
    unsigned int nPorts(PortType portType) const override;

    /// @see NodeDataModel::dataType
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    /// @see NodeDataModel::outData
    std::shared_ptr<NodeData> outData(PortIndex port) override;

    /// @see NodeDataModel::setInData
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    /// @see NodeDataModel::painterDelegate
    QtNodes::NodePainterDelegate* painterDelegate() const override;

public:
    /**
    * @brief testing facility
    * @return can but model used to interface with QML
    */
    CANBusModel* getCANBusModel();

public slots:
    /**
     * @see CANBusModel
     */
    void sendFrame(const quint32& frameId, const QByteArray& frameData);

    /**
     * @see CANBusModel
     */
    void sendSignal(const QString& name, const QVariant& value);

signals:
    /**
     * @see CANBusModel
     */
    void frameReceived(const quint32& frameId, const QByteArray& frameData);

    /**
     * @brief A CAN signal was received
     * @param signal name
     * @param signal value
     */
    void signalReceived(const QString& name, const QVariant& value);

    /**
     * @brief not used currently
     */
    void simulationStarted();

    /**
     * @brief not used currently
     */
    void simulationStopped();

    /**
     * @brief not used currently
     */
    void requestRedraw();

private:
    /**
     * @brief Gets next frame to send
     * @return shared_ptr to frame or nullptr
     */
    std::shared_ptr<NodeData> getNextQueuedFrame();

    /**
     * @brief Gets next signal to send
     * @return shared_ptr to signal or nullptr
     */
    std::shared_ptr<NodeData> getNextQueuedSignal();

private:
    //! Componant stuff
    std::unique_ptr<NodePainter> _painter;

    //! ST queue to buffer frames to send
    std::deque<CanRawData> _rawSendQueue;

    //! ST queue to buffer signals to send
    std::deque<CanSignalModel> _signalSendQueue;

    //! can buf model to create interface with QML
    std::unique_ptr<CANBusModel> _CANBusModel;
};

#endif // QMLEXECUTORMODEL_H
