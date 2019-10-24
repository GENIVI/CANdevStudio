#include "cansignaldatamodel.h"
#include "cansignaldataplugin.h"
#include <datamodeltypes/candbdata.h>
#include <log.h>

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, { }
    },
    { PortType::Out, 
        {
            { CanDbData{}.type() }
        }
    }
};
// clang-format on

} // namespace

CanSignalDataModel::CanSignalDataModel()
    : ComponentModel("CanSignalData")
    , _painter(std::make_unique<NodePainter>(CanSignalDataPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&_component, &CanSignalData::canDbUpdated, this, &CanSignalDataModel::canDbUpdated);
}

QtNodes::NodePainterDelegate* CanSignalDataModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanSignalDataModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanSignalDataModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return {};
}

std::shared_ptr<NodeData> CanSignalDataModel::outData(PortIndex)
{
    return std::make_shared<CanDbData>(_messages);
}

void CanSignalDataModel::canDbUpdated(const CANmessages_t& messages)
{
    _messages = messages;
    emit dataUpdated(0); // Data ready on port 0
}
