#include "canrawplayermodel.h"
#include <datamodeltypes/canrawdata.h>
#include <log.h>
#include "canrawplayerplugin.h"

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, { }
    },
    { PortType::Out, 
        {
            {CanRawData{}.type() }
        }
    }
};
// clang-format on

} // namespace
    

CanRawPlayerModel::CanRawPlayerModel()
    : ComponentModel("CanRawPlayer")
    , _painter(std::make_unique<NodePainter>(CanRawPlayerPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&_component, &CanRawPlayer::sendFrame, this, &CanRawPlayerModel::sendFrame);
}

QtNodes::NodePainterDelegate* CanRawPlayerModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanRawPlayerModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanRawPlayerModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return { };
}

std::shared_ptr<NodeData> CanRawPlayerModel::outData(PortIndex)
{
    std::shared_ptr<NodeData> ret;
    bool status = _msgQueue.try_dequeue(ret);

    if (!status) {
        cds_error("No data available on rx queue");
        return {};
    }

    return ret;
}

void CanRawPlayerModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    (void) nodeData;
}

void CanRawPlayerModel::sendFrame(const QCanBusFrame& frame)
{
    bool ret = _msgQueue.try_enqueue(std::make_shared<CanRawData>(frame));

    if(ret) {
        emit dataUpdated(0); // Data ready on port 0
    } else {
        cds_warn("Queue full. Frame dropped");
    } 
}
