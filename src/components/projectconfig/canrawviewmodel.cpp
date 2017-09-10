#include "canrawviewmodel.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <datamodeltypes/canrawviewdata.h>
#include <log.h>
#include <nodes/DataModelRegistry>

CanRawViewModel::CanRawViewModel()
    : _label(new QLabel())
{
    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setFixedSize(75, 25);
    _label->setAttribute(Qt::WA_TranslucentBackground);

    _component.getMainWidget()->setWindowTitle("CANrawView");
    connect(this, &CanRawViewModel::frameSent, &_component, &CanRawView::frameSent);
    connect(this, &CanRawViewModel::frameReceived, &_component, &CanRawView::frameReceived);
}

QString CanRawViewModel::caption() const
{
    return QString("CanRawView Node");
} // TODO

QString CanRawViewModel::name() const
{
    return QString("CanRawViewModel");
}

std::unique_ptr<NodeDataModel> CanRawViewModel::clone() const
{
    return std::make_unique<CanRawViewModel>();
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

QJsonObject CanRawViewModel::save() const
{
    QJsonObject json;
    json["name"] = name();
    _component.saveSettings(json);
    return json;
}

void CanRawViewModel::visit(CanNodeDataModelVisitor& v)
{
    v(*this);
}

QWidget* CanRawViewModel::embeddedWidget()
{
    return _label;
}

bool CanRawViewModel::resizable() const
{
    return false;
}

QString CanRawViewModel::modelName() const
{
    return QString("Raw view");
}

CanRawView& CanRawViewModel::getComponent()
{
    return _component;
}

