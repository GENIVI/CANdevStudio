#include "canrawsendermodel.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "../datamodeltypes/rawsenderdata.h"
CanRawSenderModel::CanRawSenderModel()
    : label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    label->setFixedSize(75, 25);

    label->installEventFilter(this);

    label->setAttribute(Qt::WA_TranslucentBackground);
}

unsigned int CanRawSenderModel::nPorts(PortType portType) const
{
    if (PortType::Out == portType) {
        return 1;
    } else {
        return 0;
    }
}

NodeDataType CanRawSenderModel::dataType(PortType, PortIndex) const { return RawSenderData().type(); }

std::shared_ptr<NodeData> CanRawSenderModel::outData(PortIndex) { return std::make_shared<RawSenderData>(_frame); }

void CanRawSenderModel::setInData(std::shared_ptr<NodeData> , PortIndex) {}

void CanRawSenderModel::sendFrame(const QCanBusFrame& frame)
{
    _frame = frame;
    emit dataUpdated(0);
}
