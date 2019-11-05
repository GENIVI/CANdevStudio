#ifndef CANSIGNALENCODERMODEL_H
#define CANSIGNALENCODERMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <cansignalencoder.h>
#include <candbpainter.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;

class CanSignalEncoderModel : public ComponentModel<CanSignalEncoder, CanSignalEncoderModel> {
    Q_OBJECT

public:
    CanSignalEncoderModel();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    QtNodes::NodePainterDelegate* painterDelegate() const override;

public slots:

signals:
    void requestRedraw();

private:
    std::unique_ptr<CanDbPainter> _painter;
};

#endif // CANSIGNALENCODERMODEL_H
