#include "canrawsendermodel.h"
#include "datamodeltypes/canrawsenderdata.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

CanRawSenderModel::CanRawSenderModel()
    : label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    label->setFixedSize(75, 25);

    label->setAttribute(Qt::WA_TranslucentBackground);

    QWidget* crsWidget = canRawSender.getMainWidget();
    crsWidget->setWindowTitle("CANrawSender");

    connect(&canRawSender, &CanRawSender::sendFrame, this, &CanRawSenderModel::sendFrame);
}

unsigned int CanRawSenderModel::nPorts(PortType portType) const { return (PortType::Out == portType) ? 1 : 0; }

NodeDataType CanRawSenderModel::dataType(PortType, PortIndex) const { return CanRawSenderDataOut().type(); }

std::shared_ptr<NodeData> CanRawSenderModel::outData(PortIndex)
{
    return std::make_shared<CanRawSenderDataOut>(_frame);
}

void CanRawSenderModel::setInData(std::shared_ptr<NodeData>, PortIndex) {}

void CanRawSenderModel::sendFrame(const QCanBusFrame& frame)
{
    _frame = frame;
    emit dataUpdated(0); // Data ready on port 0
}
