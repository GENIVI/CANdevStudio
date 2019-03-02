#include "canrawfiltermodel.h"
#include <datamodeltypes/canrawdata.h>
#include <log.h>
#include "canrawfilterplugin.h"

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, 
        {
            {CanRawData{}.type() }
        }
    },
    { PortType::Out, 
        {
            {CanRawData{}.type() }
        }
    }
};
// clang-format on

} // namespace

CanRawFilterModel::CanRawFilterModel()
    : ComponentModel("CanRawFilter")
    , _painter(std::make_unique<NodePainter>(CanRawFilterPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(this, &CanRawFilterModel::filterTx, &_component, &CanRawFilter::txFrameIn);
    connect(this, &CanRawFilterModel::filterRx, &_component, &CanRawFilter::rxFrameIn);
    connect(&_component, &CanRawFilter::txFrameOut, this, &CanRawFilterModel::filteredTx);
    connect(&_component, &CanRawFilter::rxFrameOut, this, &CanRawFilterModel::filteredRx);
}

QtNodes::NodePainterDelegate* CanRawFilterModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanRawFilterModel::nPorts(PortType portType) const
{
    return portMappings.at(portType).size();
}

NodeDataType CanRawFilterModel::dataType(PortType portType, PortIndex ndx) const
{
    if (portMappings.at(portType).size() > static_cast<uint32_t>(ndx)) {
        return portMappings.at(portType)[ndx];
    }

    cds_error("No port mapping for ndx: {}", ndx);
    return {};
}

std::shared_ptr<NodeData> CanRawFilterModel::outData(PortIndex)
{
    std::shared_ptr<NodeData> ret;
    bool status = _fwdQueue.try_dequeue(ret);

    if (!status) {
        cds_error("No data available on fwd queue");
        return {};
    }

    return ret;
}

void CanRawFilterModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanRawData>(nodeData);
        assert(nullptr != d);
        if (d->direction() == Direction::TX) {
            if (d->status()) {
                emit filterTx(d->frame());
            }
        } else if (d->direction() == Direction::RX) {
            emit filterRx(d->frame());
        } else {
            cds_warn("Incorrect direction");
        }
    } else {
        cds_warn("Incorrect nodeData");
    }
}

void CanRawFilterModel::filteredTx(const QCanBusFrame& frame)
{
    bool ret = _fwdQueue.try_enqueue(std::make_shared<CanRawData>(frame, Direction::TX));

    if (ret) {
        emit dataUpdated(0); // Data ready on port 0
    } else {
        cds_warn("Queue full. Frame dropped");
    }
}

void CanRawFilterModel::filteredRx(const QCanBusFrame& frame)
{
    bool ret = _fwdQueue.try_enqueue(std::make_shared<CanRawData>(frame, Direction::RX));

    if (ret) {
        emit dataUpdated(0); // Data ready on port 0
    } else {
        cds_warn("Queue full. Frame dropped");
    }
}
