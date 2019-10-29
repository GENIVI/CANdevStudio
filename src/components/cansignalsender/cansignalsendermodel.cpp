#include "cansignalsendermodel.h"
#include "cansignalsenderplugin.h"
#include <datamodeltypes/cansignalmodel.h>
#include <log.h>

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, 
        {
        }
    },
    { PortType::Out, 
        {
            { CanSignalModel{}.type() }
        }
    }
};
// clang-format on

} // namespace

CanSignalSenderModel::CanSignalSenderModel()
    : ComponentModel("CanSignalSender")
    , _painter(std::make_unique<CanDbPainter>(CanSignalSenderPlugin::PluginType::sectionColor(), &_component, 16))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&_component, &CanSignalSender::requestRedraw, this, &CanSignalSenderModel::requestRedraw);
}

QtNodes::NodePainterDelegate* CanSignalSenderModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanSignalSenderModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanSignalSenderModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return { };
}

std::shared_ptr<NodeData> CanSignalSenderModel::outData(PortIndex)
{
    // example
    // return std::make_shared<CanRawData>(_frame, _direction, _status);

    return { };
}

void CanSignalSenderModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
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
