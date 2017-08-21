#include "candevicemodel.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>

#include "log.hpp"
#include <assert.h>

#include <QtWidgets/QFileDialog>

#include "datamodeltypes/candevicedata.h"
#include <nodes/DataModelRegistry>

CanDeviceModel::CanDeviceModel()
    : label(new QLabel())
    , canDevice(std::make_unique<CanDevice>())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    label->setFixedSize(75, 25);

    label->installEventFilter(this);

    label->setAttribute(Qt::WA_TranslucentBackground);

    connect(canDevice.get(), &CanDevice::frameSent, this, &CanDeviceModel::frameSent);
    connect(canDevice.get(), &CanDevice::frameReceived, this, &CanDeviceModel::frameReceived);

    canDevice->init("socketcan", "can0"); // TODO
    canDevice->start();
}

unsigned int CanDeviceModel::nPorts(PortType portType) const
{
    assert((PortType::In == portType) || (PortType::Out == portType) || (PortType::None == portType)); // range check

    return (PortType::None != portType) ? 1 : 0;
}

void CanDeviceModel::frameOnQueue()
{
    std::tie(_frame, _direction, _status) = frameQueue.takeFirst();
    emit dataUpdated(0); // Data ready on port 0
}

void CanDeviceModel::frameReceived(const QCanBusFrame& frame)
{
    frameQueue.push_back(std::make_tuple(frame, Direction::RX, false));
    frameOnQueue();
}

void CanDeviceModel::frameSent(bool status, const QCanBusFrame& frame)
{
    frameQueue.push_back(std::make_tuple(frame, Direction::TX, status));
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
        canDevice->sendFrame(d->frame());
    } else {
        cds_warn("Incorrect nodeData");
    }
}
