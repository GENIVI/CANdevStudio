
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
    canRawView
    {
        UsesUIBackendCtor_ActionOrSelector
/*      , [](CanRawView& v)
        {
            assert(nullptr != v.impl());
            auto widget = v.impl()->backend().getMainWidget();
            assert(nullptr != widget);

            widget->setLayout(widget->layout());
        }*/
      , [](CanRawViewPrivate& v)
        {
            v._tvModel.setHorizontalHeaderLabels(v._columnsOrder);

            auto& ui = v.backend();

            ui.initTableView(v._tvModel);

            v._uniqueModel.setSourceModel(&v._tvModel);
            ui.setModel(&v._uniqueModel);

            ui.setClearCbk([&v]{ v.clear(); });
            ui.setDockUndockCbk([&v]{ v.dockUndock(); });
            ui.setSectionClikedCbk([&v](int index){ v.sort(index); });
            ui.setFilterCbk([&v]{ v.setFilter(); });
        }
    }
  , label(new QLabel())
{
    label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    label->setFixedSize(75, 25);
    label->setAttribute(Qt::WA_TranslucentBackground);

    assert(nullptr != canRawView.impl());
    assert(nullptr != canRawView.impl()->backend().getMainWidget());

    canRawView.impl()->backend().getMainWidget()->setWindowTitle("CANrawView");
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
