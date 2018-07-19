#ifndef CANRAWFILTERMODEL_H
#define CANRAWFILTERMODEL_H

#include "canrawfilter.h"
#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <readerwriterqueue.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QCanBusFrame;
enum class Direction;

class CanRawFilterModel : public ComponentModel<CanRawFilter, CanRawFilterModel> {
    Q_OBJECT

public:
    CanRawFilterModel();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    QtNodes::NodePainterDelegate* painterDelegate() const override;

public slots:
    void filteredTx(const QCanBusFrame& frame);
    void filteredRx(const QCanBusFrame& frame);

signals:
    void filterTx(const QCanBusFrame& frame);
    void filterRx(const QCanBusFrame& frame);
    void requestRedraw();

private:
    std::unique_ptr<NodePainter> _painter;
    // 127 to use 4 blocks, 512 bytes each
    moodycamel::ReaderWriterQueue<std::shared_ptr<NodeData>> _fwdQueue{ 127 };
};

#endif // CANRAWFILTERMODEL_H
