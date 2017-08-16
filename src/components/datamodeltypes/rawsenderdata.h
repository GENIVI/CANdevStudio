#ifndef RAWSENDERDATA_H
#define RAWSENDERDATA_H

#include <nodes/NodeDataModel>

#include <QCanBusFrame>

using QtNodes::NodeDataType;

class RawSenderData : public NodeData {
public:
    RawSenderData(){};
    RawSenderData(QCanBusFrame const& frame)
        : _frame(frame)
    {
    }

    NodeDataType type() const override { return NodeDataType{ "rawsender", "Raw" }; }

    QCanBusFrame frame() const { return _frame; };

private:
    QCanBusFrame _frame;
};

#endif // RAWSENDERDATA_H
