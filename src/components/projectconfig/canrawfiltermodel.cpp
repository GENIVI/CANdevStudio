#include "canrawfiltermodel.h"
#include <datamodeltypes/canrawfilterdata.h>
#include <log.h>

namespace {

// clang-format off
const std::map<PortType, std::vector<NodeDataType>> portMappings = {
    { PortType::In, 
        {
            //{CanSignalCoderDataIn{}.type() },
            //{CanSignalCoderSignalIn{}.type() },
            //{CanSignalCoderRawIn{}.type() }
        }
    },
    { PortType::Out, 
        {
            //{CanSignalCoderSignalOut{}.type()}, 
            //{CanSignalCoderRawOut{}.type() }
        }
    }
};
// clang-format on

} // namespace

CanRawFilterModel::CanRawFilterModel()
    : ComponentModel("CanRawFilter")
    , _painter(std::make_unique<NodePainter>(headerColor1(), headerColor2()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);
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
    return { };
}

std::shared_ptr<NodeData> CanRawFilterModel::outData(PortIndex)
{
    // example
    // return std::make_shared<CanDeviceDataOut>(_frame, _direction, _status);

    return { };
}

void CanRawFilterModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    // example
    // if (nodeData) {
    //     auto d = std::dynamic_pointer_cast<CanDeviceDataIn>(nodeData);
    //     assert(nullptr != d);
    //     emit sendFrame(d->frame());
    // } else {
    //     cds_warn("Incorrect nodeData");
    // }
    (void) nodeData;
}
