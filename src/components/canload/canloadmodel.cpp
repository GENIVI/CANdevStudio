#include "canloadmodel.h"
#include <datamodeltypes/canrawdata.h>
#include <log.h>
#include "canloadplugin.h"

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

CanLoadModel::CanLoadModel()
    : ComponentModel("CanLoad")
    , _painter(std::make_unique<CanLoadPainter>(CanLoadPlugin::PluginType::sectionColor(), _currentLoad))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(this, &CanLoadModel::frameIn, &_component, &CanLoad::frameIn);
    connect(&_component, &CanLoad::currentLoad, this, &CanLoadModel::currentLoad);
}

QtNodes::NodePainterDelegate* CanLoadModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanLoadModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanLoadModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return {};
}

std::shared_ptr<NodeData> CanLoadModel::outData(PortIndex)
{
    return {};
}

void CanLoadModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanRawData>(nodeData);
        assert(nullptr != d);
        emit frameIn(d->frame());
    } else {
        cds_warn("Incorrect nodeData");
    }
}

void CanLoadModel::currentLoad(uint8_t load)
{
    bool valueChanged = _currentLoad != load;
    _currentLoad = load;

    if (valueChanged) {
        emit requestRedraw();
    }
}
