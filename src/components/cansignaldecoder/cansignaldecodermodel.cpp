#include "cansignaldecodermodel.h"
#include "cansignaldecoderplugin.h"
#include <log.h>

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, 
        {
            //{CanRawData{}.type() }
        }
    },
    { PortType::Out, 
        {
            //{CanRawData{}.type() }
        }
    }
};
// clang-format on

} // namespace

CanSignalDecoderModel::CanSignalDecoderModel()
    : ComponentModel("CanSignalDecoder")
    , _painter(std::make_unique<NodePainter>(CanSignalDecoderPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);
}

QtNodes::NodePainterDelegate* CanSignalDecoderModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanSignalDecoderModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanSignalDecoderModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return { };
}

std::shared_ptr<NodeData> CanSignalDecoderModel::outData(PortIndex)
{
    // example
    // return std::make_shared<CanRawData>(_frame, _direction, _status);

    return { };
}

void CanSignalDecoderModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    // example
    // if (nodeData) {
    //     auto d = std::dynamic_pointer_cast<CanRawData>(nodeData);
    //     assert(nullptr != d);
    //     emit sendFrame(d->frame());
    // } else {
    //     cds_warn("Incorrect nodeData");
    // }
    (void) nodeData;
}
