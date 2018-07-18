#include "canrawloggermodel.h"
#include <datamodeltypes/canrawdata.h>
#include <log.h>
#include "canrawloggerplugin.h"

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, 
        {
            {CanRawData{}.type() }
        }
    },
    { PortType::Out, { }
    }
};
// clang-format on

} // namespace

CanRawLoggerModel::CanRawLoggerModel()
    : ComponentModel("CanRawLogger")
    , _painter(std::make_unique<NodePainter>(CanRawLoggerPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(this, &CanRawLoggerModel::frameSent, &_component, &CanRawLogger::frameSent);
    connect(this, &CanRawLoggerModel::frameReceived, &_component, &CanRawLogger::frameReceived);
}

QtNodes::NodePainterDelegate* CanRawLoggerModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanRawLoggerModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanRawLoggerModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return {};
}

std::shared_ptr<NodeData> CanRawLoggerModel::outData(PortIndex)
{
    return {};
}

void CanRawLoggerModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
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
