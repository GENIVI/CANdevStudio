#ifndef CANDEVICEMODEL_H
#define CANDEVICEMODEL_H

#include "modelvisitor.h"  // CanNodeDataModelVisitor
#include "visitablewith.h"

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <QCanBusFrame>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <candevice.h>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

enum class Direction;

/**
*   @brief The class provides node graphical representation of CanDevice
*/
class CanDeviceModel
  : public NodeDataModel
  , public VisitableWith<CanNodeDataModelVisitor>
{
    Q_OBJECT

public:
    CanDeviceModel();

public:
    /** @see VisitableWith */
    virtual void visit(CanNodeDataModelVisitor& v) override { v(*this); }

    /**
    *   @brief  Used to get node caption
    *   @return Node caption
    */
    QString caption() const override { return QString("CanDevice Node"); } // TODO

    /**
    *   @brief  Used to identify model by data model name
    *   @return Node model name
    */
    QString name() const override { return QString("CanDeviceModel"); }

    std::unique_ptr<NodeDataModel> clone() const override { return std::make_unique<CanDeviceModel>(); }

public:
    /**
    *   @brief  Used to get model name
    *   @return Model name
    */
    virtual QString modelName() const { return QString("CAN device"); }

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
    *   @brief  Used to get widget embedded in Node
    *   @return QLabel
    */
    QWidget* embeddedWidget() override { return label; }

    /**
    *   @brief  Used to get information if node is resizable
    *   @return false
    */
    bool resizable() const override { return false; }

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

private:
    QLabel* label;

    std::shared_ptr<NodeData> _nodeData;

    QVector<std::tuple<QCanBusFrame, Direction, bool>> frameQueue;

    bool _status;

    Direction _direction;

    QCanBusFrame _frame;

    std::unique_ptr<CanDevice> canDevice;

signals:
    void sendFrame(const QCanBusFrame& frame);
};

#endif // CANDEVICEMODEL_H
