#include "canrawsendermodel.h"
#include <datamodeltypes/canrawsenderdata.h>

CanRawSenderModel::CanRawSenderModel()
    : ComponentModel("CanRawSender")
    , _painter(std::make_unique<NodePainter>(headerColor1(), headerColor2()))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&_component, &CanRawSender::sendFrame, this, &CanRawSenderModel::sendFrame);

    QWidget* crsWidget = _component.mainWidget();
    crsWidget->setWindowTitle(_caption);

    QColor bgColor = QColor(93, 93, 93);
    QtNodes::NodeStyle style;
    style.GradientColor0 = bgColor;
    style.GradientColor1 = bgColor;
    style.GradientColor2 = bgColor;
    style.GradientColor3 = bgColor;
    style.NormalBoundaryColor = bgColor;
    style.Opacity = 1.0;
    setNodeStyle(style);
}

QtNodes::NodePainterDelegate* CanRawSenderModel::painterDelegate() const
{
    return _painter.get();
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
