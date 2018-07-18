#include "canrawsendermodel.h"
#include <datamodeltypes/canrawdata.h>
#include "canrawsenderplugin.h"

CanRawSenderModel::CanRawSenderModel()
    : ComponentModel("CanRawSender")
    , _painter(std::make_unique<NodePainter>(CanRawSenderPlugin::PluginType::sectionColor()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&_component, &CanRawSender::sendFrame, this, &CanRawSenderModel::sendFrame);

    QWidget* crsWidget = _component.mainWidget();
    crsWidget->setWindowTitle(_caption);
}

QtNodes::NodePainterDelegate* CanRawSenderModel::painterDelegate() const
{
    return _painter.get();
}

NodeDataType CanRawSenderModel::dataType(PortType, PortIndex) const
{
    return CanRawData().type();
}

std::shared_ptr<NodeData> CanRawSenderModel::outData(PortIndex)
{
    return std::make_shared<CanRawData>(_frame);
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
