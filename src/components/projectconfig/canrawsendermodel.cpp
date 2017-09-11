#include "canrawsendermodel.h"
#include "datamodeltypes/canrawsenderdata.h"
#include <nodes/DataModelRegistry>

CanRawSenderModel::CanRawSenderModel()
    : _label(new QLabel())
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    QWidget* crsWidget = _component.getMainWidget();
    crsWidget->setWindowTitle("CANrawSender");

    connect(&_component, &CanRawSender::sendFrame, this, &CanRawSenderModel::sendFrame);
}

QString CanRawSenderModel::caption() const
{
    return QString("CanRawSender Node");
} // TODO

QString CanRawSenderModel::name() const
{
    return QString("CanRawSenderModel");
}

std::unique_ptr<NodeDataModel> CanRawSenderModel::clone() const
{
    return std::make_unique<CanRawSenderModel>();
}

unsigned int CanRawSenderModel::nPorts(PortType portType) const
{
    return (PortType::Out == portType) ? 1 : 0;
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

QJsonObject CanRawSenderModel::save() const
{
    QJsonObject json;
    json["name"] = name();
    _component.saveSettings(json);
    return json;
}

void CanRawSenderModel::visit(CanNodeDataModelVisitor& v)
{
    v(*this);
}

QString CanRawSenderModel::modelName() const
{
    return QString("Raw sender");
}

QWidget* CanRawSenderModel::embeddedWidget()
{
    return _label;
}

bool CanRawSenderModel::resizable() const
{
    return false;
}

CanRawSender& CanRawSenderModel::getComponent()
{
    return _component;
}
