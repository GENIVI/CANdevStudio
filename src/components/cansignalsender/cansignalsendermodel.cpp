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
    , _painter(std::make_unique<CanDbPainter>(CanSignalSenderPlugin::PluginType::sectionColor(), &_component, 14))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&_component, &CanSignalSender::requestRedraw, this, &CanSignalSenderModel::requestRedraw);
    connect(&_component, &CanSignalSender::sendSignal, this, &CanSignalSenderModel::rcvSignal);

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
    return {};
}

std::shared_ptr<NodeData> CanSignalSenderModel::outData(PortIndex)
{
    std::shared_ptr<NodeData> ret;
    bool status = _rxQueue.try_dequeue(ret);

    if (!status) {
        cds_error("No data available on rx queue");
        return {};
    }

    return ret;
}

void CanSignalSenderModel::rcvSignal(const QString& name, const QVariant& val)
{
    bool ret = _rxQueue.try_enqueue(std::make_shared<CanSignalModel>(name, val));

    if (ret) {
        emit dataUpdated(0); // Data ready on port 0
    } else {
        cds_warn("Queue full. Frame dropped");
    }
}
