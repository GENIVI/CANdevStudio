#include "cansignalencodermodel.h"
#include "cansignalencoderplugin.h"
#include <datamodeltypes/canrawdata.h>
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
            { CanRawData{}.type() }
        }
    }
};
// clang-format on

} // namespace

CanSignalEncoderModel::CanSignalEncoderModel()
    : ComponentModel("CanSignalEncoder")
    , _painter(std::make_unique<CanDbPainter>(CanSignalEncoderPlugin::PluginType::sectionColor(), &_component, 14))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(this, &CanSignalEncoderModel::sndSignal, &_component, &CanSignalEncoder::rcvSignal);
    connect(&_component, &CanSignalEncoder::sndFrame, this, &CanSignalEncoderModel::rcvFrame);
    connect(&_component, &CanSignalEncoder::requestRedraw, this, &CanSignalEncoderModel::requestRedraw);
}

QtNodes::NodePainterDelegate* CanSignalEncoderModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanSignalEncoderModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanSignalEncoderModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return {};
}

std::shared_ptr<NodeData> CanSignalEncoderModel::outData(PortIndex)
{
    std::shared_ptr<NodeData> ret;
    bool status = _rxQueue.try_dequeue(ret);

    if (!status) {
        cds_error("No data available on rx queue");
        return {};
    }

    return ret;
}

void CanSignalEncoderModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanSignalModel>(nodeData);
        assert(nullptr != d);
        emit sndSignal(d->name(), d->value());
    } else {
        cds_warn("Incorrect nodeData");
    }
}

void CanSignalEncoderModel::rcvFrame(const QCanBusFrame& frame)
{
    bool ret = _rxQueue.try_enqueue(std::make_shared<CanRawData>(frame));

    if (ret) {
        emit dataUpdated(0); // Data ready on port 0
    } else {
        cds_warn("Queue full. Frame dropped");
    }
}
