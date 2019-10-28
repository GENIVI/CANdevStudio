#include "cansignaldatamodel.h"
#include "cansignaldataplugin.h"
#include <datamodeltypes/candbdata.h>
#include <log.h>

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, { }
    },
    { PortType::Out, { }
    }
};
// clang-format on

} // namespace

CanSignalDataModel::CanSignalDataModel()
    : ComponentModel("CanSignalData")
    , _painter(std::make_unique<NodePainter>(CanSignalDataPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 75);
    _label->setAttribute(Qt::WA_TranslucentBackground);
}

QtNodes::NodePainterDelegate* CanSignalDataModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanSignalDataModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanSignalDataModel::dataType(PortType, PortIndex) const
{
    return {};
}

std::shared_ptr<NodeData> CanSignalDataModel::outData(PortIndex)
{
    return {};
}

