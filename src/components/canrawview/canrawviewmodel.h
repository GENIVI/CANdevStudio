#ifndef CANRAWVIEWMODEL_H
#define CANRAWVIEWMODEL_H

#include "canrawview.h"
#include "componentmodel.h"
#include "nodepainter.h"
#include <QtSerialBus/QCanBusFrame>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

/**
 *   @brief The class provides node graphical representation of CanRawView
 */
class CanRawViewModel : public ComponentModel<CanRawView, CanRawViewModel> {
    Q_OBJECT

public:
    CanRawViewModel();
    virtual ~CanRawViewModel() = default;

    /**
     *   @brief  Used to get number of ports of each type used by model
     *   @param  type of port
     *   @return 1 if port in, 0 if any other type
     */
    unsigned int nPorts(PortType portType) const override;

    /**
     *   @brief  Used to get data type of each port
     *   @param  type of port
     *   @patam  port id
     *   @return CanDeviceDataOIn type if portType is out
     */
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    /**
     *   @brief  Sets output data for propagation, not used in this class
     *   @param  port id
     *   @return
     */
    std::shared_ptr<NodeData> outData(PortIndex port) override;

    /**
     *   @brief  Handles data on input port, send frames to CanRawView
     *   @param  data on port
     *   @param  port id
     */
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    /**
     *   @brief Used to provide custom painter to nodeeditor
     *   @return NodePainterDelegate used to perform custom node painting
     */
    QtNodes::NodePainterDelegate* painterDelegate() const override;

signals:
    /**
     *   @brief  Emits singal on CAN frame receival
     *   @param frame Received frame
     */
    void frameReceived(const QCanBusFrame& frame);

    /**
     *   @brief Emits signal on CAN fram transmission
     *   @param status true if frame has be sent successfuly
     *   @param frame Transmitted frame
     */
    void frameSent(bool status, const QCanBusFrame& frame);
    void requestRedraw();

private:
    QCanBusFrame _frame;
    std::unique_ptr<NodePainter> _painter;
};

#endif // CANRAWVIEWMODEL_H
