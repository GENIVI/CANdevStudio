#ifndef CANRAWLOGGERMODEL_H
#define CANRAWLOGGERMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <canrawlogger.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QCanBusFrame;
enum class Direction;

class CanRawLoggerModel : public ComponentModel<CanRawLogger, CanRawLoggerModel> {
    Q_OBJECT

public:
    CanRawLoggerModel();

    unsigned int nPorts(PortType portType) const override;
    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;
    std::shared_ptr<NodeData> outData(PortIndex port) override;
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex port) override;
    QtNodes::NodePainterDelegate* painterDelegate() const override;

    static QColor headerColor1()
    {
        return QColor(144, 187, 62);
    }

    static QColor headerColor2()
    {
        return QColor(84, 84, 84);
    }

public slots:

signals:
    void frameReceived(const QCanBusFrame& frame);
    void frameSent(bool status, const QCanBusFrame& frame);

private:
    std::unique_ptr<NodePainter> _painter;
};

#endif // CANRAWLOGGERMODEL_H
