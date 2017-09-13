#include "canrawsendermodel.h"
#include <datamodeltypes/canrawsenderdata.h>

CanRawSenderModel::CanRawSenderModel()
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    QWidget* crsWidget = _component.getMainWidget();
    crsWidget->setWindowTitle("CANrawSender");

    connect(&_component, &CanRawSender::sendFrame, this, &CanRawSenderModel::sendFrame);

    _caption = "CanRawSender Node";
    _name = "CanRawSenderModel";
    _modelName = "Raw sender";
}

NodeDataType CanRawSenderModel::dataType(PortType, PortIndex) const
{
    return CanRawSenderDataOut().type();
}

std::shared_ptr<NodeData> CanRawSenderModel::outData(PortIndex)
{
    return std::make_shared<CanRawSenderDataOut>(_frame);
}

void CanRawSenderModel::sendFrame(const QCanBusFrame& frame)
{
    // TODO: Check if we don't need queue here. If different threads will operate on _frame we may loose data
    _frame = frame;
    emit dataUpdated(0); // Data ready on port 0
}

unsigned int CanRawSenderModel::nPorts(PortType portType) const
{
    return (PortType::Out == portType) ? 1 : 0;
}
