#ifndef CANDEVICEMODEL_H
#define CANDEVICEMODEL_H

#include "candevice.h"
#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <QtSerialBus/QCanBusFrame>
#include <readerwriterqueue.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

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
     *   @brief Used to provide custom painter to nodeeditor
     *   @return NodePainterDelegate used to perform custom node painting
     */
    QtNodes::NodePainterDelegate* painterDelegate() const override;

    virtual bool hasSeparateThread() const override
    {
        return true;
    }

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
    void requestRedraw();

private:
    std::shared_ptr<NodeData> _nodeData;
    bool _status;
    Direction _direction;
    QCanBusFrame _frame;
    std::unique_ptr<NodePainter> _painter;
    // 127 to use 4 blocks, 512 bytes each
    moodycamel::ReaderWriterQueue<std::shared_ptr<NodeData>> _rxQueue{ 127 };
};

#endif // CANDEVICEMODEL_H
