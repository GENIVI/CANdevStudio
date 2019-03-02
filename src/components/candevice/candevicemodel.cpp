#include "candevicemodel.h"
#include <assert.h>
#include <datamodeltypes/canrawdata.h>
#include <log.h>
#include "candeviceplugin.h"

CanDeviceModel::CanDeviceModel()
    : ComponentModel("CanDevice")
    , _status(false)
    , _direction(Direction::Uninitialized)
    , _painter(std::make_unique<NodePainter>(CanDevicePlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&_component, &CanDevice::frameSent, this, &CanDeviceModel::frameSent);
    connect(&_component, &CanDevice::frameReceived, this, &CanDeviceModel::frameReceived);
    connect(this, &CanDeviceModel::sendFrame, &_component, &CanDevice::sendFrame);
}

QtNodes::NodePainterDelegate* CanDeviceModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanDeviceModel::nPorts(PortType portType) const
{
    assert((PortType::In == portType) || (PortType::Out == portType) || (PortType::None == portType)); // range check

    return (PortType::None != portType) ? 1 : 0;
}

void CanDeviceModel::frameReceived(const QCanBusFrame& frame)
{
    bool ret = _rxQueue.try_enqueue(std::make_shared<CanRawData>(frame, Direction::RX));

    if(ret) {
        emit dataUpdated(0); // Data ready on port 0
    } else {
        cds_warn("Queue full. Frame dropped");
    } 
}

void CanDeviceModel::frameSent(bool status, const QCanBusFrame& frame)
{
    bool ret = _rxQueue.try_enqueue(std::make_shared<CanRawData>(frame, Direction::TX, status));

    if(ret) {
        emit dataUpdated(0); // Data ready on port 0
    } else {
        cds_warn("Queue full. Frame dropped");
    } 
}

NodeDataType CanDeviceModel::dataType(PortType portType, PortIndex) const
{
    assert((PortType::In == portType) || (PortType::Out == portType)); // allowed input

    return (PortType::Out == portType) ? CanRawData{}.type() : CanRawData{}.type();
}

std::shared_ptr<NodeData> CanDeviceModel::outData(PortIndex)
{
    std::shared_ptr<NodeData> ret;
    bool status = _rxQueue.try_dequeue(ret);

    if (!status) {
        cds_error("No data available on rx queue");
        return {};
    }

    return ret;
}

void CanDeviceModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanRawData>(nodeData);
        assert(nullptr != d);
        emit sendFrame(d->frame());
    } else {
        cds_warn("Incorrect nodeData");
    }
}
