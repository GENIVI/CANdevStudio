
#include "canrawviewmodel.h"
#include "canrawviewbackend.hpp"
#include "datamodeltypes/canrawviewdata.h"
#include "log.hpp"

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtWidgets/QFileDialog>
#include <nodes/DataModelRegistry>

#include <cassert> // assert

CanRawViewModel::CanRawViewModel()
  :
    CanRawView
    {
        [](CanRawView& v)
        {
            assert(nullptr != v.d_ptr);
            auto widget = v.d_ptr->backend().getMainWidget();
            assert(nullptr != widget);

            widget->setLayout(widget->layout);
        }
      , [](CanRawViewPrivate& v)
        {
            v._tvModel.setHorizontalHeaderLabels(v._columnsOrder);

            auto& ui = v.backend();

            ui.initTableView(_tvModel);
            ui.setClearCbk([&v]{ v.clear(); });
            ui.setDockUndockCbk([&v]{ v.dockUndock(); });
            ui.setSectionClikedCbk([&v](int index){ v.sort(index); });
        }
    }
  , label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    label->setFixedSize(75, 25);
    label->setAttribute(Qt::WA_TranslucentBackground);

    assert(nullptr != canRawView.d_ptr());
    assert(nullptr != canRawView.d_ptr()->backend().getMainWindow());

    canRawView.d_ptr->backend().getMainWindow()->setWindowTitle("CANrawView");
    connect(this, &CanRawViewModel::frameSent, &canRawView, &CanRawView::frameSent);
    connect(this, &CanRawViewModel::frameReceived, &canRawView, &CanRawView::frameReceived);
}

unsigned int CanRawViewModel::nPorts(PortType portType) const { return (PortType::In == portType) ? 1 : 0; }

NodeDataType CanRawViewModel::dataType(PortType, PortIndex) const { return CanRawViewDataIn().type(); }

std::shared_ptr<NodeData> CanRawViewModel::outData(PortIndex) { return std::make_shared<CanRawViewDataIn>(); }

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
