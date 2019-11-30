#ifndef CANSIGNALDECODERMODEL_H
#define CANSIGNALDECODERMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <cansignaldecoder.h>
#include <candbpainter.h>
#include <readerwriterqueue.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;
class QCanBusFrame;

class CanSignalDecoderModel : public ComponentModel<CanSignalDecoder, CanSignalDecoderModel> {
    Q_OBJECT

public:
    CanSignalDecoderModel();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    QtNodes::NodePainterDelegate* painterDelegate() const override;

public slots:
    void rcvSignal(const QString& name, const QVariant& val, const Direction& dir);

signals:
    void requestRedraw();
    void sndFrame(const QCanBusFrame& frame, Direction const direction, bool status);

private:
    std::unique_ptr<CanDbPainter> _painter;
    // 127 to use 4 blocks, 512 bytes each
    moodycamel::ReaderWriterQueue<std::shared_ptr<NodeData>> _rxQueue{ 127 };
};

#endif // CANSIGNALDECODERMODEL_H
