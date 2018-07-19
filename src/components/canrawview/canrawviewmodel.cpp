#include "canrawviewmodel.h"
#include <datamodeltypes/canrawdata.h>
#include <log.h>
#include "canrawviewplugin.h"

CanRawViewModel::CanRawViewModel()
    : ComponentModel("CanRawView")
    , _painter(std::make_unique<NodePainter>(CanRawViewPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    _component.mainWidget()->setWindowTitle(_caption);

    connect(this, &CanRawViewModel::frameSent, &_component, &CanRawView::frameSent);
    connect(this, &CanRawViewModel::frameReceived, &_component, &CanRawView::frameReceived);
}

QtNodes::NodePainterDelegate* CanRawViewModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanRawViewModel::nPorts(PortType portType) const
{
    return (PortType::In == portType) ? 1 : 0;
}

NodeDataType CanRawViewModel::dataType(PortType, PortIndex) const
{
    return CanRawData().type();
}

std::shared_ptr<NodeData> CanRawViewModel::outData(PortIndex)
{
    return std::make_shared<CanRawData>();
}

void CanRawViewModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanRawData>(nodeData);
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
