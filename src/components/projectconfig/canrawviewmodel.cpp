#include "canrawviewmodel.h"
#include <datamodeltypes/canrawviewdata.h>
#include <log.h>

CanRawViewModel::CanRawViewModel()
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    _caption = "CanRawView Node";
    _name = "CanRawViewModel";
    _modelName = "Raw view";

    _component.mainWidget()->setWindowTitle("CANrawView");
    connect(this, &CanRawViewModel::frameSent, &_component, &CanRawView::frameSent);
    connect(this, &CanRawViewModel::frameReceived, &_component, &CanRawView::frameReceived);
}

unsigned int CanRawViewModel::nPorts(PortType portType) const
{
    return (PortType::In == portType) ? 1 : 0;
}

NodeDataType CanRawViewModel::dataType(PortType, PortIndex) const
{
    return CanRawViewDataIn().type();
}

std::shared_ptr<NodeData> CanRawViewModel::outData(PortIndex)
{
    return std::make_shared<CanRawViewDataIn>();
}

void CanRawViewModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanRawViewDataIn>(nodeData);
        assert(nullptr != d);
        if (d->direction() == Direction::TX) {
            emit frameSent(d->status(), d->frame());
        } else if (d->direction() == Direction::RX) {
            emit frameReceived(d->frame());
        } else {
            cds_warn("Incorrect direction");
        }
    } else {
        cds_warn("Incorrect nodeData");
    }
}
