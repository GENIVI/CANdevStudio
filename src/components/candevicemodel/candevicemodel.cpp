#include "candevicemodel.h"
#include <iostream>
#include <QtCore/QDir>
#include <QtCore/QEvent>

#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "../datamodeltypes/rawsenderdata.h"
#include "../datamodeltypes/rawviewdata.h"

CanDeviceModel::CanDeviceModel()
    : label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    label->setFixedSize(150, 100);

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
    }

    return result;
}

bool CanDeviceModel::eventFilter(QObject* object, QEvent* event) { return false; }

NodeDataType CanDeviceModel::dataType(PortType portType, PortIndex portIndex) const
{
    switch (portType) {
    case PortType::In:
        return RawSenderData().type();

    case PortType::Out:
        return RawViewData().type();
    }
}

std::shared_ptr<NodeData> CanDeviceModel::outData(PortIndex) {
return std::make_shared<RawSenderData>();
}

void CanDeviceModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex) {
       if(nodeData)
       {	       
	auto d = std::dynamic_pointer_cast<RawSenderData>(nodeData);
	canDevice->sendFrame(d->frame());
       }
}
