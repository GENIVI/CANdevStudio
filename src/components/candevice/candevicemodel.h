#ifndef CANDEVICEMODEL_H
#define CANDEVICEMODEL_H

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <QCanBusFrame>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <candevice/candevice.h>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

enum class Direction;

class CanDeviceModel : public NodeDataModel {
    Q_OBJECT

public:
    CanDeviceModel();

public:
    QString caption() const override { return QString("CanDevice Node"); } // TODO

    QString name() const override { return QString("CanDeviceModel"); }

    std::unique_ptr<NodeDataModel> clone() const override { return std::make_unique<CanDeviceModel>(); }

public:
    virtual QString modelName() const { return QString("CAN device"); }

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;

    QWidget* embeddedWidget() override { return label; }

    bool resizable() const override { return false; }

    void frameOnQueue();

private slots:

    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);

private:
    QLabel* label;

    std::shared_ptr<NodeData> _nodeData;
    QVector<std::tuple<QCanBusFrame, Direction, bool>> frameQueue;

    bool _status;

    Direction _direction;

    QCanBusFrame _frame;

    std::unique_ptr<CanDevice> canDevice;
};

#endif // CANDEVICEMODEL_H
