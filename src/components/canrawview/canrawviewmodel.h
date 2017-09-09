#ifndef CANRAWVIEWMODEL_H
#define CANRAWVIEWMODEL_H

#include "modelvisitor.h" // CanNodeDataModelVisitor
#include "visitablewith.h"

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <canrawview.h>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <QCanBusFrame>

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

public:
    /**
    *   @brief  Used to get node caption
    *   @return Node caption
    */
    QString caption() const override
    {
        return QString("CanRawView Node");
    } // TODO

    /**
    *   @brief  Used to identify model by data model name
    *   @return Node model name
    */
    QString name() const override
    {
        return QString("CanRawViewModel");
    }

    std::unique_ptr<NodeDataModel> clone() const override
    {
        return std::make_unique<CanRawViewModel>();
    }

    /**
     * @brief Possibility to save node properties
     * @return json object
     */
    QJsonObject save() const override;

public:
    /** @see VisitableWith */
    virtual void visit(CanNodeDataModelVisitor& v) override
    {
        v(*this);
    }

    /**
    *   @brief  Used to get model name
    *   @return Model name
    */
    virtual QString modelName() const
    {
        return QString("Raw view");
    }

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
    QWidget* embeddedWidget() override
    {
        return label;
    }

    /**
    *   @brief  Used to get information if node is resizable
    *   @return false
    */
    bool resizable() const override
    {
        return false;
    }

    CanRawView canRawView;

private:
    QLabel* label;
    QCanBusFrame _frame;

signals:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);
};

#endif // CANRAWVIEWMODEL_H
