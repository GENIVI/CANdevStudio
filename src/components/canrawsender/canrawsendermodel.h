#ifndef CANRAWSENDERMODEL_H
#define CANRAWSENDERMODEL_H

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <canrawsender/canrawsender.h>

#include <QCanBusFrame>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/**
*   @brief The class provides node graphical representation of CanRawSender
*/
class CanRawSenderModel : public NodeDataModel {
    Q_OBJECT

public:
    CanRawSenderModel();

    virtual ~CanRawSenderModel() {}

public:
    /**
    *   @brief  Used to get node caption
    *   @return Node caption
    */
    QString caption() const override { return QString("CanRawSender Node"); } // TODO

    /**
    *   @brief  Used to identify model by data model name
    *   @return Node model name
    */
    QString name() const override { return QString("CanRawSenderModel"); }

    std::unique_ptr<NodeDataModel> clone() const override { return std::make_unique<CanRawSenderModel>(); }

public:
    /**
    *   @brief  Used to get model name
    *   @return Model name
    */
    virtual QString modelName() const { return QString("Raw sender"); }

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
    void setInData(std::shared_ptr<NodeData>, PortIndex) override;

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

    CanRawSender canRawSender;

private:
    QLabel* label;
    QCanBusFrame _frame;

public slots:

    /**
    *   @brief  Callback, called when CanRawSender emits signal sendFrame, sends frame
    *   @param  received frame
    */
    void sendFrame(const QCanBusFrame& frame);
};

#endif // CANRAWSENDERMODEL_H
