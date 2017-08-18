#include "candevicemodel.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "datamodeltypes/rawinputdata.h"
#include "datamodeltypes/rawoutputdata.h"

CanDeviceModel::CanDeviceModel()
    : label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    label->setFixedSize(75, 25);

    label->installEventFilter(this);

    label->setAttribute(Qt::WA_TranslucentBackground);

    canDevice = new CanDevice(factory);

    connect(canDevice, &CanDevice::frameSent, this, &CanDeviceModel::frameSent);
    connect(canDevice, &CanDevice::frameReceived, this, &CanDeviceModel::frameReceived);

    canDevice->init("socketcan", "can0");
    canDevice->start();
}

CanDeviceModel::~CanDeviceModel() { delete canDevice; }

unsigned int CanDeviceModel::nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType) {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 1;
        break;

    case PortType::None:
        result = 0;
        break;
    }

    return result;
}

void CanDeviceModel::frameReceived(const QCanBusFrame& frame)
{
    _frame = frame;
    _direction = "RX";
    emit dataUpdated(0);
}

void CanDeviceModel::frameSent(bool status, const QCanBusFrame& frame)
{
    _status = status;
    _frame = frame;
    _direction = "TX";
    emit dataUpdated(0);
}

NodeDataType CanDeviceModel::dataType(PortType portType, PortIndex) const
{
    switch (portType) {
    case PortType::In:
        return RawSenderData().type();
        break;

    case PortType::Out:
        return RawViewData().type();
        break;

    case PortType::None:
        return RawSenderData().type(); // dummy TODO
        break;
    }
    return RawSenderData().type(); // dummy TODO
}

std::shared_ptr<NodeData> CanDeviceModel::outData(PortIndex)
{
    return std::make_shared<RawViewData>(_frame, _direction, _status);
}

void CanDeviceModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<RawSenderData>(nodeData);
        canDevice->sendFrame(d->frame());
    }
}
