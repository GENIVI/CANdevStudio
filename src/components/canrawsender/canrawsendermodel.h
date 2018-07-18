#ifndef CANRAWSENDERMODEL_H
#define CANRAWSENDERMODEL_H

#include "canrawsender.h"
#include "componentmodel.h"
#include "nodepainter.h"
#include <QtSerialBus/QCanBusFrame>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

/**
 *   @brief The class provides node graphical representation of CanRawSender
 */
class CanRawSenderModel : public ComponentModel<CanRawSender, CanRawSenderModel> {
    Q_OBJECT

public:
    CanRawSenderModel();
    virtual ~CanRawSenderModel() = default;

    /**
     *   @brief  Used to get number of ports of each type used by model
     *   @param  type of port
     *   @return 1 if port out, 0 if any other type
     */
    unsigned int nPorts(PortType portType) const override;

    /**
     *   @brief  Used to get data type of each port
     *   @param  type of port
     *   @patam  port id
     *   @return CanDeviceDataOut type if portType is out
     */
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    /**
     *   @brief  Sets output data for propagation
     *   @param  port id
     *   @return CanDeviceDataOut filled with data
     */
    std::shared_ptr<NodeData> outData(PortIndex port) override;

    /**
     *   @brief  Handles data on input port, not used in this class
     *   @param  data on port
     *   @param  port id
     */
    void setInData(std::shared_ptr<NodeData>, PortIndex) override{};

    /**
     *   @brief Used to provide custom painter to nodeeditor
     *   @return NodePainterDelegate used to perform custom node painting
     */
    QtNodes::NodePainterDelegate* painterDelegate() const override;

public slots:

    /**
     *   @brief  Callback, called when CanRawSender emits signal sendFrame, sends frame
     *   @param  received frame
     */
    void sendFrame(const QCanBusFrame& frame);

signals:
    void requestRedraw();

private:
    QCanBusFrame _frame;
    std::unique_ptr<NodePainter> _painter;
};

#endif // CANRAWSENDERMODEL_H
