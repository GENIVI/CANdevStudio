#ifndef CANSIGNALDECODERMODEL_H
#define CANSIGNALDECODERMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <cansignaldecoder.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;

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

signals:
    void requestRedraw();

private:
    std::unique_ptr<NodePainter> _painter;
};

#endif // CANSIGNALDECODERMODEL_H
