#include "cansignalviewermodel.h"
#include "cansignalviewerplugin.h"
#include <datamodeltypes/cansignalmodel.h>
#include <log.h>

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, 
        {
            { CanSignalModel{}.type() }
        }
    },
    { PortType::Out, 
        {
        }
    }
};
// clang-format on

} // namespace

CanSignalViewerModel::CanSignalViewerModel()
    : ComponentModel("CanSignalViewer")
    , _painter(std::make_unique<NodePainter>(CanSignalViewerPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(this, &CanSignalViewerModel::sndSignal, &_component, &CanSignalViewer::rcvSignal);
}

QtNodes::NodePainterDelegate* CanSignalViewerModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanSignalViewerModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanSignalViewerModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return {};
}

std::shared_ptr<NodeData> CanSignalViewerModel::outData(PortIndex)
{
    return {};
}

void CanSignalViewerModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanSignalModel>(nodeData);
        assert(nullptr != d);
        emit sndSignal(d->name(), d->value(), d->direction());
    } else {
        cds_warn("Incorrect nodeData");
    }
}
