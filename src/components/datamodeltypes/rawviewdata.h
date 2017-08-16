#ifndef RAWVIEWDATA_H
#define RAWVIEWDATA_H

#include <nodes/NodeDataModel>

#include <QCanBusFrame>

using QtNodes::NodeDataType;

class RawViewData : public NodeData {
public:
    RawViewData(){};
    RawViewData(QCanBusFrame const& frame, QString const direction)
        : _frame(frame)
        , _direction(direction)
    {
    }

    NodeDataType type() const override { return NodeDataType{ "rawview", "Raw" }; }

    QCanBusFrame frame() const { return _frame; };
    QString direction() const { return _direction; };

private:
    QCanBusFrame _frame;
    QString _direction;
};

#endif // RAWVIEWDATA_H
