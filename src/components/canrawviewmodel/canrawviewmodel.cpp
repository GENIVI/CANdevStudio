#include "canrawviewmodel.h"
#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>

#include <nodes/DataModelRegistry>

#include "../datamodeltypes/rawviewdata.h"

CanRawViewModel::CanRawViewModel()
    : label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

    label->setFixedSize(150, 100);

    label->installEventFilter(this);

    label->setAttribute(Qt::WA_TranslucentBackground);

}

unsigned int CanRawViewModel::nPorts(PortType portType) const
{
    if (PortType::In == portType) {
        return 1;
    } else {
        return 0;
    }
}

bool CanRawViewModel::eventFilter(QObject* object, QEvent* event)
{
    if (object == label) {

        if (event->type() == QEvent::MouseButtonPress) {
            viewWindow.show();

            return true;
        }
    }
    return false;
}

NodeDataType CanRawViewModel::dataType(PortType, PortIndex) const { return RawViewData().type(); }

std::shared_ptr<NodeData> CanRawViewModel::outData(PortIndex) { return std::make_shared<RawViewData>(_frame); }

void CanRawViewModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) {
if(nodeData)
{
//
}
}

