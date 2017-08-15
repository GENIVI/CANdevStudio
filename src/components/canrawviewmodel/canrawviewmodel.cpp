#include "canrawviewmodel.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "../datamodeltypes/rawviewdata.h"
CanRawViewModel::CanRawViewModel()
    : label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    label->setFixedSize(75, 25);

    label->installEventFilter(this);

    label->setAttribute(Qt::WA_TranslucentBackground);
    canRawView.setWindowTitle("CANrawView test");
canRawView.startSimulation();

}

unsigned int CanRawViewModel::nPorts(PortType portType) const
{
    if (PortType::In == portType) {
        return 1;
    } else {
        return 0;
    }
}

NodeDataType CanRawViewModel::dataType(PortType, PortIndex) const { return RawViewData().type(); }

std::shared_ptr<NodeData> CanRawViewModel::outData(PortIndex) { return std::make_shared<RawViewData>(); }

void CanRawViewModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex) {
    if(nodeData)
    {

        auto d = std::dynamic_pointer_cast<RawViewData>(nodeData);
	if(d->direction() == "TX")
	{
		canRawView.frameSent(true, d->frame());
	}
    }
}

