#ifndef CANRAWVIEWMODEL_H
#define CANRAWVIEWMODEL_H

#include <QtCore/QObject>
#include <QtSerialBus/QCanBusFrame>
#include <QtWidgets/QLabel>
#include <canrawview.h>
#include <modelvisitor.h>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>
#include <visitablewith.h>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/**
*   @brief The class provides node graphical representation of CanRawView
*/
class CanRawViewModel : public NodeDataModel, public VisitableWith<CanNodeDataModelVisitor> {
    Q_OBJECT

public:
    CanRawViewModel();
    virtual ~CanRawViewModel() = default;

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

    /** @see VisitableWith */
    virtual void visit(CanNodeDataModelVisitor& v) override;

    /**
    *   @brief  Used to get model name
    *   @return Model name
    */
    virtual QString modelName() const;

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
    *   @brief Component getter
    *   @return Component managed by model
    */
    CanRawView& getComponent();

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

private:
    CanRawView _component;
    QLabel* _label;
    QCanBusFrame _frame;
};

#endif // CANRAWVIEWMODEL_H
