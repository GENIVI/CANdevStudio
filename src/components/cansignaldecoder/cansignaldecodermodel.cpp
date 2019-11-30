#include "cansignaldecodermodel.h"
#include "cansignaldecoderplugin.h"
#include <datamodeltypes/canrawdata.h>
#include <datamodeltypes/cansignalmodel.h>
#include <log.h>

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, 
        {
            { CanRawData{}.type() }
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

CanSignalDecoderModel::CanSignalDecoderModel()
    : ComponentModel("CanSignalDecoder")
    , _painter(std::make_unique<CanDbPainter>(CanSignalDecoderPlugin::PluginType::sectionColor(), &_component, 14))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(this, &CanSignalDecoderModel::sndFrame, &_component, &CanSignalDecoder::rcvFrame);
    connect(&_component, &CanSignalDecoder::sndSignal, this, &CanSignalDecoderModel::rcvSignal);
    connect(&_component, &CanSignalDecoder::requestRedraw, this, &CanSignalDecoderModel::requestRedraw);
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
    return {};
}

std::shared_ptr<NodeData> CanSignalDecoderModel::outData(PortIndex)
{
    std::shared_ptr<NodeData> ret;
    bool status = _rxQueue.try_dequeue(ret);

    if (!status) {
        cds_error("No data available on rx queue");
        return {};
    }

    return ret;
}

void CanSignalDecoderModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanRawData>(nodeData);
        assert(nullptr != d);
        emit sndFrame(d->frame(), d->direction(), d->status());
    } else {
        cds_warn("Incorrect nodeData");
    }
}

void CanSignalDecoderModel::rcvSignal(const QString& name, const QVariant& val, const Direction& dir)
{
    bool ret = _rxQueue.try_enqueue(std::make_shared<CanSignalModel>(name, val, dir));

    if (ret) {
        emit dataUpdated(0); // Data ready on port 0
    } else {
        cds_warn("Queue full. Frame dropped");
    }
}

