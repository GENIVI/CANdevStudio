#ifndef CANDEVICEMODEL_H
#define CANDEVICEMODEL_H

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <QCanBusFrame>
#include <nodes/DataModelRegistry>
#include <nodes/NodeDataModel>

#include <candevice/candevice.h>
#include <candevice/canfactoryqt.hpp>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

class CanDeviceModel : public NodeDataModel {
    Q_OBJECT

public:
    CanDeviceModel();

    virtual ~CanDeviceModel() {}

public:
    QString caption() const override { return QString("CanDevice Node"); }

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

private:
    QLabel* label;

    std::shared_ptr<NodeData> _nodeData;

    QString _direction;

    QCanBusFrame _frame;

    CanFactoryQt factory;

    CanDevice* canDevice;
};

#endif // CANDEVICEMODEL_H
