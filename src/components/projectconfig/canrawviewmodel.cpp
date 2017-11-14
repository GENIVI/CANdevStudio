#include "canrawviewmodel.h"
#include <datamodeltypes/canrawviewdata.h>
#include <log.h>

CanRawViewModel::CanRawViewModel()
    : ComponentModel("CanRawView")
    , _painter(std::make_unique<NodePainter>((NodePainterSettings){QColor(144, 187, 62), QColor(84, 84, 84)}))
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    _component.mainWidget()->setWindowTitle(_caption);

    connect(this, &CanRawViewModel::frameSent, &_component, &CanRawView::frameSent);
    connect(this, &CanRawViewModel::frameReceived, &_component, &CanRawView::frameReceived);

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

QtNodes::NodePainterDelegate* CanRawViewModel::painterDelegate() const
{
    return _painter.get();
}

unsigned int CanRawViewModel::nPorts(PortType portType) const
{
    return (PortType::In == portType) ? 1 : 0;
}

NodeDataType CanRawViewModel::dataType(PortType, PortIndex) const
{
    return CanRawViewDataIn().type();
}

std::shared_ptr<NodeData> CanRawViewModel::outData(PortIndex)
{
    return std::make_shared<CanRawViewDataIn>();
}

void CanRawViewModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex)
{
    if (nodeData) {
        auto d = std::dynamic_pointer_cast<CanRawViewDataIn>(nodeData);
        assert(nullptr != d);
        if (d->direction() == Direction::TX) {
            emit frameSent(d->status(), d->frame());
        } else if (d->direction() == Direction::RX) {
            emit frameReceived(d->frame());
        } else {
            cds_warn("Incorrect direction");
        }
    } else {
        cds_warn("Incorrect nodeData");
    }
}
