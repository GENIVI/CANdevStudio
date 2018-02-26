#ifndef CANLOADMODEL_H
#define CANLOADMODEL_H

#include "componentmodel.h"
#include "nodepainter.h"
#include <QtCore/QObject>
#include <canload.h>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::PortIndex;
using QtNodes::PortType;

enum class Direction;

class CanLoadModel : public ComponentModel<CanLoad, CanLoadModel> {
    Q_OBJECT

public:
    CanLoadModel();

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
    void frameIn(const QCanBusFrame& frame);

private:
    std::unique_ptr<NodePainter> _painter;
};

#endif // CANLOADMODEL_H
