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

    label->setFixedSize(150, 100);

    label->installEventFilter(this);

    label->setAttribute(Qt::WA_TranslucentBackground);

    connect(&senderWindow, &CanRawSender::sendFrame, this, &CanRawSenderModel::sendFrame);
}

unsigned int CanRawSenderModel::nPorts(PortType portType) const
{
    if (PortType::Out == portType) {
        return 1;
    } else {
        return 0;
    }
}

bool CanRawSenderModel::eventFilter(QObject* object, QEvent* event)
{
    if (object == label) {

        if (event->type() == QEvent::MouseButtonPress) {
            senderWindow.show();

            return true;
        }
    }
    return false;
}

NodeDataType CanRawSenderModel::dataType(PortType, PortIndex) const { return RawSenderData().type(); }

std::shared_ptr<NodeData> CanRawSenderModel::outData(PortIndex) { return std::make_shared<RawSenderData>(_frame); }

void CanRawSenderModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) {}

void CanRawSenderModel::sendFrame(const QCanBusFrame& frame)
{
    _frame = frame;
    emit dataUpdated(0);
}
