#ifndef CANDEVICEMODEL_H
#define CANDEVICEMODEL_H

#include <QtCore/QObject>
#include <QtSerialBus/QCanBusFrame>
#include <QtWidgets/QLabel>
#include <modelvisitor.h>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <visitablewith.h>

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
class CanDeviceModel : public NodeDataModel, public VisitableWith<CanNodeDataModelVisitor> {
    Q_OBJECT

public:
    CanDeviceModel();

    /** @see VisitableWith */
    virtual void visit(CanNodeDataModelVisitor& v) override;

    /**
    *   @brief  Used to get node caption
    *   @return Node caption
    */
    QString caption() const override;

    /**
    *   @brief  Used to identify model by data model name
    *   @return Node model name
    */
    QString name() const override;

    /**
    *   @brief Creates new node of the same type
    *   @return cloned node
    */
    std::unique_ptr<NodeDataModel> clone() const override;

    /**
     * @brief Possibility to save node properties
     * @return json object
     */
    QJsonObject save() const override;

    /**
    *   @brief  Used to get model name
    *   @return Model name
    */
    virtual QString modelName() const;

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
    QWidget* embeddedWidget() override;

    /**
    *   @brief  Used to get information if node is resizable
    *   @return false
    */
    bool resizable() const override;

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
    QLabel* _label;
    std::shared_ptr<NodeData> _nodeData;
    QVector<std::tuple<QCanBusFrame, Direction, bool>> _frameQueue;
    bool _status;
    Direction _direction;
    QCanBusFrame _frame;
    CanDevice _component;
};

#endif // CANDEVICEMODEL_H
