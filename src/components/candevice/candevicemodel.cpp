#include "candevicemodel.h"
#include <assert.h>
#include <datamodeltypes/candevicedata.h>
#include <log.h>
#include <nodes/DataModelRegistry>

CanDeviceModel::CanDeviceModel()
    : _label(new QLabel())
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&_component, &CanDevice::frameSent, this, &CanDeviceModel::frameSent);
    connect(&_component, &CanDevice::frameReceived, this, &CanDeviceModel::frameReceived);
    connect(this, &CanDeviceModel::sendFrame, &_component, &CanDevice::sendFrame);

    _component.init("socketcan", "can0"); // TODO
    _component.start();
}

unsigned int CanDeviceModel::nPorts(PortType portType) const
{
    assert((PortType::In == portType) || (PortType::Out == portType) || (PortType::None == portType)); // range check

    return (PortType::None != portType) ? 1 : 0;
}

void CanDeviceModel::frameOnQueue()
{
    std::tie(_frame, _direction, _status) = _frameQueue.takeFirst();
    emit dataUpdated(0); // Data ready on port 0
}

void CanDeviceModel::frameReceived(const QCanBusFrame& frame)
{
    _frameQueue.push_back(std::make_tuple(frame, Direction::RX, false));
    frameOnQueue();
}

void CanDeviceModel::frameSent(bool status, const QCanBusFrame& frame)
{
    _frameQueue.push_back(std::make_tuple(frame, Direction::TX, status));
    frameOnQueue();
}

NodeDataType CanDeviceModel::dataType(PortType portType, PortIndex) const
{
    assert((PortType::In == portType) || (PortType::Out == portType)); // allowed input

    return (PortType::Out == portType) ? CanDeviceDataOut{}.type() : CanDeviceDataIn{}.type();
}

std::shared_ptr<NodeData> CanDeviceModel::outData(PortIndex)
{
    return std::make_shared<CanDeviceDataOut>(_frame, _direction, _status);
}

void CanDeviceModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanDeviceDataIn>(nodeData);
        assert(nullptr != d);
        emit sendFrame(d->frame());
    } else {
        cds_warn("Incorrect nodeData");
    }
}

QJsonObject CanDeviceModel::save() const
{
    QJsonObject json;
    json["name"] = name();

    // TODO save can device settings
    // _component->saveSettings(json);

    return json;
}

void CanDeviceModel::visit(CanNodeDataModelVisitor& v)
{
    v(*this);
}

QString CanDeviceModel::caption() const
{
    return QString("CanDevice Node");
} // TODO

QString CanDeviceModel::name() const
{
    return QString("CanDeviceModel");
}

std::unique_ptr<NodeDataModel> CanDeviceModel::clone() const
{
    return std::make_unique<CanDeviceModel>();
}

QString CanDeviceModel::modelName() const
{
    return QString("CAN device");
}

QWidget* CanDeviceModel::embeddedWidget()
{
    return _label;
}

bool CanDeviceModel::resizable() const
{
    return false;
}
