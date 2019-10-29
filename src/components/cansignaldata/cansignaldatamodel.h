#ifndef CANSIGNALDATAMODEL_H
#define CANSIGNALDATAMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <cansignaldata.h>
#include <candbpainter.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;

class CanSignalDataModel : public ComponentModel<CanSignalData, CanSignalDataModel> {
    Q_OBJECT

public:
    CanSignalDataModel();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData>, PortIndex) override{};
    QtNodes::NodePainterDelegate* painterDelegate() const override;

signals:
    void requestRedraw();

private:
    std::unique_ptr<CanDbPainter> _painter;
    CANmessages_t _messages;
};

#endif // CANSIGNALDATAMODEL_H
