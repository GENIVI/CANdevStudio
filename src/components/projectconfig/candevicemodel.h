#ifndef CANDEVICEMODEL_H
#define CANDEVICEMODEL_H

#include "componentmodel.h"
#include <QtCore/QObject>
#include <QtSerialBus/QCanBusFrame>
#include <candevice.h>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;

enum class Direction;

/**
*   @brief The class provides node graphical representation of CanDevice
*/
class CanDeviceModel : public ComponentModel<CanDevice, CanDeviceModel> {
    Q_OBJECT

public:
    CanDeviceModel();

    /**
    *   @brief  Used to get number of ports of each type used by model
    *   @param  type of port
    *   @return 1 if port in or out, 0 if any other type
    */
    unsigned int nPorts(PortType portType) const override;

    /**
    *   @brief  Used to get data type of each port
    *   @param  type of port
    *   @patam  port id
    *   @return CanDeviceDataOut type if portType is out, CanDeviceDataIn type if portType is in
    */
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    /**
    *   @brief  Sets output data for propagation
    *   @param  port id
    *   @return CanDeviceDataOut or CanDeviceDataIn filled with data
    */
    std::shared_ptr<NodeData> outData(PortIndex port) override;

    /**
    *   @brief  Handles data on input port, sends frame if correct
    *   @param  data on port
    *   @param  port id
    */
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    /**
    *   @brief Used to send frames that were put in queue
    */
    void frameOnQueue();

public slots:

    /**
    *   @brief  Callback, called when CanDevice emits signal frameReceived
    *   @param  received frame
    */
    void frameReceived(const QCanBusFrame& frame);

    /**
    *   @brief  Callback, called when CanDevice emits signal frameReceived
    *   @param  status indicating if sending frame was successful
    *   @param  sent frame
    */
    void frameSent(bool status, const QCanBusFrame& frame);

signals:
    /**
    *   @brief  Used to send a frame
    *   @param  frame Frame to be sent
    */
    void sendFrame(const QCanBusFrame& frame);

private:
    std::shared_ptr<NodeData> _nodeData;
    QVector<std::tuple<QCanBusFrame, Direction, bool>> _frameQueue;
    bool _status;
    Direction _direction;
    QCanBusFrame _frame;
};

#endif // CANDEVICEMODEL_H
