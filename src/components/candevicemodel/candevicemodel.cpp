#include "candevicemodel.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <iostream>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "../datamodeltypes/rawsenderdata.h"
#include "../datamodeltypes/rawviewdata.h"

CanDeviceModel::CanDeviceModel()
    : label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    label->setFixedSize(75, 25);

    label->installEventFilter(this);

    label->setAttribute(Qt::WA_TranslucentBackground);

    canDevice = new CanDevice(factory);
    canDevice->init("socketcan", "can0");
    canDevice->start();
}

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
        break;
    }
}

std::shared_ptr<NodeData> CanDeviceModel::outData(PortIndex)
{
    return std::make_shared<RawViewData>(_frame, _direction);
}

void CanDeviceModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<RawSenderData>(nodeData);
        canDevice->sendFrame(d->frame());
        _frame = d->frame();
        _direction = "TX";
        emit dataUpdated(0);
    }
}
